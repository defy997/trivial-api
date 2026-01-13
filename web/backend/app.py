from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware
import subprocess
import os
from filelock import FileLock, Timeout
from typing import List, Dict, Any


class Query(BaseModel):
    sql: str


app = FastAPI(title="TrivialDB Visual API")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# determine repository root and binary path
HERE = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TRIVIAL_BIN = os.path.join(REPO_ROOT, "build", "trivial_db")
LOCK_PATH = os.path.join(REPO_ROOT, "trivialdb.lock")


def execute_sql(sql: str, timeout: int = 30):
    # allow overriding path with environment variable, and check common alternate build locations
    env_bin = os.environ.get("TRIVIAL_BIN")
    if env_bin:
        bin_path = env_bin
    else:
        candidates = [
            os.path.join(REPO_ROOT, "build", "trivial_db"),
            os.path.join(REPO_ROOT, "build", "build", "trivial_db"),
            os.path.join(REPO_ROOT, "build", "trivial_db.exe"),
            os.path.join(REPO_ROOT, "build", "build", "trivial_db.exe"),
        ]
        bin_path = next((p for p in candidates if os.path.exists(p)), TRIVIAL_BIN)

    if not os.path.exists(bin_path):
        raise HTTPException(status_code=500, detail=f"trivial_db binary not found, tried: {bin_path}")

    payload = sql.strip()
    # ensure semicolon termination for the CLI parser
    if not payload.endswith(";"):
        payload += ";"
    # ensure we exit the CLI after running
    payload += "\nEXIT;\n"

    lock = FileLock(LOCK_PATH, timeout=10)
    try:
        with lock:
            proc = subprocess.run(
                [TRIVIAL_BIN],
                input=payload,
                text=True,
                capture_output=True,
                timeout=timeout,
            )
    except Timeout:
        raise HTTPException(status_code=503, detail="Database is busy (lock timeout)")
    except subprocess.TimeoutExpired:
        raise HTTPException(status_code=504, detail="Execution timed out")

    return proc


def _parse_table_text(text: str) -> Dict[str, Any]:
    """
    Parse a plain-text table output into structured {"columns": [...], "rows": [...]}
    Heuristics:
      - If lines contain '|' treat as ASCII table with separators
      - Else treat first line as header and whitespace-separated columns
      - Fallback to single-column lines
    """
    if not text:
        return {"columns": [], "rows": []}

    lines = [l.replace("\r", "").strip() for l in text.splitlines()]
    # filter empty and [exit] lines
    lines = [l for l in lines if l and not l.strip().lower().startswith("[exit]")]
    if not lines:
        return {"columns": [], "rows": []}

    # ASCII pipe table
    if any("|" in l for l in lines):
        # find header line index
        header_idx = next((i for i, l in enumerate(lines) if "|" in l), 0)
        header = [c.strip() for c in lines[header_idx].split("|") if c.strip()]
        data_lines = lines[header_idx + 1 :]
        # remove separator lines like ----
        data_lines = [l for l in data_lines if not all(ch == "-" or ch == " " for ch in l)]
        rows = []
        for dl in data_lines:
            cols = [c.strip() for c in dl.split("|") if c.strip()]
            obj = {}
            for i, h in enumerate(header):
                obj[h] = cols[i] if i < len(cols) else ""
            rows.append(obj)
        return {"columns": header, "rows": rows}

    # whitespace-separated: first line header
    if len(lines) >= 2:
        header = [c for c in lines[0].split() if c]
        rows = []
        for dl in lines[1:]:
            tokens = dl.split()
            obj = {}
            for i, h in enumerate(header):
                obj[h] = tokens[i] if i < len(tokens) else ""
            rows.append(obj)
        return {"columns": header, "rows": rows}

    # fallback single column
    return {"columns": ["value"], "rows": [{"value": l} for l in lines]}


@app.get("/tables")
def list_tables():
    """
    Return JSON list of table names.
    """
    proc = execute_sql("SHOW TABLE;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    # parse table names: try to extract word tokens from output lines
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        # skip header/separators
        if l.lower().startswith("name") or set(l) <= set("- "):
            continue
        # if pipe-separated, take tokens between pipes
        if "|" in l:
            parts = [p.strip() for p in l.split("|") if p.strip()]
            # if single column, it's a table name
            if len(parts) == 1:
                names.append(parts[0])
            else:
                # assume first column is name
                names.append(parts[0])
        else:
            # whitespace separated, take first token
            token = l.split()[0]
            names.append(token)
    # dedupe preserving order
    seen = set()
    out = []
    for n in names:
        if n not in seen:
            seen.add(n)
            out.append(n)
    return {"tables": out}


@app.get("/table/{name}")
def get_table(name: str, limit: int = 50):
    """
    Return structure and sample rows for a table.
    """
    sql = f"SELECT * FROM {name} LIMIT {limit};"
    proc = execute_sql(sql)
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    parsed = _parse_table_text(proc.stdout or proc.stderr or "")
    return {"table": name, "columns": parsed["columns"], "rows": parsed["rows"]}


@app.get("/health")
def health():
    return {"ok": True, "trivial_bin_exists": os.path.exists(TRIVIAL_BIN)}


@app.post("/query")
def query(q: Query):
    """
    执行一条 SQL（或多条以分号分隔），返回原始 stdout/stderr 与返回码。
    注意：目前后端直接将 SQL 发给本地 `build/trivial_db` 可执行文件。
    """
    proc = execute_sql(q.sql)

    def _clean(text: str) -> str:
        if not text:
            return ""
        lines = text.splitlines()
        filtered = [l for l in lines if not l.strip().lower().startswith("[exit]")]
        return "\n".join(filtered).strip()

    return {
        "returncode": proc.returncode,
        "stdout": _clean(proc.stdout),
        "stderr": _clean(proc.stderr),
        "raw_stdout": proc.stdout,
    }


@app.post("/exec")
def exec_sql(q: Query):
    """
    单纯别名，和 /query 相同，语义上可用于非查询命令（CREATE/INSERT/UPDATE/DELETE）。
    """
    proc = execute_sql(q.sql)

    def _clean(text: str) -> str:
        if not text:
            return ""
        lines = text.splitlines()
        filtered = [l for l in lines if not l.strip().lower().startswith("[exit]")]
        return "\n".join(filtered).strip()

    return {
        "returncode": proc.returncode,
        "stdout": _clean(proc.stdout),
        "stderr": _clean(proc.stderr),
        "raw_stdout": proc.stdout,
    }


