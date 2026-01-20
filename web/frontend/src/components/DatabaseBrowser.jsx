import React, { useState, useEffect, useRef } from "react";
import { Tree, Spin, Button, Modal, Input, Form, message, Dropdown, Typography, Space, Menu } from "antd";
import { DatabaseOutlined, TableOutlined, SaveOutlined, MoreOutlined, EditOutlined, DeleteOutlined } from "@ant-design/icons";
import { getDatabases, getTablesInDatabase, saveQuery, getSavedQueries, deleteSavedQuery, runSQL, getTableSchema, execSQL } from "../api";

const { Text } = Typography;
const { TextArea } = Input;

export default function DatabaseBrowser({ onSelectTable, onLoadQuery, onSelectDatabase }) {
  const [databases, setDatabases] = useState([]);
  const [loading, setLoading] = useState(false);
  const [treeData, setTreeData] = useState([]);
  const [expandedKeys, setExpandedKeys] = useState([]);
  const [selectedKeys, setSelectedKeys] = useState([]);

  // 保存查询对话框状态
  const [saveModalVisible, setSaveModalVisible] = useState(false);
  const [currentDatabase, setCurrentDatabase] = useState("");
  const [currentTable, setCurrentTable] = useState("");
  const [currentQuery, setCurrentQuery] = useState("");
  const [form] = Form.useForm();
  const [createTableModalVisible, setCreateTableModalVisible] = useState(false);
  const [createTableDb, setCreateTableDb] = useState("");
  const [newTableName, setNewTableName] = useState("");
  const [newTableColsText, setNewTableColsText] = useState("");
  const [createDbModalVisible, setCreateDbModalVisible] = useState(false);
  const [newDbName, setNewDbName] = useState("");
  const [openDropdownKey, setOpenDropdownKey] = useState(null);
  const [hoveredKey, setHoveredKey] = useState(null);
  const [overlayPos, setOverlayPos] = useState({ left: 0, top: 0 });
  // removed global anchored context menu state; we will open per-node Dropdowns instead
  const [editTableModalVisible, setEditTableModalVisible] = useState(false);
  const [editTableSchemaText, setEditTableSchemaText] = useState("");
  const [editingTableNode, setEditingTableNode] = useState(null);

  // 加载数据库列表
  async function loadDatabases() {
    setLoading(true);
    try {
      const res = await getDatabases();
      setDatabases(res.databases || []);
      // If some databases are currently expanded, reload their contents so refresh keeps children visible
      try {
        const expandedDbNames = (expandedKeys || []).filter(k => String(k).startsWith("db-")).map(k => String(k).replace("db-", ""));
        for (const dbName of expandedDbNames) {
          // fire-and-forget: load child nodes for expanded DBs
          loadDatabaseContent(dbName).catch(() => {});
        }
      } catch (e) {}
    } catch (err) {
      message.error("加载数据库列表失败: " + err.message);
    } finally {
      setLoading(false);
    }
  }

  // 加载数据库下的表和查询
  async function loadDatabaseContent(dbName) {
    try {
      // 加载表
      const tablesRes = await getTablesInDatabase(dbName);
      const tables = tablesRes.tables || [];

      // 加载保存的查询
      const queriesRes = await getSavedQueries(dbName);
      const queries = queriesRes.queries || {};

      // 构建树节点
      const dbNode = {
        title: (
          <Space>
            <DatabaseOutlined />
            {dbName}
          </Space>
        ),
        key: `db-${dbName}`,
        icon: <DatabaseOutlined />,
        children: []
      };

      // 添加表节点（即使为空也保留节点以显示 switcher）
        const tablesNode = {
          title: "表",
          key: `tables-${dbName}`,
          icon: <TableOutlined />,
        // children may be empty array if there are no tables yet — keeps switcher visible
        children: (tables || []).map(table => ({
            title: table,
            key: `table-${dbName}-${table}`,
            icon: <TableOutlined />,
            isLeaf: true,
            tableName: table,
            database: dbName
          }))
        };
        dbNode.children.push(tablesNode);

      // 添加查询节点
      const queryKeys = Object.keys(queries);
      // 添加查询节点（保留即使为空）
        const queriesNode = {
          title: "保存的查询",
          key: `queries-${dbName}`,
          icon: <SaveOutlined />,
        children: (queryKeys || []).map(queryName => ({
            title: queryName,
            key: `query-${dbName}-${queryName}`,
            icon: <SaveOutlined />,
            isLeaf: true,
            queryName,
            database: dbName,
            sql: queries[queryName]
          }))
        };
        dbNode.children.push(queriesNode);

    // 更新树数据：在原有节点位置替换，保持顺序不变
    setTreeData(prev => {
      const key = `db-${dbName}`;
      let found = false;
      const next = prev.map(node => {
        if (node.key === key) {
          found = true;
          return dbNode;
        }
        return node;
      });
      if (!found) {
        // 如果之前没有该库（新库），追加到末尾
        next.push(dbNode);
      }
      return next;
    });

    } catch (err) {
      message.error(`加载数据库 ${dbName} 内容失败: ` + err.message);
    }
  }

  // 处理树节点展开
  function onExpand(expandedKeys) {
    setExpandedKeys(expandedKeys);
  }

  // 处理树节点选择
  function onSelect(selectedKeys, { node }) {
    setSelectedKeys(selectedKeys);
    if (node.isLeaf && node.tableName) {
      // 选择了表
      onSelectTable?.(node.tableName, node.database);
      setCurrentDatabase(node.database);
      setCurrentTable(node.tableName);
    } else if (node.isLeaf && node.queryName) {
      // 选择了查询
      onLoadQuery?.(node.sql);
      setCurrentDatabase(node.database);
      setCurrentQuery(node.sql);
    } else if (node.database && !node.tableName) {
      // 选择了数据库（非叶子）——请求在右侧显示该数据库的表
      onSelectDatabase?.(node.database);
      setCurrentDatabase(node.database);
    }
  }

  // 处理树节点加载（动态加载子节点）
  async function onLoadData({ key, children }) {
    if (children) return; // 已有子节点

    const [type, dbName] = key.split('-', 2);
    if (type === 'db') {
      await loadDatabaseContent(dbName);
    }
  }

  // Fallback context handler invoked on right-click (works even when node is not expanded)
  function handleNodeContext(nodeWrapper) {
    // keep for legacy calls: simply select node
    const node = nodeWrapper?.dataRef || nodeWrapper;
    try { console.log("handleNodeContext node (legacy):", node); } catch (e) {}
    if (!node) return;
    // select node so header state updates
    setSelectedKeys(node.key ? [node.key] : []);
    if (node.database && !node.tableName) {
      setCurrentDatabase(node.database);
    }
    if (node.tableName) {
      setCurrentDatabase(node.database);
      setCurrentTable(node.tableName);
    }
  }

  function openContextMenuAt(event, node) {
    try {
      event && event.preventDefault && event.preventDefault();
      event && event.stopPropagation && event.stopPropagation();
    } catch (e) {}
    if (!node) return;
    // Only open per-node dropdowns for specific node types; otherwise just select node.
    const isAllowed =
      !!node.tableName ||
      !!node.queryName ||
      (node.key && (String(node.key).startsWith("tables-") || String(node.key).startsWith("queries-")));
    setSelectedKeys(node.key ? [node.key] : []);
    if (!isAllowed) return;
    // open the existing per-node Dropdown (keeps right-side menus intact)
    setOpenDropdownKey(node.key);
  }

  // helper: find node in treeData by key
  function findNodeByKey(key) {
    if (!key) return null;
    const stack = [...treeData];
    while (stack.length) {
      const n = stack.shift();
      if (!n) continue;
      if (n.key === key) return n;
      if (n.children && n.children.length) {
        stack.push(...n.children);
      }
    }
    return null;
  }

  // 保存查询
  async function handleSaveQuery(values) {
    try {
      await saveQuery({
        name: values.name,
        sql: currentQuery,
        database: currentDatabase
      });
      message.success("查询保存成功");
      setSaveModalVisible(false);
      form.resetFields();
      // 重新加载数据库内容
      await loadDatabaseContent(currentDatabase);
    } catch (err) {
      message.error("保存查询失败: " + err.message);
    }
  }

  // 删除保存的查询
  async function handleDeleteQuery(database, queryName) {
    try {
      await deleteSavedQuery(database, queryName);
      message.success("查询删除成功");
      // 重新加载数据库内容
      await loadDatabaseContent(database);
    } catch (err) {
      message.error("删除查询失败: " + err.message);
    }
  }

  // 构建右键菜单
  function getContextMenu(node) {
    // debug: inspect node when building context menu
    try {
      console.log("getContextMenu called for node:", node);
    } catch (e) {}
    // query node menu
    if (node.queryName) {
      return [
        {
          key: 'load',
          icon: <EditOutlined />,
          label: '加载查询',
          onClick: () => onLoadQuery?.(node.sql)
        },
        {
          key: 'delete',
          icon: <DeleteOutlined />,
          label: '删除查询',
          danger: true,
          onClick: () => handleDeleteQuery(node.database, node.queryName)
        }
      ];
    }
    // database node menu: no delete here — deletion is handled via header button
    if (node.database && !node.tableName) {
      try { console.log("building DB context menu for:", node.database); } catch(e) {}
      return [];
    }
    // tables-list node (the "表" folder) - allow creating a new table
    if (node.key && node.key.startsWith("tables-")) {
      const dbName = node.key.replace("tables-", "");
      return [
        {
          key: "new_table",
          icon: <TableOutlined />,
          label: "新建表",
          onClick: () => {
            setCreateTableDb(dbName);
            setNewTableName("");
            setNewTableColsText("");
            setCreateTableModalVisible(true);
          },
        },
      ];
    }
    // table node menu (single table)
    if (node.tableName) {
      return [
        {
          key: "edit_table",
          icon: <EditOutlined />,
          label: "编辑表结构",
          onClick: async () => {
            // open edit modal with fetched schema
            setEditingTableNode(node);
            setEditTableSchemaText("正在加载表结构...");
            setEditTableModalVisible(true);
            try {
              const schemaResp = await getTableSchema(node.tableName, node.database);
              // format schema: array of {name,type} or mapping expected from backend
              let text = "";
              if (schemaResp && schemaResp.columns) {
                text = schemaResp.columns.map(c => `${c.name} ${c.type || ""}`.trim()).join("\n");
              } else if (schemaResp && Array.isArray(schemaResp)) {
                text = schemaResp.map(c => `${c.name} ${c.type || ""}`.trim()).join("\n");
              } else if (schemaResp && schemaResp.schemaText) {
                text = schemaResp.schemaText;
              } else {
                text = JSON.stringify(schemaResp, null, 2);
              }
              setEditTableSchemaText(text);
            } catch (e) {
              setEditTableSchemaText("");
              message.error("获取表结构失败: " + e.message);
              setEditTableModalVisible(false);
            }
          },
        },
        {
          key: "delete_table",
          icon: <DeleteOutlined />,
          label: "删除表",
          danger: true,
          onClick: () => {
            Modal.confirm({
              title: `确认永久删除表 "${node.tableName}" 吗？`,
              content: "此操作不可恢复。",
              okText: "删除",
              okType: "danger",
              cancelText: "取消",
              onOk: async () => {
                try {
                  const resp = await runSQL(`USE ${node.database}; DROP TABLE ${node.tableName};`);
                  if (resp && resp.returncode && resp.returncode !== 0) {
                    message.error("删除表失败: " + (resp.stderr || resp.stdout || JSON.stringify(resp)));
                  } else {
                    message.success("表已删除");
                    await loadDatabaseContent(node.database);
                  }
                } catch (e) {
                  message.error("删除表失败: " + e.message);
                }
              },
            });
          },
        },
      ];
    }
    return [];
  }

  // 自定义树节点渲染
  function renderTreeNode(nodeData) {
    const node = nodeData.node || nodeData;
    // debug: log node structure when rendering
    try { console.log("renderTreeNode node:", node); } catch (e) {}

    if (node.queryName) {
      return (
        <Dropdown
          menu={{ items: getContextMenu(node) }}
          trigger={['contextMenu']}
          open={openDropdownKey === node.key}
          onOpenChange={(o) => setOpenDropdownKey(o ? node.key : null)}
          getPopupContainer={() => document.body}
        >
          <div style={{ padding: '4px 0' }}>
            <Space>
              <SaveOutlined />
              <Text>{node.title}</Text>
            </Space>
          </div>
        </Dropdown>
      );
    }
    // database node context menu
    if (node.database && !node.tableName) {
      return (
        <div
          style={{ padding: '4px 0', position: 'relative' }}
          onMouseEnter={(e) => {
            try {
              setHoveredKey(node.key);
              const li = e.currentTarget.closest && e.currentTarget.closest('li[data-key]');
              let rect = null;
              if (li && li.getBoundingClientRect) rect = li.getBoundingClientRect();
              if (!rect) {
                const el = document.querySelector(`[data-key="${node.key}"]`);
                if (el && el.getBoundingClientRect) rect = el.getBoundingClientRect();
              }
              if (rect) setOverlayPos({ left: rect.right - 12, top: rect.top + rect.height / 2 });
            } catch (err) {}
          }}
          onMouseLeave={() => {
            setHoveredKey(null);
          }}
        >
          <div style={{ display: 'flex', alignItems: 'center', paddingRight: 36, minHeight: 28 }}>
            {node.title}
          </div>
        </div>
      );
    }
    if (node.key && node.key.startsWith("tables-")) {
      return (
        <Dropdown
          menu={{ items: getContextMenu(node) }}
          trigger={['contextMenu']}
          open={openDropdownKey === node.key}
          onOpenChange={(o) => setOpenDropdownKey(o ? node.key : null)}
          getPopupContainer={() => document.body}
        >
          <div style={{ padding: '4px 0' }}>{node.title}</div>
        </Dropdown>
      );
    }
    if (node.tableName) {
      return (
        <Dropdown
          menu={{ items: getContextMenu(node) }}
          trigger={['contextMenu']}
          open={openDropdownKey === node.key}
          onOpenChange={(o) => setOpenDropdownKey(o ? node.key : null)}
          getPopupContainer={() => document.body}
        >
          <div
            style={{ padding: '4px 0' }}
            onClick={(e) => {
              try {
                e && e.stopPropagation && e.stopPropagation();
              } catch (err) {}
              // always trigger selection/open even if already selected (fix: closing tab then clicking same table)
              setSelectedKeys(node.key ? [node.key] : []);
              if (node.tableName) {
                onSelectTable?.(node.tableName, node.database);
                setCurrentDatabase(node.database);
                setCurrentTable(node.tableName);
              }
            }}
          >
            <Space>
              <TableOutlined />
              <Text>{node.title}</Text>
            </Space>
          </div>
        </Dropdown>
      );
    }

    return node.title;
  }

  useEffect(() => {
    loadDatabases();
  }, []);

  // clicking outside or Esc will close any open per-node dropdowns
  useEffect(() => {
    const handleDocClick = () => setOpenDropdownKey(null);
    const handleKey = (e) => { if (e.key === "Escape") setOpenDropdownKey(null); };
    document.addEventListener("click", handleDocClick);
    // Do not close dropdowns on global contextmenu events — right-click should open per-node menus.
    document.addEventListener("keydown", handleKey);
    return () => {
      document.removeEventListener("click", handleDocClick);
      document.removeEventListener("keydown", handleKey);
    };
  }, []);

  // 构建树数据结构
  useEffect(() => {
    const tree = databases.map(db => ({
      title: (
        <Space>
          <DatabaseOutlined />
          {db}
        </Space>
      ),
      key: `db-${db}`,
      icon: <DatabaseOutlined />,
      isLeaf: false,
      database: db
    }));
    setTreeData(tree);
  }, [databases]);

  return (
    <div>
      {/* Styles to force expanded-like switcher appearance and keep layout stable */}
      <style>{`
        /* make the switcher icon area visually consistent (expanded style) */
        .db-tree-container .ant-tree-switcher {
          background: rgba(0,0,0,0.03);
          border-radius: 6px;
          width: 20px;
          height: 20px;
          display: inline-flex;
          align-items: center;
          justify-content: center;
          margin-right: 6px;
        }
        /* keep the switcher visible even when collapsed */
        .db-tree-container .ant-tree-switcher::after {
          font-size: 12px;
          color: rgba(0,0,0,0.45);
        }
        /* ensure node content padding is stable */
        .db-tree-container .ant-tree-node-content-wrapper {
          padding: 4px 8px !important;
          display: flex;
          align-items: center;
          justify-content: space-between;
        }
        /* ensure title area grows and the action button stays visible */
        .db-tree-container .ant-tree-node-content-wrapper > div {
          display: flex;
          align-items: center;
          flex: 1 1 auto;
          min-width: 0;
        }
        .db-tree-container button[aria-label^="actions-"] {
          visibility: visible;
          margin-left: 6px;
          flex: 0 0 auto;
          z-index: 2000;
        }
        /* ensure dropdown overlay appears above tree */
        .db-tree-container .ant-dropdown {
          z-index: 3000 !important;
        }
      `}</style>
      <div style={{ marginBottom: 8, display: "flex", justifyContent: "space-between" }}>
        <Text strong>数据库浏览器</Text>
        <Space>
          <Button size="small" onClick={loadDatabases}>
            刷新
          </Button>
          <Button size="small" onClick={() => setCreateDbModalVisible(true)}>
            新建数据库
          </Button>
          {/* 头部删除数据库按钮（仅在已选中数据库时可用） */}
          <Button
            size="small"
            danger
            disabled={!currentDatabase}
            onClick={() => {
              if (!currentDatabase) {
                message.warn("请先在左侧选择要删除的数据库");
                return;
              }
              Modal.confirm({
                title: `确认永久删除数据库 "${currentDatabase}" 吗？`,
                content: "此操作不可恢复。",
                okText: "删除",
                okType: "danger",
                cancelText: "取消",
                onOk: async () => {
                  try {
                    const resp = await runSQL(`DROP DATABASE ${currentDatabase};`);
                    if (resp && resp.returncode && resp.returncode !== 0) {
                      message.error("删除数据库失败: " + (resp.stderr || resp.stdout || JSON.stringify(resp)));
                    } else {
                      message.success("数据库已删除");
                      await loadDatabases();
                      // remove from treeData
                      setTreeData(prev => prev.filter(n => n.key !== `db-${currentDatabase}`));
                      setCurrentDatabase("");
                      setSelectedKeys([]);
                    }
                  } catch (e) {
                    message.error("删除数据库失败: " + e.message);
                  }
                }
              });
            }}
          >
            删除数据库
          </Button>
          {currentQuery && (
            <Button
              size="small"
              type="primary"
              onClick={() => setSaveModalVisible(true)}
            >
              保存查询
            </Button>
          )}
        </Space>
      </div>

      {loading ? (
        <Spin />
      ) : (
        // wrap Tree in a container to capture contextmenu events even when clicking
        // on the expand/collapse icon or empty areas; map DOM target to data-key.
        <div className="db-tree-container"
          ref={(el) => { /* placeholder ref for potential future use */ }}
          onContextMenu={(e) => {
            try {
              // If right-click on the switcher icon, prevent the default toggle behavior first
              try {
                const switcher = e.target && e.target.closest && e.target.closest(".ant-tree-switcher");
                if (switcher) {
                  // find li[data-key] ancestor relative to switcher
                  let li = switcher;
                  while (li && li !== document && !(li.getAttribute && li.getAttribute("data-key"))) {
                    li = li.parentNode;
                  }
                  const keyFromSwitcher = li && li.getAttribute ? li.getAttribute("data-key") : null;
                  if (keyFromSwitcher) {
                    e.preventDefault();
                    e.stopPropagation();
                    const node = findNodeByKey(keyFromSwitcher);
                    if (node) {
                      openContextMenuAt(e, node);
                      return;
                    }
                  }
                }
              } catch (inner) {
                // ignore
              }

              // fallback: find the nearest ancestor with data-key attribute (antd Tree sets li[data-key])
              let el = e.target;
              while (el && el !== document && !(el.getAttribute && el.getAttribute("data-key"))) {
                el = el.parentNode;
              }
              const key = el && el.getAttribute ? el.getAttribute("data-key") : null;
              if (key) {
                const node = findNodeByKey(key);
                if (node) {
                  e.preventDefault();
                  openContextMenuAt(e, node);
                  return;
                }
              }
            } catch (err) {
              // ignore
            }
          }}
        >
        <Tree
          treeData={treeData}
          expandedKeys={expandedKeys}
          selectedKeys={selectedKeys}
          onExpand={onExpand}
          onSelect={onSelect}
            // debug: capture right-clicks on nodes and show fallback menu/actions
          onRightClick={({ event, node }) => {
            try { console.log("Tree onRightClick node:", node); } catch (e) {}
            try {
              // prevent default and stop propagation to avoid Tree toggling when right-clicking
              try { event && event.preventDefault && event.preventDefault(); } catch (e) {}
              try { event && event.stopPropagation && event.stopPropagation(); } catch (e) {}
              openContextMenuAt(event, node);
            } catch (e) {}
          }}
          loadData={onLoadData}
          titleRender={renderTreeNode}
          showIcon
        />
          {/* global anchored menu removed — per-node Dropdowns are used instead */}
        </div>
      )}

      {/* 保存查询对话框 */}
      <Modal
        title="保存查询"
        open={saveModalVisible}
        onCancel={() => {
          setSaveModalVisible(false);
          form.resetFields();
        }}
        footer={null}
      >
        <Form
          form={form}
          layout="vertical"
          onFinish={handleSaveQuery}
        >
          <Form.Item
            name="name"
            label="查询名称"
            rules={[{ required: true, message: '请输入查询名称' }]}
          >
            <Input placeholder="输入查询名称" />
          </Form.Item>

          <Form.Item label="SQL查询">
            <TextArea
              value={currentQuery}
              rows={4}
              readOnly
              placeholder="当前查询内容"
            />
          </Form.Item>

          <Form.Item>
            <Space>
              <Button type="primary" htmlType="submit">
                保存
              </Button>
              <Button onClick={() => {
                setSaveModalVisible(false);
                form.resetFields();
              }}>
                取消
              </Button>
            </Space>
          </Form.Item>
        </Form>
      </Modal>
      
      {/* 新建表对话框 */}
      <Modal
        title={`在数据库 "${createTableDb}" 中新建表`}
        open={createTableModalVisible}
        onCancel={() => {
          setCreateTableModalVisible(false);
          setNewTableName("");
          setNewTableColsText("");
        }}
        onOk={async () => {
          const db = createTableDb;
          const table = (newTableName || "").trim();
          const colsText = (newTableColsText || "").trim();
          if (!table) {
            message.error("请输入表名");
            return;
          }
          if (!colsText) {
            message.error("请输入字段定义，每行一个，格式：name TYPE");
            return;
          }
          // parse columns lines
          const lines = colsText.split("\n").map(l => l.trim()).filter(Boolean);
          const defs = lines.map(l => {
            const parts = l.split(/\s+/);
            const name = parts[0];
            const type = parts.slice(1).join(" ") || "TEXT";
            return `${name} ${type}`;
          });
          const sql = `USE ${db}; CREATE TABLE ${table} (${defs.join(", ")});`;
          try {
            const resp = await runSQL(sql);
            if (resp && resp.returncode && resp.returncode !== 0) {
              message.error("创建表失败: " + (resp.stderr || resp.stdout || JSON.stringify(resp)));
            } else {
              message.success("表创建成功");
              setCreateTableModalVisible(false);
              setNewTableName("");
              setNewTableColsText("");
              await loadDatabaseContent(db);
            }
          } catch (e) {
            message.error("创建表失败: " + e.message);
          }
        }}
      >
        <Input
          placeholder="表名"
          value={newTableName}
          onChange={(e) => setNewTableName(e.target.value)}
          style={{ marginBottom: 8 }}
        />
        <Input.TextArea
          placeholder="每行一个字段定义，格式：name TYPE，例如：id INTEGER\nname TEXT"
          value={newTableColsText}
          onChange={(e) => setNewTableColsText(e.target.value)}
          rows={6}
        />
      </Modal>
      
      {/* 编辑表结构对话框 */}
      <Modal
        title={editingTableNode ? `编辑表结构：${editingTableNode.database}.${editingTableNode.tableName}` : "编辑表结构"}
        open={editTableModalVisible}
        onCancel={() => {
          setEditTableModalVisible(false);
          setEditTableSchemaText("");
          setEditingTableNode(null);
        }}
        onOk={async () => {
          if (!editingTableNode) {
            setEditTableModalVisible(false);
            return;
          }
          const db = editingTableNode.database;
          const table = editingTableNode.tableName;
          const userText = (editTableSchemaText || "").trim();
          if (!userText) {
            message.error("表结构内容为空，取消操作");
            return;
          }
          // Send raw SQL execution: user should provide DDL/ALTER statements or column definitions.
          // To avoid attempting risky automatic migrations, we let the user provide SQL statements.
          const sql = `USE ${db};\n-- 请在下方输入您想执行的 ALTER/DDL 语句，例如：ALTER TABLE ${table} ADD COLUMN newcol TEXT; \n${userText}`;
          try {
            const resp = await execSQL({ sql });
            if (resp && resp.returncode && resp.returncode !== 0) {
              message.error("执行失败: " + (resp.stderr || resp.stdout || JSON.stringify(resp)));
            } else {
              message.success("执行成功");
              setEditTableModalVisible(false);
              setEditTableSchemaText("");
              setEditingTableNode(null);
              await loadDatabaseContent(db);
            }
          } catch (e) {
            message.error("执行表结构修改失败: " + e.message);
          }
        }}
        width={720}
      >
        <div style={{ marginBottom: 8, color: "#666" }}>
          提示：此处为低级编辑器，您可以直接输入要执行的 SQL（ALTER/DDL）。系统不会自动生成迁移；请确保语句正确。
        </div>
        <Input.TextArea
          value={editTableSchemaText}
          onChange={(e) => setEditTableSchemaText(e.target.value)}
          rows={12}
        />
      </Modal>
      
      {/* 新建数据库对话框 */}
      <Modal
        title="新建数据库"
        open={createDbModalVisible}
        onCancel={() => {
          setCreateDbModalVisible(false);
          setNewDbName("");
        }}
        onOk={async () => {
          const db = (newDbName || "").trim();
          if (!db) {
            message.error("请输入数据库名称");
            return;
          }
          try {
            const resp = await runSQL(`CREATE DATABASE ${db};`);
            if (resp && resp.returncode && resp.returncode !== 0) {
              message.error("创建数据库失败: " + (resp.stderr || resp.stdout || JSON.stringify(resp)));
            } else {
              message.success("数据库创建成功");
              setCreateDbModalVisible(false);
              setNewDbName("");
              await loadDatabases();
            }
          } catch (e) {
            message.error("创建数据库失败: " + e.message);
          }
        }}
      >
        <Input
          placeholder="数据库名称"
          value={newDbName}
          onChange={(e) => setNewDbName(e.target.value)}
        />
      </Modal>
    </div>
  );
}
