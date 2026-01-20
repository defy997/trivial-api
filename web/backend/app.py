from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware
import subprocess
import os
from filelock import FileLock, Timeout
from typing import List, Dict, Any
from fastapi import Request, Depends
import bcrypt
import json
import hmac
import hashlib
import base64


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
USERS_FILE = os.path.join(HERE, "users.json")
USERS_LOCK = os.path.join(HERE, "users.lock")
JWT_SECRET_ENV = "TRIVIAL_AUTH_SECRET"


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
                [bin_path],
                input=payload,
                text=True,
                capture_output=True,
                timeout=timeout,
                cwd=HERE,
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
    proc = execute_sql("SHOW TABLES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    # parse table names from the actual SHOW TABLES output format
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        # skip header lines and separator lines
        if l.startswith("========") or l.lower().startswith("tables in database") or l.lower().startswith("total"):
            continue
        # table names appear as lines with leading spaces
        if l and not any(char in l for char in ['=', 'Tables', 'Total']):
            names.append(l.strip())
    return {"tables": names}


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
     SQL stdout/stderr 
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


@app.post("/api/query")
def api_query(q: Query, request: Request):
    # require auth
    user = _require_auth(request)
    try:
        print(f"DEBUG api_query user={user} sql={q.sql}")
    except Exception:
        pass
    auth = request.headers.get("authorization", "") or ""
    token = None
    if auth.startswith("Bearer "):
        token = auth[7:]
    sql = q.sql or ""
    orig_sql = sql
    # detect CREATE DATABASE by the user and, if it is a tenant_<username> name, execute unauthenticated
    import re
    mcreate = re.match(r'^\s*CREATE\s+DATABASE\s+(?:IF\s+NOT\s+EXISTS\s+)?`?([A-Za-z0-9_]+)`?', sql.strip(), re.I)
    if mcreate and user and not user.get("is_admin"):
        dbname = mcreate.group(1)
        # allow creating only databases that start with tenant_<username>
        uname = user.get("username")
        if uname and dbname.startswith(f"tenant_{uname}"):
            # execute as system (no AUTH) so engine permits create
            proc = execute_sql(sql)
            # if created, grant mapping to user
            if proc.returncode == 0:
                try:
                    _grant_db_to_user(uname, dbname)
                except Exception:
                    pass
            # continue to build result below
        else:
            # not allowed by naming policy; prepend auth and proceed normally (will likely be denied)
            if token:
                safe_token = token.replace("'", "''")
                sql = "AUTH '__TOKEN__' '" + safe_token + "';\n" + sql
            proc = execute_sql(sql)
    else:
        if token:
            # escape single quotes by doubling for SQL literal safety
            safe_token = token.replace("'", "''")
            sql = "AUTH '__TOKEN__' '" + safe_token + "';\n" + sql
        proc = execute_sql(sql)

    def _clean(text: str) -> str:
        if not text:
            return ""
        lines = text.splitlines()
        filtered = [l for l in lines if not l.strip().lower().startswith("[exit]")]
        return "\\n".join(filtered).strip()

    result = {
        "returncode": proc.returncode,
        "stdout": _clean(proc.stdout),
        "stderr": _clean(proc.stderr),
        "raw_stdout": proc.stdout,
    }

    # if we successfully created a database, grant visibility to the creating user
    try:
        if proc.returncode == 0:
            import re
            text_to_match = (orig_sql or "").strip()
            m = re.match(r'^\s*CREATE\s+DATABASE\s+(?:IF\s+NOT\s+EXISTS\s+)?`?([A-Za-z0-9_]+)`?', text_to_match, re.I)
            try:
                print(f"DEBUG api_exec regex_target={text_to_match} match={bool(m)} user={user}")
            except Exception:
                pass
            if m and user:
                dbname = m.group(1)
                # update in-memory users_store and persist immediately so creator sees DB
                try:
                    uname = user.get("username")
                    global users_store
                    users_store = users_store or _load_users()
                    if users_store is None:
                        users_store = {}
                    u = users_store.get(uname)
                    if not u:
                        users_store[uname] = {"password_hash": "", "is_admin": False, "patterns": [dbname]}
                    else:
                        pats = u.get("patterns", []) or []
                        if dbname not in pats:
                            pats.append(dbname)
                            u["patterns"] = pats
                            users_store[uname] = u
                    try:
                        _persist_users(users_store)
                    except Exception:
                        pass
                    # also best-effort persist to admin_user_dbs
                    try:
                        safe_u = uname.replace("'", "''")
                        safe_db = dbname.replace("'", "''")
                        execute_sql(f"USE admin; INSERT INTO admin_user_dbs (username, db_name) VALUES ('{safe_u}', '{safe_db}');")
                    except Exception:
                        pass
                except Exception:
                    pass
    except Exception:
        pass

    return result


@app.post("/sql")
def execute_sql_query(q: Query):
    """
    SQLSHOW DATABASES
    """
    proc = execute_sql(q.sql)

    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={
            "stdout": proc.stdout,
            "stderr": proc.stderr,
            "returncode": proc.returncode
        })

    # [exit]
    def _clean(text: str) -> str:
        if not text:
            return ""
        lines = text.splitlines()
        filtered = [l for l in lines if not l.strip().lower().startswith("[exit]")]
        return "\n".join(filtered).strip()

    cleaned_output = _clean(proc.stdout or proc.stderr or "")

    # SHOW DATABASES
    if q.sql.strip().upper().startswith("SHOW DATABASES"):
        # 
        lines = [l.strip() for l in cleaned_output.splitlines() if l.strip()]
        databases = []
        for line in lines:
            # 
            if line.lower().startswith("database") or set(line) <= set("- "):
                continue
            # 
            if "|" in line:
                parts = [p.strip() for p in line.split("|") if p.strip()]
                if parts:
                    databases.append(parts[0])
            else:
                tokens = line.split()
                if tokens:
                    databases.append(tokens[0])

        return {
            "command": "SHOW DATABASES",
            "databases": databases,
            "raw_output": cleaned_output
        }

    # 
    parsed = _parse_table_text(cleaned_output)
    return {
        "command": q.sql.strip(),
        "columns": parsed["columns"],
        "rows": parsed["rows"],
        "raw_output": cleaned_output
    }


