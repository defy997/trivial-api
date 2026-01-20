# 新增的API端点 - 数据库浏览器功能

from fastapi import HTTPException

# 保存查询的简单存储（内存中，实际应用中应该持久化）
saved_queries = {}

def list_databases(execute_sql):
    """
    Return JSON list of database names.
    """
    proc = execute_sql("SHOW DATABASES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    # parse database names similar to tables
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        # skip header/separators
        if l.lower().startswith("name") or set(l) <= set("- "):
            continue
        # if pipe-separated, take tokens between pipes
        if "|" in l:
            parts = [p.strip() for p in l.split("|") if p.strip()]
            if len(parts) == 1:
                names.append(parts[0])
            else:
                # assume first column is name
                names.append(parts[0])
        else:
            # whitespace separated, take first token
            tokens = l.split()
            if tokens:
                names.append(tokens[0])
    return {"databases": names}


def list_tables_in_database(db_name: str, execute_sql):
    """
    Return JSON list of table names in a specific database.
    """
    # First switch to the database, then show tables
    proc = execute_sql(f"USE {db_name}; SHOW TABLES;")
    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail={"stdout": proc.stdout, "stderr": proc.stderr})
    text = proc.stdout or proc.stderr or ""
    # parse table names similar to the main list_tables function
    lines = [l.strip() for l in text.splitlines() if l.strip() and not l.strip().lower().startswith("[exit]")]
    names = []
    for l in lines:
        # skip header/separators
        if l.lower().startswith("name") or set(l) <= set("- "):
            continue
        # if pipe-separated, take tokens between pipes
        if "|" in l:
            parts = [p.strip() for p in l.split("|") if p.strip()]
            if len(parts) == 1:
                names.append(parts[0])
            else:
                # assume first column is name
                names.append(parts[0])
        else:
            # whitespace separated, take first token
            tokens = l.split()
            if tokens:
                names.append(tokens[0])
    # dedupe preserving order
    seen = set()
    out = []
    for n in names:
        if n not in seen:
            seen.add(n)
            out.append(n)
    return {"tables": out}


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
    return {"status": "saved"}


def get_saved_queries(database: str):
    """
    Get saved queries for a specific database.
    """
    return {"queries": saved_queries.get(database, {})}


def delete_saved_query(database: str, query_name: str):
    """
    Delete a saved query.
    """
    if database in saved_queries and query_name in saved_queries[database]:
        del saved_queries[database][query_name]
        return {"status": "deleted"}
    raise HTTPException(status_code=404, detail="Query not found")
