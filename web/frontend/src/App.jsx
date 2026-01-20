import React, { useState, useEffect } from "react";
import { Layout, Input, Button, Space, Typography, Modal, Select, List, Tabs } from "antd";
import LoginPage from "./Login";
import RegisterPage from "./Register";
import TableResult from "./components/TableResult";
import DatabaseBrowser from "./components/DatabaseBrowser";
import TableView from "./components/TableView";
import AdminPanel from "./components/AdminPanel";
import { runSQL, saveQuery, getDatabases, getTablesInDatabase, login, setAuthToken, getAuthToken, clearAuthToken } from "./api";
import "./App.css";

const { Header, Content } = Layout;
const { TextArea } = Input;
const { Title } = Typography;

export default function App() {
  const [sql, setSql] = useState("");
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [selectedTable, setSelectedTable] = useState(null);
  const [selectedDatabase, setSelectedDatabase] = useState(null);
  const [dbTables, setDbTables] = useState([]);
  const [saveModalVisible, setSaveModalVisible] = useState(false);
  const [saveName, setSaveName] = useState("");
  const [availableDbs, setAvailableDbs] = useState([]);
  const [saveDb, setSaveDb] = useState("");
  const [tabs, setTabs] = useState([]);
  const [activeTab, setActiveTab] = useState(null);
  const [tableRawOutput, setTableRawOutput] = useState(null);
  const [loginVisible, setLoginVisible] = useState(false);
  const [loginUser, setLoginUser] = useState("");
  const [loginPass, setLoginPass] = useState("");
  const [authTokenState, setAuthTokenState] = useState(getAuthToken());
  const [currentUser, setCurrentUser] = useState(null);

  async function onRun() {
    setLoading(true);
    try {
      const data = await runSQL(sql);
      setResult(data);
      // write result into active tab if any
      if (activeTab) {
        setTabs(prev => prev.map(t => t.key === activeTab ? { ...t, result: data, sql } : t));
      }
    } catch (err) {
      setResult({ error: err.message });
    } finally {
      setLoading(false);
    }
  }

  function handleTableSelect(tableName, dbName) {
    // Ensure selection updates and tab opens even if same table was previously selected
    const tabKey = `table-${dbName}-${tableName}`;
    const existing = tabs.find(t => t.key === tabKey);
    setSelectedDatabase(dbName || selectedDatabase);
    setSelectedTable(tableName);
    if (existing) {
      // activate existing tab and ensure its raw output is displayed
      setActiveTab(tabKey);
      setSql(existing.sql || `USE ${dbName}; SELECT * FROM ${tableName};`);
      const out = (existing.result && (existing.result.raw_stdout || existing.result.stdout)) || "";
      setTableRawOutput(out);
    } else {
      // no existing tab: create one and fetch data (so repeated clicks work)
      createTableTab(dbName, tableName, `USE ${dbName}; SELECT * FROM ${tableName};`);
      // trigger fetch immediately (don't rely only on effect)
      const q = `USE ${dbName}; SELECT * FROM ${tableName};`;
      setTableRawOutput(undefined);
      setLoading(true);
      runSQL(q)
        .then((res) => {
          console.debug("handleTableSelect runSQL:", { dbName, tableName, res });
          const out = res.raw_stdout || res.stdout || res.stderr || "";
          setTableRawOutput(out);
          setTabs(prev => {
            const exists = prev.some(t => t.key === tabKey);
            if (exists) {
              return prev.map(t => t.key === tabKey ? { ...t, sql: q, result: { stdout: out, raw_stdout: res.raw_stdout }, dirty: false } : t);
            } else {
              const tab = { key: tabKey, title: `${dbName}-${tableName}`, sql: q, result: { stdout: out, raw_stdout: res.raw_stdout }, dirty: false, isTable: true };
              return [...prev, tab];
            }
          });
        })
        .catch((err) => {
          console.error("handleTableSelect runSQL error:", err);
          setTableRawOutput("");
        })
        .finally(() => setLoading(false));
    }
  }

  useEffect(() => {
    if (selectedTable && selectedDatabase) {
      const q = `USE ${selectedDatabase}; SELECT * FROM ${selectedTable};`;
      // create or reuse a tab for this table
      const tabKey = `table-${selectedDatabase}-${selectedTable}`;
      let existing = tabs.find(t => t.key === tabKey);
      if (!existing) {
        createTableTab(selectedDatabase, selectedTable, q);
      } else {
        setActiveTab(tabKey);
        // also set editor sql state for active tab
        setSql(existing.sql || q);
      }

      // Always fetch fresh data to populate the TableView and the tab's stored result
      setTableRawOutput(undefined);
      setLoading(true);
      runSQL(q)
        .then((res) => {
          console.debug("runSQL response for table load:", { selectedDatabase, selectedTable, res });
          const out = res.raw_stdout || res.stdout || res.stderr || "";
          setTableRawOutput(out);
          setTabs(prev => {
            const exists = prev.some(t => t.key === tabKey);
            if (exists) {
              return prev.map(t => t.key === tabKey ? { ...t, sql: q, result: { stdout: out, raw_stdout: res.raw_stdout }, dirty: false } : t);
            } else {
              const tab = { key: tabKey, title: `${selectedDatabase}-${selectedTable}`, sql: q, result: { stdout: out, raw_stdout: res.raw_stdout }, dirty: false, isTable: true };
              return [...prev, tab];
            }
          });
        })
        .catch((err) => {
          console.error("runSQL error for table load:", err);
          setTableRawOutput("");
        })
        .finally(() => setLoading(false));
    }
  }, [selectedTable, selectedDatabase]);

  async function loadDatabasesList() {
    try {
      const res = await getDatabases();
      setAvailableDbs(res.databases || []);
      if (!saveDb && res.databases && res.databases.length > 0) {
        setSaveDb(res.databases[0]);
      }
    } catch (e) {
      // ignore
    }
  }

  async function handleSaveQuery() {
    const name = (saveName || "").trim();
    if (!name) {
      Modal.error({ title: "请输入查询名称" });
      return;
    }
    try {
      await saveQuery({ name, sql, database: saveDb || "" });
      Modal.success({ title: "保存成功" });
      setSaveModalVisible(false);
      setSaveName("");
    } catch (e) {
      Modal.error({ title: "保存失败", content: e.message });
    }
  }

  async function showDatabaseTables(dbName) {
    setSelectedDatabase(dbName);
    setSelectedTable(null);
    try {
      const res = await getTablesInDatabase(dbName);
      setDbTables(res.tables || []);
    } catch (e) {
      setDbTables([]);
    }
  }

  function createNewQueryTab(initialSql = "") {
    const id = `q-${Date.now()}`;
    const tab = { key: id, title: "无标题 - 查询", sql: initialSql || "", result: null, dirty: false };
    setTabs(prev => [...prev, tab]);
    setActiveTab(id);
    setResult(null);
    if (initialSql) {
      // also set editor/sql state
      setSql(initialSql);
    }
    return id;
  }

  // Internal: create a table-only tab (separate from user-created query tabs)
  function createTableTab(db, table, sql = "") {
    const tabKey = `table-${db}-${table}`;
    setTabs(prev => {
      if (prev.some(t => t.key === tabKey)) return prev;
      const tab = { key: tabKey, title: `${db}-${table}`, sql: sql || "", result: null, dirty: false, isTable: true };
      return [...prev, tab];
    });
    // activate after state update
    setTimeout(() => setActiveTab(tabKey), 0);
    return tabKey;
  }

  function closeTab(targetKey) {
    const tab = tabs.find(t => t.key === targetKey);
    if (!tab) return;
    const doClose = async () => {
      const next = tabs.filter(t => t.key !== targetKey);
      setTabs(next);
      if (activeTab === targetKey) {
        setActiveTab(next.length ? next[next.length - 1].key : null);
      }
    };
    if (tab.dirty) {
      Modal.confirm({
        title: "关闭确认",
        content: "当前查询有未保存或未运行的更改，确认关闭？",
        okText: "关闭",
        cancelText: "取消",
        onOk: doClose
      });
    } else {
      doClose();
    }
  }

  function updateActiveTabSql(newSql) {
    setTabs(prev => prev.map(t => t.key === activeTab ? { ...t, sql: newSql, dirty: true } : t));
    setSql(newSql);
  }

  function refreshTable(table, db) {
    if (!table || !db) return;
    const q = `USE ${db}; SELECT * FROM ${table};`;
    setTableRawOutput(undefined);
    setLoading(true);
    runSQL(q)
      .then((res) => {
        const out = res.stdout || res.raw_stdout || res.stderr || "";
        setTableRawOutput(out);
        const tabKey = `table-${db}-${table}`;
        setTabs(prev => prev.map(t => t.key === tabKey ? { ...t, result: { stdout: out }, sql: q } : t));
      })
      .catch(() => setTableRawOutput(""))
      .finally(() => setLoading(false));
  }

  // 处理加载保存的查询
  function handleLoadQuery(sql) {
    setSql(sql);
  }

  useEffect(() => {
    loadDatabasesList();
  }, []);

  // 解析JWT token获取用户信息
  function parseToken(token) {
    if (!token) return null;
    try {
    const parts = token.split('.');
    let raw = null;
    if (parts.length === 3) {
      raw = atob(parts[1]);
    } else if (parts.length === 2) {
      // backend tokens use b64.signature => payload is first part
      raw = atob(parts[0]);
    } else {
      return null;
    }
    // try JSON payload first, fallback to colon-separated payload produced by backend
    try {
      const payload = JSON.parse(raw);
        return {
          username: payload.username,
          isAdmin: payload.is_admin === true || payload.is_admin === 1 || payload.is_admin === '1' || payload.is_admin === 'true' || payload.is_admin === 'True',
          patterns: payload.patterns || [],
          exp: payload.exp || null
        };
      } catch (_) {
        // backend uses colon-separated payload: username:is_admin:pattern1,pattern2
      const parts2 = raw.split(':');
      const uname = parts2[0] || "";
      const isAdminFlag = parts2[1] === "1";
      const pats = parts2[2] ? parts2[2].split(",").filter(p => p) : [];
      return { username: uname, isAdmin: isAdminFlag, patterns: pats, exp: null };
      }
    } catch (e) {
      return null;
    }
  }

  // reflect token changes (for header UI)
  useEffect(() => {
    const token = getAuthToken();
    setAuthTokenState(token);
    const parsed = parseToken(token);
    // if token has exp and is expired, clear and redirect to login
    if (parsed && parsed.exp) {
      try {
        const now = Math.floor(Date.now() / 1000);
        if (now >= parsed.exp) {
          clearAuthToken();
          if (typeof window !== "undefined") window.location.href = "/login";
          return;
        }
      } catch (e) {}
    }
    setCurrentUser(parsed);
  }, []);

  // listen for global logout events (e.g., 401 from api)
  useEffect(() => {
    function onLoggedOut() {
      try {
        // if already on /login do nothing
        if (typeof window !== "undefined" && window.location && window.location.pathname !== "/login" && window.location.pathname !== "/register") {
          window.location.href = "/login";
        }
      } catch (e) {}
    }
    window.addEventListener("trivial:logged_out", onLoggedOut);
    return () => window.removeEventListener("trivial:logged_out", onLoggedOut);
  }, []);

  // route handling: redirect unauthenticated users to /login, and serve /login and /register pages
  if (typeof window !== "undefined") {
    const path = window.location.pathname;
    const token = getAuthToken();
    // if explicit routes
    if (path === "/login") {
      return <LoginPage onLogin={async (token) => { await loadDatabasesList(); }} />;
    }
    if (path === "/register") {
      return <RegisterPage />;
    }
    // if not authenticated, redirect to login
    if (!token) {
      window.location.href = "/login";
      return null;
    }
  }

  async function handleLogin() {
    try {
      const resp = await login({ username: loginUser, password: loginPass });
      if (resp && resp.token) {
        setAuthToken(resp.token);
        setAuthTokenState(resp.token);
        setCurrentUser(parseToken(resp.token));
        setLoginVisible(false);
        setLoginUser("");
        setLoginPass("");
        Modal.success({ title: "登录成功" });
        // reload DB list to reflect permissions
        await loadDatabasesList();
      } else {
        Modal.error({ title: "登录失败" });
      }
    } catch (e) {
      Modal.error({ title: "登录失败", content: e.message });
    }
  }

  function handleLogout() {
    clearAuthToken();
    setAuthTokenState(null);
    setCurrentUser(null);
    Modal.info({ title: "已登出" });
  }

  return (
    <Layout style={{ minHeight: "100vh" }}>
      {/* Save Query modal for editor tabs */}
      <Modal
        title="保存 SQL 查询"
        open={saveModalVisible}
        onCancel={() => {
          setSaveModalVisible(false);
          setSaveName("");
        }}
        onOk={handleSaveQuery}
        okText="保存"
        cancelText="取消"
      >
        <Input
          placeholder="查询名称"
          value={saveName}
          onChange={(e) => setSaveName(e.target.value)}
          style={{ marginBottom: 8 }}
        />
        <Select value={saveDb} onChange={(v) => setSaveDb(v)} style={{ width: "100%" }}>
          {(availableDbs || []).map((d) => (
            <Select.Option key={d} value={d}>
              {d}
            </Select.Option>
          ))}
        </Select>
      </Modal>
      <Header style={{ color: "#fff" }}>
        <div style={{ display: "flex", alignItems: "center", justifyContent: "space-between" }}>
        <Title style={{ color: "#fff", margin: 0 }} level={4}>
          TrivialDB 可视化
        </Title>
          <Space>
            {authTokenState ? (
              <Button onClick={handleLogout} size="small">登出</Button>
            ) : (
              <Button type="primary" onClick={() => setLoginVisible(true)} size="small">登录</Button>
            )}
          </Space>
        </div>
      </Header>

      {/* 登录对话框 */}
      <Modal
        title="登录"
        open={loginVisible}
        onCancel={() => setLoginVisible(false)}
        onOk={handleLogin}
        okText="登录"
        cancelText="取消"
      >
        <Input
          placeholder="用户名"
          value={loginUser}
          onChange={(e) => setLoginUser(e.target.value)}
          style={{ marginBottom: 8 }}
        />
        <Input
          placeholder="密码"
          type="password"
          value={loginPass}
          onChange={(e) => setLoginPass(e.target.value)}
        />
      </Modal>
      <Layout>
          <Layout.Sider width={300} style={{ background: "#fff", padding: 12 }}>
          <DatabaseBrowser
            onSelectTable={handleTableSelect}
            onLoadQuery={handleLoadQuery}
            onSelectDatabase={showDatabaseTables}
          />
        </Layout.Sider>
        <Layout>
          <Content style={{ padding: 16 }}>
            <Space direction="vertical" style={{ width: "100%" }}>
              {/* Top quick toolbar (图4) */}
              <div style={{ marginBottom: 12 }}>
                <Space>
                  <Button
                    type="primary"
                    onClick={() => createNewQueryTab()}
                    style={{ borderRadius: 8, padding: "6px 18px" }}
                  >
                    新建查询
                  </Button>
                </Space>
              </div>

              {/* SQL 编辑/运行区：以可关闭的标签页形式显示多个查询 */}
              {(() => {
                // whether the currently active tab is a table tab
                const isTableTabActive = activeTab && activeTab.startsWith && activeTab.startsWith("table-");

                const tabItems = tabs.map(tab => {
                  const isTableTab = tab.key && tab.key.startsWith && tab.key.startsWith("table-");
                  return {
                    key: tab.key,
                    label: tab.title,
                    // allow closing for all tabs (including table-tabs)
                    closable: true,
                    // for table-tabs return null children to avoid Tabs reserving content height
                    children: isTableTab ? null : (
                      <div className="sql-editor-container">
                        <TextArea
                          value={tab.key === activeTab ? sql : tab.sql}
                          onChange={(e) => {
                            if (tab.key === activeTab) {
                              updateActiveTabSql(e.target.value);
                            } else {
                              setTabs(prev => prev.map(t => t.key === tab.key ? { ...t, sql: e.target.value, dirty: true } : t));
                            }
                          }}
                          rows={6}
                        />
                        <Space style={{ marginBottom: 12 }}>
                          <Button type="primary" onClick={onRun} loading={loading}>
                            运行 SQL
                          </Button>
                          <Button onClick={() => { setSaveModalVisible(true); }}>
                            保存 SQL 查询
                          </Button>
                        </Space>
                        <TableResult data={tab.key === activeTab ? result : tab.result} />
                      </div>
                    )
                  };
                });

                // 为管理员添加管理面板标签页
                if (currentUser && currentUser.isAdmin) {
                  tabItems.unshift({
                    key: 'admin-panel',
                    label: '用户管理',
                    closable: false,
                    children: <AdminPanel />
                  });
                }

                return (
                  <Tabs
                    activeKey={activeTab}
                    type="editable-card"
                    hideAdd
                    className={`main-tabs ${isTableTabActive ? "hide-content" : ""}`}
                    items={tabItems}
                    onEdit={(targetKey, action) => {
                      if (action === "add") createNewQueryTab();
                      if (action === "remove") closeTab(targetKey);
                    }}
                    onChange={(key) => {
                      setActiveTab(key);
                      const t = tabs.find(x => x.key === key);
                      if (t) {
                        setSql(t.sql || "");
                        setResult(t.result || null);
                        // if switching to a table tab, ensure TableView receives the stored raw output
                        if (t.isTable) {
                          const out = t.result ? (t.result.stdout || t.result.raw_output || t.result) : "";
                          setTableRawOutput(out);
                        } else {
                          // clear table raw output when switching away from table tabs
                          setTableRawOutput(undefined);
                        }
                      } else {
                        setSql("");
                        setResult(null);
                        setTableRawOutput(undefined);
                      }
                    }}
                  />
                );
              })()}

              {/* 右侧：如果选择了数据库，显示该数据库的表；否则如果选择了表，显示表结构 */}
              <div style={{ marginTop: 12 }}>
                {(() => {
                  // only show bottom area when a table tab is active
                  const isTableTabActive = activeTab && activeTab.startsWith && activeTab.startsWith("table-");
                  if (!isTableTabActive) return null;

                  // parse db/table from activeTab safely (table may contain dashes)
                  const match = activeTab.match(/^table-([^-]+)-(.+)$/);
                  const activeDb = match ? match[1] : selectedDatabase;
                  const activeTbl = match ? match[2] : selectedTable;

                  return (
                    <TableView
                      dbName={activeDb}
                      tableName={activeTbl}
                      rawOutput={tableRawOutput}
                      onRefresh={() => refreshTable(activeTbl, activeDb)}
                    />
                  );
                })()}
              </div>
            </Space>
          </Content>
        </Layout>
      </Layout>
    </Layout>
  );
}