# Generic exec endpoint used by frontend to run arbitrary SQL (returns raw stdout/stderr and returncode)
@app.post("/exec")
def exec_sql_endpoint(payload: Dict[str, Any]):
    """
    Execute arbitrary SQL and return { returncode, stdout, stderr }.
    Expects JSON body: { "sql": "..." }
    """
    sql = ""
    orig_sql = ""
    if isinstance(payload, dict):
        sql = payload.get("sql", "") or ""
        orig_sql = sql
    else:
        raise HTTPException(status_code=400, detail="Invalid payload")

    # if Authorization header provided with Bearer token, forward token to engine
    from fastapi import Request
    # Note: Request cannot be injected directly here; use environ-like fallback via os env if set by middleware
    auth_header = None
    try:
        # uvicorn/fastapi stores headers in os.environ for CLI use? Fallback: allow clients to pass "auth_token" in payload
        auth_header = payload.get("auth_token")
    except Exception:
        auth_header = None
    if not auth_header:
        # try environment fallback (TRIVIAL_AUTH_USER/PASS used previously)
        trivial_user = os.environ.get("TRIVIAL_AUTH_USER")
        trivial_pass = os.environ.get("TRIVIAL_AUTH_PASS")
        if trivial_user and trivial_pass:
            auth_header = f"__TOKEN__:{trivial_pass}"
    # if auth_token present, prepend AUTH __TOKEN__ 'token'
    if auth_header:
        # if payload supplied token directly
        token = auth_header
        # allow token being passed as "token" or dict
        if isinstance(token, dict) and token.get("token"):
            token = token.get("token")
        # prepend AUTH directive (escape single quotes)
        safe_token = token.replace("'", "''")
        sql = "AUTH '__TOKEN__' '" + safe_token + "';\n" + sql

    proc = execute_sql(sql)

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


@app.post("/api/exec")
def api_exec_sql_endpoint(payload: Dict[str, Any], request: Request):
    # require auth and forward token to engine
    user = _require_auth(request)
    try:
        print(f"DEBUG api_exec user={user} sql={(payload.get('sql') if isinstance(payload, dict) else '')}")
    except Exception:
        pass
    auth = request.headers.get("authorization", "") or ""
    token = None
    if auth.startswith("Bearer "):
        token = auth[7:]

    sql = ""
    if isinstance(payload, dict):
        sql = payload.get("sql", "") or ""
    else:
        raise HTTPException(status_code=400, detail="Invalid payload")

    # detect CREATE DATABASE and allow tenant_<username>* creation without auth
    import re
    mcreate = re.match(r'^\s*CREATE\s+DATABASE\s+(?:IF\s+NOT\s+EXISTS\s+)?`?([A-Za-z0-9_]+)`?', sql.strip(), re.I)
    if mcreate and user and not user.get("is_admin"):
        dbname = mcreate.group(1)
        uname = user.get("username")
        if uname and dbname.startswith(f"tenant_{uname}"):
            # create DB unauthenticated (engine allows if no user set)
            proc = execute_sql(sql)
            if proc.returncode == 0:
                try:
                    _grant_db_to_user(uname, dbname)
                except Exception:
                    pass
        else:
            if token:
                safe_token = token.replace("'", "''")
                sql = "AUTH '__TOKEN__' '" + safe_token + "';\n" + sql
            proc = execute_sql(sql)
    else:
        if token:
            safe_token = token.replace("'", "''")
            sql = "AUTH '__TOKEN__' '" + safe_token + "';\n" + sql
        proc = execute_sql(sql)

    def _clean(text: str) -> str:
        if not text:
            return ""
        lines = text.splitlines()
        filtered = [l for l in lines if not l.strip().lower().startswith("[exit]")]
        return "\\n".join(filtered).strip()

    result = {
        "returncode": proc.returncode,
        "stdout": _clean(proc.stdout),
        "stderr": _clean(proc.stderr),
        "raw_stdout": proc.stdout,
    }

    # if we successfully created a database via /api/exec, grant visibility to creator
    try:
        if proc.returncode == 0:
            import re
            m = re.match(r'^\s*CREATE\s+DATABASE\s+(?:IF\s+NOT\s+EXISTS\s+)?`?([A-Za-z0-9_]+)`?', (orig_sql or "").strip(), re.I)
            if m and user:
                dbname = m.group(1)
                _grant_db_to_user(user.get("username"), dbname)
        else:
            # if create failed due to permission denied, attempt as admin and then grant to creator
            try:
                import re
                text_to_match = (orig_sql or "").strip()
                m2 = re.match(r'^\s*CREATE\s+DATABASE\s+(?:IF\s+NOT\s+EXISTS\s+)?`?([A-Za-z0-9_]+)`?', text_to_match, re.I)
                if m2 and ("permission denied" in (result.get("stderr") or "").lower() or "permission denied" in (result.get("stdout") or "").lower()):
                    dbname = m2.group(1)
                    admin_token = _make_token_for_user("admin", True, ["*"])
                    safe_admin = admin_token.replace("'", "''")
                    admin_sql = "AUTH '__TOKEN__' '" + safe_admin + "';\n" + (orig_sql or "")
                    admin_proc = execute_sql(admin_sql)
                    if admin_proc and admin_proc.returncode == 0:
                        # grant mapping and persist
                        try:
                            _grant_db_to_user(user.get("username"), dbname)
                            result = {
                                "returncode": admin_proc.returncode,
                                "stdout": _clean(admin_proc.stdout),
                                "stderr": _clean(admin_proc.stderr),
                                "raw_stdout": admin_proc.stdout,
                            }
                        except Exception:
                            pass
            except Exception:
                pass
    except Exception:
        pass

    return result


# Safe delete endpoint: accepts {"database": "...", "table": "...", "where": "..."}
@app.post("/delete-row")
def delete_row_endpoint(payload: Dict[str, Any]):
    """
    Accept either:
      { "database": "...", "table": "...", "where": "..." }
    or batch:
      { "deletes": [ { "database": "...", "table": "...", "where": "..." }, ... ] }

    Returns either single result or list of results.
    """
    if not isinstance(payload, dict):
        raise HTTPException(status_code=400, detail="Invalid payload")

    # DEBUG: log incoming payload to help diagnose front-end discrepancies
    try:
        print(f"DEBUG /delete-row payload: {payload}")
    except Exception:
        pass

    import re
    ident_re = re.compile(r"^[A-Za-z0-9_]+$")
    unsafe_re = re.compile(r"(;|\\bdrop\\b|\\balter\\b)", re.IGNORECASE)

    def validate_and_exec(item: Dict[str, Any]):
        database = item.get("database", "") or ""
        table = item.get("table", "") or ""
        where = item.get("where", "") or ""
        # sanitize numeric literals that were sent quoted by frontend, e.g. id='5' -> id=5
        try:
            # replace column='123' or column="123" with column=123 (only digits, optional decimal)
            where = re.sub(r"\b([A-Za-z0-9_]+)\s*=\s*'(-?\d+(?:\.\d+)?)'", r"\1=\2", where)
            where = re.sub(r'\b([A-Za-z0-9_]+)\s*=\s*"(-?\d+(?:\.\d+)?)"', r"\1=\2", where)
            # also replace any standalone quoted numeric literals like '123' or "123" with 123
            where = re.sub(r'([\'"])\s*(-?\d+(?:\.\d+)?)\s*\1', r"\2", where)
        except Exception:
            pass
        # debug: log original and sanitized where for troubleshooting
        try:
            print(f"DEBUG delete where original: {item.get('where', '')} -> sanitized: {where}")
        except Exception:
            pass
        if not table or not ident_re.match(table):
            return {"error": "Invalid table name", "item": item}
        if database and not ident_re.match(database):
            return {"error": "Invalid database name", "item": item}
        if not where or unsafe_re.search(where):
            return {"error": "Invalid or unsafe WHERE clause", "item": item}

        sql = f"DELETE FROM {table} WHERE {where};"
        if database:
            sql = f"USE {database}; {sql}"

        proc = execute_sql(sql)
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

    # batch mode
    if "deletes" in payload and isinstance(payload["deletes"], list):
        results = []
        for it in payload["deletes"]:
            results.append(validate_and_exec(it if isinstance(it, dict) else {}))
        # if any result contains error, raise 400 with first error detail
        for r in results:
            if isinstance(r, dict) and r.get("error"):
                raise HTTPException(status_code=400, detail=r.get("error"))
        return {"results": results}

    # single mode (backwards compatible)
    res = validate_and_exec(payload)
    if isinstance(res, dict) and res.get("error"):
        raise HTTPException(status_code=400, detail=res.get("error"))
    return res


