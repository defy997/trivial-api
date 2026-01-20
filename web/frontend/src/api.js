// small auth-aware API client
let AUTH_TOKEN = null;

function getAuthToken() {
  if (!AUTH_TOKEN) {
    try { AUTH_TOKEN = localStorage.getItem("trivial_auth_token"); } catch (e) {}
  }
  return AUTH_TOKEN;
}

function setAuthToken(token) {
  AUTH_TOKEN = token;
  try { if (token) localStorage.setItem("trivial_auth_token", token); else localStorage.removeItem("trivial_auth_token"); } catch (e) {}
}

function clearAuthToken() {
  AUTH_TOKEN = null;
  try { localStorage.removeItem("trivial_auth_token"); } catch (e) {}
}

async function authFetch(path, opts = {}) {
  const headers = opts.headers ? { ...opts.headers } : {};
  const token = getAuthToken();
  if (token) headers["Authorization"] = `Bearer ${token}`;
  opts.headers = headers;
  let resp;
  try {
    resp = await fetch(path, opts);
  } catch (e) {
    // network-level error, try fallback to backend port 8000 on same host
    try {
      if (typeof window !== "undefined" && path && path.startsWith("/api")) {
        const fallback = `${window.location.protocol}//${window.location.hostname}:8000${path}`;
        resp = await fetch(fallback, opts);
      } else {
        throw e;
      }
    } catch (e2) {
      throw e2;
    }
  }
  if (resp && resp.status === 401) {
    // Allow caller to suppress global logout handling (e.g., admin re-auth flows)
    const suppress = opts && opts._suppressLogoutOn401;
    if (!suppress) {
      try { clearAuthToken(); } catch (e) {}
      try {
        if (typeof window !== "undefined" && !window.__trivial_login_active && window.dispatchEvent) {
          window.dispatchEvent(new Event("trivial:logged_out"));
        }
      } catch (e) {}
    }
  }
  // If backend returned 404 for an /api path on the current origin, try fallback to port 8000 once
  if (resp && resp.status === 404 && typeof window !== "undefined" && path && path.startsWith("/api") && !opts._triedFallback) {
    try {
      const fallback = `${window.location.protocol}//${window.location.hostname}:8000${path}`;
      const fallbackOpts = { ...opts, _triedFallback: true };
      const r2 = await fetch(fallback, fallbackOpts);
      return r2;
    } catch (e) {
      // ignore fallback failure, return original resp
    }
  }
  return resp;
}

export async function runSQL(sql) {
  const res = await authFetch("/api/query", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ sql }),
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function execSQL(payload) {
  const res = await authFetch("/api/exec", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function deleteRow(payload) {
  const res = await authFetch("/api/delete-row", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

// 数据库浏览器相关API
export async function getDatabases() {
  const res = await authFetch("/api/databases");
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function getTablesInDatabase(dbName) {
  const res = await authFetch(`/api/database/${dbName}/tables`);
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function saveQuery(queryData) {
  const res = await authFetch("/api/queries/save", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(queryData),
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function getSavedQueries(database) {
  const res = await authFetch(`/api/queries/${database}`);
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function deleteSavedQuery(database, queryName) {
  const res = await authFetch(`/api/queries/${database}/${queryName}`, {
    method: "DELETE",
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function getTable(name) {
  const res = await authFetch(`/api/table/${encodeURIComponent(name)}`);
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function getTableSchema(name, db) {
  const url = db ? `/api/table/${encodeURIComponent(name)}/schema?db=${encodeURIComponent(db)}` : `/api/table/${encodeURIComponent(name)}/schema`;
  const res = await authFetch(url);
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function login(credentials) {
  const res = await fetch("/api/auth/login", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(credentials),
  });
  if (!res.ok) {
    // Map common auth errors to friendly messages
    if (res.status === 401) {
      throw new Error("用户名或密码错误");
    }
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  const json = await res.json();
  if (json.token) setAuthToken(json.token);
  return json;
}

export async function register(credentials) {
  const res = await fetch("/api/auth/register", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(credentials),
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

// Admin APIs
export async function listUsers() {
  const base = (typeof window !== "undefined" && window.location && window.location.origin) ? window.location.origin : "";
  const res = await authFetch(base + "/api/admin/users");
  if (!res.ok) {
    let text = await res.text();
    try {
      const j = JSON.parse(text);
      text = j.detail || text;
    } catch (e) {}
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function createUser(userData, adminPassword) {
  const base = (typeof window !== "undefined" && window.location && window.location.origin) ? window.location.origin : "";
  const headers = { "Content-Type": "application/json" };
  if (adminPassword) headers["X-Admin-Password"] = adminPassword;
  const res = await authFetch(base + "/api/admin/users", {
    method: "POST",
    headers,
    body: JSON.stringify(userData),
  });
  if (!res.ok) {
    let text = await res.text();
    try {
      const j = JSON.parse(text);
      text = j.detail || text;
    } catch (e) {}
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export async function deleteUser(username, adminPassword) {
  const base = (typeof window !== "undefined" && window.location && window.location.origin) ? window.location.origin : "";
  const headers = {};
  if (adminPassword) headers["X-Admin-Password"] = adminPassword;
  const res = await authFetch(`${base}/api/admin/users/${encodeURIComponent(username)}`, {
    method: "DELETE",
    headers,
    _suppressLogoutOn401: true,
  });
  if (!res.ok) {
    let text = await res.text();
    try {
      const j = JSON.parse(text);
      text = j.detail || text;
    } catch (e) {}
    throw new Error(`HTTP ${res.status}: ${text}`);
  }
  return await res.json();
}

export { authFetch, getAuthToken, setAuthToken, clearAuthToken };