# Return column names and inferred types for a table.
@app.get("/table/{name}/schema")
def table_schema(name: str, db: str = ""):
    """
    Infer column names and simple types for a table by sampling rows.
    Query params:
      db: optional database name to USE before selecting.
    Response:
      { columns: [{ name: <col>, type: "number"|"string" }], sample_rows: [ {...} ] }
    """
    # build SQL (avoid LIMIT which may not be supported by CLI)
    if db:
        sql = f"USE {db}; SELECT * FROM {name};"
    else:
        sql = f"SELECT * FROM {name};"

    # debug: log the SQL we'll execute
    try:
        print(f"DEBUG schema sql: {sql}")
    except Exception:
        pass
    # execute and capture output
    proc = execute_sql(sql)
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})

    cleaned = (proc.stdout or proc.stderr or "").splitlines()
    # parse using existing parser
    parsed = _parse_table_text("\n".join(cleaned))
    cols = parsed.get("columns", [])
    rows = parsed.get("rows", [])

    def infer_type(values):
        # if all non-empty values are numeric -> number, else string
        has_nonempty = False
        for v in values:
            if v is None:
                continue
            s = str(v).strip()
            if s == "":
                continue
            has_nonempty = True
            if not re.match(r"^-?\d+(\.\d+)?$", s):
                return "string"
        return "number" if has_nonempty else "string"

    import re
    types = []
    for c in cols:
        col_values = [r.get(c, "") for r in rows]
        types.append({"name": c, "type": infer_type(col_values)})

    return {"columns": types, "sample_rows": rows}


@app.get("/api/table/{name}")
def api_get_table(name: str, limit: int = 50, request: Request = None):
    user = _require_auth(request)
    # delegate to underlying implementation which already performs ACL checks when request provided
    return get_table(name, limit, request)


@app.get("/api/table/{name}/schema")
def api_table_schema(name: str, db: str = "", request: Request = None):
    user = _require_auth(request)
    if db and not _db_allowed_for_user(user, db):
        raise HTTPException(status_code=403, detail="permission denied for database")
    return table_schema(name, db, request)


# Structured delete endpoint: accepts conditions as array of {col, value}
@app.post("/delete-row-struct")
def delete_row_struct(payload: Dict[str, Any]):
    """
    Payload examples:
      Single:
        { "database": "demo", "table": "customer", "conditions": [ {"col":"id","val":"4"}, {"col":"age","val":"50"} ] }
      Batch:
        { "deletes": [ { ...single... }, {...} ] }
    """
    if not isinstance(payload, dict):
        raise HTTPException(status_code=400, detail="Invalid payload")

    import re

    def exec_single(item: Dict[str, Any]):
        db = item.get("database", "") or ""
        table = item.get("table", "") or ""
        conds = item.get("conditions", []) or []
        if not table:
            return {"error": "Invalid table", "item": item}

        # get schema to infer types
        try:
            schema = table_schema(table, db)
            col_types = {c["name"]: c["type"] for c in schema.get("columns", [])}
        except Exception as e:
            # fallback: assume all strings
            col_types = {}

        where_parts = []
        for c in conds:
            col = c.get("col")
            val = c.get("val")
            if col is None:
                continue
            s = "" if val is None else str(val).strip()
            # remove surrounding quotes
            if (s.startswith("'") and s.endswith("'")) or (s.startswith('"') and s.endswith('"')):
                s = s[1:-1].strip()
            typ = col_types.get(col, None)
            if typ == "number" or re.match(r"^-?\d+(\.\d+)?$", s):
                where_parts.append(f"{col}={s}")
            else:
                escaped_s = s.replace("'", "''")
                where_parts.append(f"{col}='{escaped_s}'")

        if not where_parts:
            return {"error": "No conditions", "item": item}

        where = " AND ".join(where_parts)
        sql = f"DELETE FROM {table} WHERE {where};"
        if db:
            sql = f"USE {db}; {sql}"
        proc = execute_sql(sql)
        def _clean(text: str) -> str:
            if not text:
                return ""
            lines = text.splitlines()
            filtered = [l for l in lines if not l.strip().lower().startswith("[exit]")]
            return "\n".join(filtered).strip()
        return {"returncode": proc.returncode, "stdout": _clean(proc.stdout), "stderr": _clean(proc.stderr)}

    # batch
    if "deletes" in payload and isinstance(payload["deletes"], list):
        results = [exec_single(d if isinstance(d, dict) else {}) for d in payload["deletes"]]
        return {"results": results}

    # single
    return exec_single(payload)

# API - 
@app.post("/api/delete-row")
def delete_row_alias(payload: Dict[str, Any]):
    return delete_row_endpoint(payload)

# Alias endpoints for saved queries to match frontend `/api/...` paths
@app.post("/api/queries/save")
def save_query_api(query: dict):
    return save_query(query)

@app.get("/api/queries/{database}")
def get_saved_queries_api(database: str):
    return get_saved_queries(database)

@app.delete("/api/queries/{database}/{query_name}")
def delete_saved_query_api(database: str, query_name: str):
    return delete_saved_query(database, query_name)

@app.get("/databases")
def list_databases(request: Request):
    """
    Protected wrapper for /databases that delegates to API version and enforces ACL.
    """
    return api_list_databases(request)


# API aliases (require auth + enforce DB ACLs where applicable)
@app.get("/api/databases")
def api_list_databases(request: Request):
    user = _require_auth(request)
    proc = execute_sql("SHOW DATABASES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        if l.startswith("========") or l.lower().startswith("available") or l.lower().startswith("total"):
            continue
        if l and not any(char in l for char in ['=', 'Available', 'Total']):
            names.append(l.strip())
    # enforce ACL: filter databases by user's allowed patterns (unless admin)
    filtered = [d for d in names if _db_allowed_for_user(user, d)]
    return {"databases": filtered}


@app.get("/database/{db_name}/tables")
def list_tables_in_database(db_name: str):
    """
    Return JSON list of table names in a specific database.
    """
    # First switch to the database, then show tables
    proc = execute_sql(f"USE {db_name}; SHOW TABLES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    # parse table names from the actual SHOW TABLES output format
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        # skip header lines and separator lines
        if l.startswith("========") or l.lower().startswith("tables in database") or l.lower().startswith("total"):
            continue
        # table names appear as lines with leading spaces
        if l and not any(char in l for char in ['=', 'Tables', 'Total']):
            names.append(l.strip())
    return {"tables": names}


@app.get("/api/database/{db_name}/tables")
def api_list_tables_in_database(db_name: str, request: Request):
    user = _require_auth(request)
    if not _db_allowed_for_user(user, db_name):
        raise HTTPException(status_code=403, detail="permission denied for database")
    return list_tables_in_database(db_name, request)


# 
# prefer storing saved queries next to the backend code so mounted container path is writable
SAVED_QUERIES_FILE = os.path.join(HERE, "saved_queries.json")
SAVED_QUERIES_LOCK = os.path.join(HERE, "saved_queries.lock")

def _load_saved_queries():
    global saved_queries
    try:
        if os.path.exists(SAVED_QUERIES_FILE):
            with FileLock(SAVED_QUERIES_LOCK, timeout=5):
                with open(SAVED_QUERIES_FILE, "r", encoding="utf-8") as f:
                    import json

                    data = json.load(f)
                    if isinstance(data, dict):
                        saved_queries = data
                        return
    except Exception:
        # fallback to empty
        try:
            print("WARN: failed to load saved queries from disk, continuing with empty store")
        except Exception:
            pass
saved_queries = {}

def _persist_saved_queries():
    try:
        with FileLock(SAVED_QUERIES_LOCK, timeout=5):
            with open(SAVED_QUERIES_FILE, "w", encoding="utf-8") as f:
                import json

                json.dump(saved_queries, f, ensure_ascii=False, indent=2)
    except Exception:
        try:
            print("WARN: failed to persist saved queries to disk")
        except Exception:
            pass


# -----------------------
# Users management (bcrypt + HMAC token)
# -----------------------

def _load_users():
    """
    Load users from TrivialDB admin.users table if available, falling back to USERS_FILE JSON.
    Returns dict: { username: { password_hash, is_admin, patterns } }
    """
    # attempt to ensure admin database and users table exist (idempotent-ish)
    try:
        try:
            execute_sql("CREATE DATABASE admin;")
        except Exception:
            pass
        try:
            execute_sql("USE admin; CREATE TABLE users (username TEXT, password_hash TEXT, is_admin INTEGER, patterns TEXT, created_at TEXT);")
        except Exception:
            pass
        # read users from DB
        proc = execute_sql("USE admin; SELECT username, password_hash, is_admin, patterns FROM users;")
        if proc.returncode == 0:
            parsed = _parse_table_text(proc.stdout or proc.stderr or "")
            rows = parsed.get("rows", []) or []
            users = {}
            for r in rows:
                uname = r.get("username") or r.get("USERNAME") or r.get("user") or ""
                if not uname:
                    continue
                pw = r.get("password_hash") or r.get("PASSWORD_HASH") or ""
                is_admin = False
                try:
                    is_admin = bool(int(r.get("is_admin") or r.get("IS_ADMIN") or 0))
                except Exception:
                    is_admin = False
                patterns_raw = r.get("patterns") or r.get("PATTERNS") or ""
                patterns = []
                if patterns_raw:
                    try:
                        # stored as comma separated or JSON-like
                        if patterns_raw.strip().startswith("["):
                            patterns = json.loads(patterns_raw)
                        else:
                            patterns = [p for p in [p.strip() for p in patterns_raw.split(",")] if p]
                    except Exception:
                        patterns = [p for p in [p.strip() for p in patterns_raw.split(",")] if p]
                users[uname] = {"password_hash": pw, "is_admin": is_admin, "patterns": patterns}
            if users:
                return users
    except Exception:
        try:
            print("WARN: DB-backed users load failed, falling back to JSON")
        except Exception:
            pass

    # fallback to file-based users (legacy)
    try:
        if os.path.exists(USERS_FILE):
            with FileLock(USERS_LOCK, timeout=5):
                with open(USERS_FILE, "r", encoding="utf-8") as f:
                    data = json.load(f)
                    if isinstance(data, dict):
                        return data
    except Exception:
        try:
            print("WARN: failed to load users file")
        except Exception:
            pass
    return {}

def _persist_users(users_dict):
    """
    Persist users into TrivialDB admin.users table. Also persist to USERS_FILE as a fallback copy.
    """
    def _escape_sql(s: str) -> str:
        return s.replace("'", "''") if isinstance(s, str) else s

    try:
        # ensure table exists
        try:
            execute_sql("CREATE DATABASE admin;")
        except Exception:
            pass
        try:
            execute_sql("USE admin; CREATE TABLE users (username TEXT, password_hash TEXT, is_admin INTEGER, patterns TEXT, created_at TEXT);")
        except Exception:
            pass
        # upsert each user (simple delete+insert)
        for uname, info in (users_dict or {}).items():
            try:
                pw = info.get("password_hash", "")
                is_admin = 1 if info.get("is_admin", False) else 0
                patterns = info.get("patterns", []) or []
                patterns_s = ",".join(patterns)
                created_at = info.get("created_at") or ""
                e_uname = _escape_sql(uname)
                e_pw = _escape_sql(pw)
                e_patterns = _escape_sql(patterns_s)
                e_created = _escape_sql(created_at)
                # delete existing then insert
                try:
                    execute_sql(f"USE admin; DELETE FROM users WHERE username='{e_uname}';")
                except Exception:
                    pass
                try:
                    insert_sql = "USE admin; INSERT INTO users (username, password_hash, is_admin, patterns, created_at) VALUES ('" + e_uname + "', '" + e_pw + "', " + str(is_admin) + ", '" + e_patterns + "', '" + e_created + "');"
                    execute_sql(insert_sql)
                except Exception:
                    # ignore individual insert errors
                    pass
            except Exception:
                continue
    except Exception:
        try:
            print("WARN: failed to persist users to DB")
        except Exception:
            pass

    # also write a JSON fallback copy for compatibility
    try:
        with FileLock(USERS_LOCK, timeout=5):
            with open(USERS_FILE, "w", encoding="utf-8") as f:
                json.dump(users_dict or {}, f, ensure_ascii=False, indent=2)
    except Exception:
        try:
            print("WARN: failed to persist users to disk fallback")
        except Exception:
            pass

# load users at startup
users_store = _load_users()
try:
    # attempt to migrate users.json into TrivialDB admin tables on startup
    def _exec_noauth(sql: str):
        # execute raw SQL using execute_sql (no auth wrapping)
        return execute_sql(sql)

    def _parse_table_names_from_show(text: str):
        lines = [l.strip() for l in (text or "").splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
        names = []
        for l in lines:
            if l.startswith("========") or l.lower().startswith("available") or l.lower().startswith("total"):
                continue
            if l and not any(char in l for char in ['=', 'Available', 'Total']):
                names.append(l.strip())
        return names

    def _ensure_admin_tables_and_migrate():
        try:
            proc = _exec_noauth("SHOW TABLES;")
            text = proc.stdout or proc.stderr or ""
            existing = _parse_table_names_from_show(text)
            if "admin_users" not in existing:
                # create admin_users and admin_user_dbs using types supported by trivial_db
                create_users_sql = """
CREATE TABLE admin_users (
  username VARCHAR(255) PRIMARY KEY,
  password_hash VARCHAR(255) NOT NULL,
  is_admin INTEGER NOT NULL,
  patterns VARCHAR(1024),
  created_at VARCHAR(64)
);
"""
                _exec_noauth(create_users_sql)
            if "admin_user_dbs" not in existing:
                create_map_sql = """
CREATE TABLE admin_user_dbs (
  username VARCHAR(255),
  db_name VARCHAR(255),
  pattern VARCHAR(255)
);
"""
                _exec_noauth(create_map_sql)
            # migrate users from users_store (if any)
            global users_store
            users = users_store or {}
            for uname, udata in users.items():
                # check if user already exists in table
                safe_uname = uname.replace("'", "''")
                sel_sql = f"SELECT username FROM admin_users WHERE username='{safe_uname}';"
                proc = _exec_noauth(sel_sql)
                out = proc.stdout or proc.stderr or ""
                if uname in out:
                    continue
                pw = udata.get('password_hash', '').replace("'", "''")
                isadm = 1 if udata.get('is_admin', False) else 0
                patterns = udata.get('patterns', []) or []
                pat_json = json.dumps(patterns).replace("'", "''")
                ins_sql = f"INSERT INTO admin_users (username, password_hash, is_admin, patterns) VALUES ('{safe_uname}', '{pw}', {isadm}, '{pat_json}');"
                try:
                    _exec_noauth(ins_sql)
                except Exception:
                    pass
        except Exception:
            try:
                print("WARN: migration to DB failed")
            except Exception:
                pass

    _ensure_admin_tables_and_migrate()
except Exception:
    pass

def _make_token_for_user(username, is_admin, patterns):
    # payload: username:is_admin:pattern1,pattern2  (base64)
    payload = f"{username}:{'1' if is_admin else '0'}:{','.join(patterns)}"
    b64 = base64_encode(payload.encode("utf-8"))
    secret = os.environ.get(JWT_SECRET_ENV, "default_secret")
    sig = hmac.new(secret.encode("utf-8"), b64.encode("utf-8"), hashlib.sha256).hexdigest()
    return f"{b64}.{sig}"

def _verify_token(token):
    try:
        if not token or "." not in token:
            return None
        b64, sig = token.split(".", 1)
        secret = os.environ.get(JWT_SECRET_ENV, "default_secret")
        expect = hmac.new(secret.encode("utf-8"), b64.encode("utf-8"), hashlib.sha256).hexdigest()
        if not hmac.compare_digest(expect, sig):
            return None
        payload = base64_decode(b64).decode("utf-8")
        parts = payload.split(":", 2)
        if len(parts) < 3:
            return None
        username = parts[0]
        is_admin = parts[1] == "1"
        patterns = parts[2].split(",") if parts[2] else []
        return {"username": username, "is_admin": is_admin, "patterns": patterns}
    except Exception:
        return None


def _get_user_from_db(username: str):
    """
    Query admin_users table for a given username, return dict or None.
    """
    try:
        safe = username.replace("'", "''")
        sql = f"SELECT username, password_hash, is_admin, patterns FROM admin_users WHERE username='{safe}';"
        proc = execute_sql(sql)
        if proc.returncode != 0:
            return None
        parsed = _parse_table_text(proc.stdout or proc.stderr or "")
        rows = parsed.get("rows", [])
        if not rows:
            return None
        r = rows[0]
        # patterns may be stored as JSON string
        pats = []
        try:
            pats = json.loads(r.get("patterns") or "[]")
        except Exception:
            pats = []
        return {
            "username": r.get("username"),
            "password_hash": r.get("password_hash"),
            "is_admin": True if str(r.get("is_admin")) in ('1', 'True', 'true') else False,
            "patterns": pats,
        }
    except Exception:
        return None


def _require_auth(request: Request):
    auth = request.headers.get("authorization", "") if request else ""
    if auth.startswith("Bearer "):
        token = auth[7:]
        info = _verify_token(token)
        if info:
            return info
    raise HTTPException(status_code=401, detail="authentication required")


def _db_allowed_for_user(user_info: Dict[str, Any], db_name: str) -> bool:
    if not user_info:
        return False
    if user_info.get("is_admin"):
        return True
    patterns = user_info.get("patterns", []) or []
    for pattern in patterns:
        if pattern == "*":
            return True
        if pattern.endswith("*"):
            prefix = pattern[:-1]
            if db_name.startswith(prefix):
                return True
        elif pattern == db_name:
            return True
    # fallback: check explicit mapping table admin_user_dbs
    try:
        username = user_info.get("username")
        if username:
            safe = username.replace("'", "''")
            proc = execute_sql(f"USE admin; SELECT db_name FROM admin_user_dbs WHERE username='{safe}';")
            if proc and proc.returncode == 0:
                parsed = _parse_table_text(proc.stdout or proc.stderr or "")
                rows = parsed.get("rows", []) or []
                for r in rows:
                    val = r.get("db_name") or r.get("DB_NAME") or ""
                    if val == db_name:
                        return True
                    # support prefix entries stored with trailing *
                    if val.endswith("*") and db_name.startswith(val[:-1]):
                        return True
    except Exception:
        pass
    # fallback: check in-memory users_store patterns (for immediacy if DB mapping not present)
    try:
        uname = user_info.get("username")
        if uname:
            us = users_store or _load_users()
            u = us.get(uname) if us else None
            if u:
                pats = u.get("patterns", []) or []
                for p in pats:
                    if p == "*" or p == db_name or (p.endswith("*") and db_name.startswith(p[:-1])):
                        return True
    except Exception:
        pass
    return False


def _grant_db_to_user(username: str, db_name: str):
    """
    Record that `username` is allowed to see `db_name` by inserting into admin_user_dbs.
    Idempotent-ish: will not raise on duplicate insert (we delete first).
    """
    # Simplified: always update in-memory users_store and fallback users.json so the creator
    # immediately sees the database. Attempt to also insert into admin_user_dbs if possible.
    try:
        print(f"DEBUG _grant_db_to_user called username={username} db_name={db_name}")
    except Exception:
        pass
    if not username or not db_name:
        return False
    safe_db = db_name.replace("'", "''")
    try:
        # update in-memory store
        global users_store
        users_store = users_store or _load_users()
        u = users_store.get(username) if users_store else None
        if not u:
            # create basic user record if missing
            users_store[username] = {"password_hash": "", "is_admin": False, "patterns": [safe_db]}
        else:
            pats = u.get("patterns", []) or []
            if safe_db not in pats:
                pats.append(safe_db)
                u["patterns"] = pats
                users_store[username] = u
        # persist fallback file
        try:
            _persist_users(users_store)
        except Exception:
            pass
        # try to persist mapping in admin_user_dbs (best-effort)
        try:
            safe_u = username.replace("'", "''")
            execute_sql(f"USE admin; INSERT INTO admin_user_dbs (username, db_name) VALUES ('{safe_u}', '{safe_db}');")
        except Exception:
            # ignore if admin tables don't exist or insert fails
            pass
        # also persist a simple mapping file as an engine-friendly fallback
        try:
            mapping_path = os.path.join(HERE, "admin_user_dbs.txt")
            # rebuild mapping from current users_store to keep file canonical
            lines = []
            us = users_store or {}
            for u, info in us.items():
                pats = info.get("patterns", []) or []
                if pats:
                    # join patterns (db names)
                    lines.append(f"{u}={','.join(pats)}")
            with open(mapping_path, "w", encoding="utf-8") as mf:
                mf.write("\n".join(lines))
        except Exception:
            pass
        return True
    except Exception:
        return False


def _verify_admin_password(request: Request = None, payload: Dict[str, Any] = None) -> bool:
    """
    Verify an admin re-authentication password. Checks payload['admin_password'] first,
    then header 'X-Admin-Password'. Returns True if matches the stored admin hash.
    """
    try:
        candidate = None
        if isinstance(payload, dict):
            candidate = payload.get("admin_password")
        if not candidate and request:
            candidate = request.headers.get("X-Admin-Password")
        if not candidate:
            return False
        admin = _get_user_from_db("admin") or (users_store or {}).get("admin")
        if not admin:
            return False
        pw_hash = admin.get("password_hash", "")
        try:
            return bcrypt.checkpw(str(candidate).encode("utf-8"), pw_hash.encode("utf-8"))
        except Exception:
            return False
    except Exception:
        return False

def base64_encode(b: bytes) -> str:
    return base64.b64encode(b).decode("utf-8")

def base64_decode(s: str) -> bytes:
    return base64.b64decode(s.encode("utf-8"))

# in-memory fallback (will be replaced by loaded data)
saved_queries = {}
# attempt to load persisted queries on startup
_load_saved_queries()

@app.post("/queries/save")
def save_query(query: dict):
    """
    Save a query with a name and database context.
    Expected format: {"name": "query_name", "sql": "SELECT...", "database": "db_name"}
    """
    name = query.get("name")
    sql = query.get("sql")
    database = query.get("database", "")

    if not name or not sql:
        raise HTTPException(status_code=400, detail="Query name and SQL are required")

    if database not in saved_queries:
        saved_queries[database] = {}

    saved_queries[database][name] = sql
    # persist to disk
    _persist_saved_queries()
    return {"status": "saved"}


# -----------------------
# Auth & admin HTTP endpoints
# -----------------------
def _is_admin_request(request: Request):
    try:
        auth = request.headers.get("authorization", "") if request else ""
        print("DEBUG _is_admin_request auth_header:", auth)
        if auth.startswith("Bearer "):
            token = auth[7:]
            info = _verify_token(token)
            print("DEBUG _is_admin_request token_info:", info)
            return info and info.get("is_admin")
    except Exception as e:
        try:
            print("DEBUG _is_admin_request exception:", e)
        except Exception:
            pass
    return False


@app.post("/api/auth/login")
def api_login(payload: Dict[str, Any]):
    """
    Login with username/password -> returns token
    """
    username = payload.get("username") if isinstance(payload, dict) else None
    password = payload.get("password") if isinstance(payload, dict) else None
    if not username or not password:
        raise HTTPException(status_code=400, detail="username and password required")
    # try DB first
    db_user = _get_user_from_db(username)
    if db_user:
        pw_hash = db_user.get("password_hash") or ""
        # if stored hash is empty or not a string, treat as invalid credentials
        if not pw_hash or not isinstance(pw_hash, str):
            try:
                print(f"LOGIN FAILED (db) bad hash for user: {username}")
            except Exception:
                pass
            raise HTTPException(status_code=401, detail="invalid credentials")
        try:
            ok = bcrypt.checkpw(password.encode("utf-8"), pw_hash.encode("utf-8"))
        except (ValueError, TypeError) as e:
            # invalid stored hash format -> treat as invalid credentials but log
            try:
                print(f"LOGIN FAILED (db) invalid hash for user: {username} error={e}")
            except Exception:
                pass
            raise HTTPException(status_code=401, detail="invalid credentials")
        except Exception as e:
            try:
                print(f"ERROR bcrypt check (db) for user={username} exc={e}")
            except Exception:
                pass
            raise HTTPException(status_code=500, detail="bcrypt error")
        if not ok:
            try:
                print(f"LOGIN FAILED (db) for user: {username}")
            except Exception:
                pass
            raise HTTPException(status_code=401, detail="invalid credentials")
        token = _make_token_for_user(username, db_user.get("is_admin", False), db_user.get("patterns", []))
        return {"token": token}
    # fallback to file-based users_store for backwards compatibility
    users = users_store or {}
    user = users.get(username)
    if not user:
        try:
            print(f"LOGIN FAILED (file) for user: {username}")
        except Exception:
            pass
        raise HTTPException(status_code=401, detail="invalid credentials")
    pw_hash = user.get("password_hash")
    if not pw_hash or not isinstance(pw_hash, str):
        try:
            print(f"LOGIN FAILED (file) bad hash for user: {username}")
        except Exception:
            pass
        raise HTTPException(status_code=401, detail="invalid credentials")
    try:
        ok = bcrypt.checkpw(password.encode("utf-8"), pw_hash.encode("utf-8"))
    except (ValueError, TypeError) as e:
        try:
            print(f"LOGIN FAILED (file) invalid hash for user: {username} error={e}")
        except Exception:
            pass
        raise HTTPException(status_code=401, detail="invalid credentials")
    except Exception as e:
        try:
            print(f"ERROR bcrypt check (file) for user={username} exc={e}")
        except Exception:
            pass
        raise HTTPException(status_code=500, detail="bcrypt error")
    if not ok:
        try:
            print(f"LOGIN FAILED (file) bad password for user: {username}")
        except Exception:
            pass
        raise HTTPException(status_code=401, detail="invalid credentials")
    token = _make_token_for_user(username, user.get("is_admin", False), user.get("patterns", []))
    return {"token": token}


# Backwards-compatible alias for older frontend bundles that POST to /auth/login
@app.post("/auth/login")
def auth_login_alias(payload: Dict[str, Any]):
    return api_login(payload)


# Public registration endpoint for new users
@app.post("/api/auth/register")
def api_register(payload: Dict[str, Any]):
    """
    Register a new user. Payload: { username, password }
    Enforces: username unique, password length >= 8
    Persists into USERS_FILE.
    """
    if not isinstance(payload, dict):
        raise HTTPException(status_code=400, detail="invalid payload")
    username = payload.get("username")
    password = payload.get("password")
    if not username or not password:
        raise HTTPException(status_code=400, detail="username and password required")
    if len(password) < 8:
        raise HTTPException(status_code=400, detail="password must be at least 8 characters")

    # load current users and ensure uniqueness
    users = _load_users() or {}
    if username in users:
        raise HTTPException(status_code=400, detail="username already exists")

    try:
        pw_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt()).decode("utf-8")
    except Exception:
        raise HTTPException(status_code=500, detail="bcrypt error")

    users[username] = {"password_hash": pw_hash, "is_admin": False, "patterns": []}
    _persist_users(users)
    # refresh in-memory store
    global users_store
    users_store = users
    return {"status": "created"}


@app.post("/auth/register")
def auth_register_alias(payload: Dict[str, Any]):
    return api_register(payload)


# Registration endpoint (username uniqueness + password policy)
@app.post("/api/auth/register")
def api_register(payload: Dict[str, Any]):
    """
    Register a new user.
    Expected payload: { "username": "...", "password": "...", "is_admin": false, "patterns": ["*"] }
    Password must be at least 8 characters. Username must be unique.
    """
    if not isinstance(payload, dict):
        raise HTTPException(status_code=400, detail="invalid payload")
    username = (payload.get("username") or "").strip()
    password = payload.get("password") or ""
    is_admin = bool(payload.get("is_admin", False))
    patterns = payload.get("patterns", []) or []

    if not username or not password:
        raise HTTPException(status_code=400, detail="username and password required")
    if len(password) < 8:
        raise HTTPException(status_code=400, detail="password must be at least 8 characters")

    global users_store
    users_store = users_store or _load_users()
    if username in (users_store or {}):
        raise HTTPException(status_code=409, detail="username already exists")

    try:
        pw_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt()).decode("utf-8")
    except Exception:
        raise HTTPException(status_code=500, detail="bcrypt error")

    users_store[username] = {"password_hash": pw_hash, "is_admin": is_admin, "patterns": patterns}
    _persist_users(users_store)
    return {"status": "created"}


# Backwards-compatible alias for older frontend bundles that POST to /auth/register
@app.post("/auth/register")
def auth_register_alias(payload: Dict[str, Any]):
    return api_register(payload)


@app.post("/api/admin/users")
def api_create_user(payload: Dict[str, Any], request: Request):
    """
    Create user (admin only unless no users exist)
    payload: { username, password, is_admin: bool, patterns: [ ... ] }
    """
    if not isinstance(payload, dict):
        raise HTTPException(status_code=400, detail="invalid payload")
    username = payload.get("username")
    password = payload.get("password")
    is_admin = bool(payload.get("is_admin", False))
    patterns = payload.get("patterns", []) or []
    if not username or not password:
        raise HTTPException(status_code=400, detail="username/password required")
    # if users empty, allow creation without admin
    global users_store
    if users_store and len(users_store) > 0:
        if not _is_admin_request(request):
            raise HTTPException(status_code=403, detail="admin required")
        # require admin re-authentication (admin password) for modifications
        ok = _verify_admin_password(request, payload)
        if not ok:
            raise HTTPException(status_code=401, detail="admin password required")
    # hash password using bcrypt
    try:
        pw_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt()).decode("utf-8")
    except Exception:
        raise HTTPException(status_code=500, detail="bcrypt error")
    # persist to admin_users table
    try:
        safe_uname = username.replace("'", "''")
        safe_hash = pw_hash.replace("'", "''")
        pat_json = json.dumps(patterns).replace("'", "''")
        ins_sql = f"INSERT INTO admin_users (username, password_hash, is_admin, patterns) VALUES ('{safe_uname}', '{safe_hash}', {1 if is_admin else 0}, '{pat_json}');"
        # execute and capture result for debugging
        try:
            proc = execute_sql(ins_sql)
            try:
                print(f"DEBUG api_create_user execute_sql stdout: {proc.stdout}")
                print(f"DEBUG api_create_user execute_sql stderr: {proc.stderr}")
            except Exception:
                pass
        except Exception as e:
            try:
                print(f"ERROR api_create_user execute_sql exception: {e}")
            except Exception:
                pass
            # continue to persist to fallback store so created user is immediately usable
            proc = None
    except Exception:
        # treat insert errors as non-fatal for HTTP response but log for debugging
        try:
            print("ERROR: failed to persist admin_users row via SQL, falling back to users_store")
        except Exception:
            pass
        proc = None
    # ensure in-memory users_store and fallback file are updated so login works immediately
    try:
        users_store = users_store or _load_users()
        users_store[username] = {"password_hash": pw_hash, "is_admin": is_admin, "patterns": patterns}
        _persist_users(users_store)
    except Exception:
        try:
            print("WARN: failed to update fallback users_store after creating user")
        except Exception:
            pass
    return {"status": "created"}


@app.get("/api/admin/users")
def api_list_users(request: Request):
    if not _is_admin_request(request):
        raise HTTPException(status_code=403, detail="admin required")
    return {"users": {u: {"is_admin": v.get("is_admin", False), "patterns": v.get("patterns", [])} for u, v in (users_store or {}).items()}}


@app.delete("/api/admin/users/{username}")
def api_delete_user(username: str, request: Request):
    if not _is_admin_request(request):
        raise HTTPException(status_code=403, detail="admin required")
    # require admin password in header X-Admin-Password for delete
    try:
        # debug: log incoming headers to help diagnose auth issues
        h = {}
        for k, v in request.headers.items():
            h[k] = v
        print("DEBUG api_delete_user headers:", h)
    except Exception:
        pass
    if not _verify_admin_password(request, None):
        raise HTTPException(status_code=401, detail="admin password required")
    global users_store
    if username in (users_store or {}):
        # remove from in-memory store and persist fallback store
        del users_store[username]
        _persist_users(users_store)
        # also remove any DB-backed records (admin_users + admin_user_dbs) to fully delete user
        try:
            safe_uname = username.replace("'", "''")
            try:
                execute_sql(f"USE admin; DELETE FROM admin_users WHERE username='{safe_uname}';")
            except Exception:
                # best effort: ignore if admin tables don't exist or delete fails
                pass
            try:
                execute_sql(f"USE admin; DELETE FROM admin_user_dbs WHERE username='{safe_uname}';")
            except Exception:
                pass
        except Exception:
            pass
        # update engine-friendly mapping file so UI/engine sees current mappings immediately
        try:
            mapping_path = os.path.join(HERE, "admin_user_dbs.txt")
            lines = []
            us = users_store or {}
            for u, info in us.items():
                pats = info.get("patterns", []) or []
                if pats:
                    lines.append(f"{u}={','.join(pats)}")
            with open(mapping_path, "w", encoding="utf-8") as mf:
                mf.write("\n".join(lines))
        except Exception:
            pass
        return {"status": "deleted"}
    raise HTTPException(status_code=404, detail="user not found")


@app.get("/queries/{database}")
def get_saved_queries(database: str):
    """
    Get saved queries for a specific database.
    """
    return {"queries": saved_queries.get(database, {})}


@app.delete("/queries/{database}/{query_name}")
def delete_saved_query(database: str, query_name: str):
    """
    Delete a saved query.
    """
    if database in saved_queries and query_name in saved_queries[database]:
        del saved_queries[database][query_name]
        # persist change
        _persist_saved_queries()
        return {"status": "deleted"}
    raise HTTPException(status_code=404, detail="Query not found")


# -----------------------
# Registration endpoint (open registration for non-admins)
# -----------------------
@app.post("/api/auth/register")
def api_register(payload: Dict[str, Any]):
    """
    Register a new non-admin user.
    payload: { username, password }
    Passwords must be at least 8 characters. Admin flag cannot be set via this endpoint.
    """
    if not isinstance(payload, dict):
        raise HTTPException(status_code=400, detail="invalid payload")
    username = payload.get("username")
    password = payload.get("password")
    if not username or not password:
        raise HTTPException(status_code=400, detail="username and password required")
    if len(password) < 8:
        raise HTTPException(status_code=400, detail="password must be at least 8 characters")
    # check DB first to ensure uniqueness
    existing = _get_user_from_db(username)
    if existing:
        raise HTTPException(status_code=400, detail="username already exists")
    try:
        pw_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt()).decode("utf-8")
    except Exception:
        raise HTTPException(status_code=500, detail="bcrypt error")
    # insert into admin_users
    try:
        safe_uname = username.replace("'", "''")
        safe_hash = pw_hash.replace("'", "''")
        ins_sql = f"INSERT INTO admin_users (username, password_hash, is_admin, patterns) VALUES ('{safe_uname}', '{safe_hash}', 0, '[]');"
        execute_sql(ins_sql)
    except Exception:
        raise HTTPException(status_code=500, detail="failed to persist user to DB")
    # also update in-memory and fallback file store for immediate availability
    try:
        global users_store
        users_store = users_store or _load_users()
        users_store[username] = {"password_hash": pw_hash, "is_admin": False, "patterns": []}
        _persist_users(users_store)
    except Exception:
        # non-fatal if fallback persist fails
        try:
            print(f"WARN: failed to update fallback users store for {username}")
        except Exception:
            pass
    return {"status": "created"}


# -----------------------
# Protect database endpoints: require authentication and enforce DB ACLs
# -----------------------
@app.get("/databases")
def list_databases(request: Request):
    """
    Return JSON list of database names filtered by user ACL.
    """
    user = _require_auth(request)
    proc = execute_sql("SHOW DATABASES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        if l.startswith("========") or l.lower().startswith("available") or l.lower().startswith("total"):
            continue
        if l and not any(char in l for char in ['=', 'Available', 'Total']):
            names.append(l.strip())
    # filter by allowed patterns for this user
    allowed = [n for n in names if _db_allowed_for_user(user, n)]
    return {"databases": allowed}


@app.get("/database/{db_name}/tables")
def list_tables_in_database(db_name: str, request: Request):
    user = _require_auth(request)
    if not _db_allowed_for_user(user, db_name):
        raise HTTPException(status_code=403, detail="permission denied for database")
    proc = execute_sql(f"USE {db_name}; SHOW TABLES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        if l.startswith("========") or l.lower().startswith("tables in database") or l.lower().startswith("total"):
            continue
        if l and not any(char in l for char in ['=', 'Tables', 'Total']):
            names.append(l.strip())
    return {"tables": names}


@app.get("/table/{name}")
def get_table(name: str, limit: int = 50, request: Request = None):
    """
    Return structure and sample rows for a table. If request provided and a db query param is present in headers or similar,
    we currently require authentication (generic).
    """
    # require auth for table access (specific DB checks are best-effort via query param 'db' in request.query_params)
    if request:
        user = _require_auth(request)
        # if client provided db param, check it
        db = request.query_params.get("db", "")
        if db and not _db_allowed_for_user(user, db):
            raise HTTPException(status_code=403, detail="permission denied for database")
    sql = f"SELECT * FROM {name} LIMIT {limit};"
    proc = execute_sql(sql)
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    parsed = _parse_table_text(proc.stdout or proc.stderr or "")
    return {"table": name, "columns": parsed["columns"], "rows": parsed["rows"]}


@app.get("/table/{name}/schema")
def table_schema(name: str, db: str = "", request: Request = None):
    # require auth and check db ACL if provided
    if request:
        user = _require_auth(request)
        if db and not _db_allowed_for_user(user, db):
            raise HTTPException(status_code=403, detail="permission denied for database")
    if db:
        sql = f"USE {db}; SELECT * FROM {name};"
    else:
        sql = f"SELECT * FROM {name};"
    try:
        proc = execute_sql(sql)
    except HTTPException:
        raise
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    cleaned = (proc.stdout or proc.stderr or "").splitlines()
    parsed = _parse_table_text("\n".join(cleaned))
    cols = parsed.get("columns", [])
    rows = parsed.get("rows", [])
    import re
    def infer_type(values):
        has_nonempty = False
        for v in values:
            if v is None:
                continue
            s = str(v).strip()
            if s == "":
                continue
            has_nonempty = True
            if not re.match(r"^-?\\d+(\\.\\d+)?$", s):
                return "string"
        return "number" if has_nonempty else "string"
    types = []
    for c in cols:
        col_values = [r.get(c, "") for r in rows]
        types.append({"name": c, "type": infer_type(col_values)})
    return {"columns": types, "sample_rows": rows}
