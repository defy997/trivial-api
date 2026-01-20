import React, { useEffect, useState } from "react";
import { Card, Table, Button, Space, Drawer, message, Popconfirm, Modal } from "antd";
import { runSQL, execSQL, getTable, deleteRow } from "../api";
import RowEditor from "./RowEditor";

function parseTableOutput(text) {
  if (!text) return { columns: [], rows: [] };
  // Normalize escaped newlines coming from backend (stdout may contain literal "\n" sequences)
  try {
    if (text.indexOf("\\n") !== -1 && text.indexOf("\n") === -1) {
      text = text.replace(/\\r\\n/g, "\n").replace(/\\n/g, "\n").replace(/\\r/g, "\n");
    }
  } catch (e) {}
  const lines = text
    .split("\n")
    .map((l) => l.replace(/\r/g, "").trim())
    // filter empty lines, [exit] lines and CLI informational lines like [Info], [Error], etc.
    .filter((l) => {
      if (!l) return false;
      const lower = l.toLowerCase();
      if (lower.startsWith("[exit]")) return false;
      // exclude bracketed meta lines such as [info], [error], [warning], ...
      if (/^\[.*\]/.test(l)) return false;
      return true;
    });

  // If contains '|' use it as separator (common ascii table)
  if (lines.some((l) => l.includes("|"))) {
    // find header line (first with |)
    const headerIdx = lines.findIndex((l) => l.includes("|"));
    const header = lines[headerIdx].split("|").map((c) => c.trim()).filter(Boolean);
    const dataLines = lines.slice(headerIdx + 1).filter((l) => !/^-+\s*$/.test(l));
    const rows = dataLines.map((l, idx) => {
      const cols = l.split("|").map((c) => c.trim()).filter(Boolean);
      const obj = {};
      header.forEach((h, i) => {
        obj[h || `col${i}`] = cols[i] || "";
      });
      obj.__rowid = idx;
      return obj;
    });
    return { columns: header, rows };
  }

  // CSV-like comma separated: header line with commas
  if (lines.some((l) => l.includes(","))) {
    const splitCsvLine = (ln) => {
      const res = [];
      let cur = "";
      let inQuotes = false;
      for (let i = 0; i < ln.length; i++) {
        const ch = ln[i];
        if (ch === '"') {
          if (i + 1 < ln.length && ln[i + 1] === '"') {
            cur += '"';
            i++;
          } else {
            inQuotes = !inQuotes;
          }
        } else if (ch === "," && !inQuotes) {
          res.push(cur);
          cur = "";
        } else {
          cur += ch;
        }
      }
      res.push(cur);
      return res.map(s => s.trim());
    };

    const header = splitCsvLine(lines[0]).map(h => {
      const parts = h.split(".");
      return parts[parts.length - 1] || h;
    });
    const dataLines = lines.slice(1).filter(l => l.trim());
    const rows = dataLines.map((l, idx) => {
      const cols = splitCsvLine(l);
      const obj = {};
      header.forEach((h, i) => {
        obj[h] = cols[i] !== undefined ? cols[i] : "";
      });
      obj.__rowid = idx;
      return obj;
    });
    return { columns: header, rows };
  }

  // fallback: simple whitespace-separated table
  if (lines.length >= 2) {
    const header = lines[0].split(/\s+/).filter(Boolean);
    const dataLines = lines.slice(1);
    const rows = dataLines.map((l, idx) => {
      const tokens = l.split(/\s+/);
      const obj = {};
      header.forEach((h, i) => {
        obj[h] = tokens[i] || "";
      });
      obj.__rowid = idx;
      return obj;
    });
    return { columns: header, rows };
  }

  // last fallback: single-column lines
  return {
    columns: ["value"],
    rows: lines.map((l, idx) => ({ value: l, __rowid: idx })),
  };
}

export default function TableView({ dbName, tableName, rawOutput = null, onRefresh }) {
  const [loading, setLoading] = useState(false);
  const [columns, setColumns] = useState([]);
  const [rows, setRows] = useState([]);
  const [numericCols, setNumericCols] = useState([]);
  // debug: log props when they change to verify parent passes updates
  useEffect(() => {
    console.debug("TableView props changed", { dbName, tableName });
  }, [dbName, tableName]);
  const [drawerOpen, setDrawerOpen] = useState(false);
  const [editingRow, setEditingRow] = useState(null);
  const [mode, setMode] = useState("edit");
  const [selectedRowKeys, setSelectedRowKeys] = useState([]);

  async function load() {
    if (!tableName) return;
    setLoading(true);
    try {
      // If dbName is present, query within that database context to avoid 500s
      try {
        // Prefer backend schema endpoint to get column types and canonical rows
        const schemaResp = await getTableSchema(tableName, dbName);
        const colsInfo = schemaResp.columns || [];
        const rowsResp = schemaResp.sample_rows || [];
        console.debug("TableView schema API data:", { tableName, dbName, colsInfo, rowsResp });
        setColumns(colsInfo.map(c => c.name));
        setRows(rowsResp);
        const detected = colsInfo.filter(c => c.type === "number").map(c => c.name);
        setNumericCols(detected);
      } catch (e) {
        // fallback to previous behavior if schema endpoint fails
        if (dbName) {
          const resp = await runSQL(`USE ${dbName}; SELECT * FROM ${tableName};`);
          // prefer cleaned stdout but fall back to raw_stdout if parsing yields no rows
          let parsed = parseTableOutput(resp.stdout || "");
          console.debug("TableView parsed data (with USE, fallback):", { tableName, dbName, parsed });
          if ((parsed.rows || []).length === 0 && resp.raw_stdout) {
            // try parsing raw engine output as a last resort
            try {
              const parsedRaw = parseTableOutput(resp.raw_stdout || "");
              console.debug("TableView parsed data from raw_stdout (fallback):", { tableName, dbName, parsedRaw });
              if ((parsedRaw.rows || []).length > 0) {
                parsed = parsedRaw;
              }
            } catch (e) {
              // ignore parsing fallback errors
            }
          }
          setColumns(parsed.columns);
          setRows(parsed.rows);
          const detected = (parsed.columns || []).filter(c => {
            const allNumeric = (parsed.rows || []).every(r => {
              const v = r[c];
              if (v === null || v === undefined || String(v).trim() === "") return true;
              return /^-?\d+(\.\d+)?$/.test(String(v).trim());
            });
            return allNumeric && (parsed.rows || []).length > 0;
          });
          setNumericCols(detected);
        } else {
          const tableResp = await getTable(tableName);
          const cols = tableResp.columns || [];
          const rows = tableResp.rows || [];
          console.debug("TableView table API data (fallback):", { tableName, cols, rows });
          setColumns(cols);
          setRows(rows);
          const detected = (cols || []).filter(c => {
            const allNumeric = (rows || []).every(r => {
              const v = r[c];
              if (v === null || v === undefined || String(v).trim() === "") return true;
              return /^-?\d+(\.\d+)?$/.test(String(v).trim());
            });
            return allNumeric && (rows || []).length > 0;
          });
          setNumericCols(detected);
        }
      }
    } catch (err) {
      message.error("加载表数据失败: " + err.message);
      setColumns([]);
      setRows([]);
    } finally {
      setLoading(false);
    }
  }

  useEffect(() => {
    let cancelled = false;
    async function doLoad() {
      // reset view state when switching table or db so UI updates cleanly
      setColumns([]);
      setRows([]);
      setNumericCols([]);
      setSelectedRowKeys([]);
      setDrawerOpen(false);
      setEditingRow(null);
      setMode("edit");

      // treat `undefined` as "no external data provided" (so component should fetch itself).
      if (rawOutput !== undefined) {
        setLoading(true);
        try {
          let parsed = parseTableOutput(rawOutput || "");
          console.debug("TableView parsed from rawOutput:", { tableName, parsed, rawOutput });

          // Extra fallback: if parser returned no columns but rawOutput contains comma-separated data,
          // try a simpler CSV parse that ignores bracketed meta lines.
          if ((!parsed.columns || parsed.columns.length === 0) && rawOutput && rawOutput.indexOf(",") !== -1) {
            try {
              const lines = (rawOutput || "").split("\n").map(l => l.replace(/\r/g, "").trim()).filter(Boolean);
              const useful = lines.filter(l => !/^\[.*\]/.test(l) && !l.toLowerCase().startsWith("[exit]"));
              if (useful.length >= 2) {
                const headerLine = useful[0];
                const dataLines = useful.slice(1);
                const headerCols = headerLine.split(",").map(h => {
                  const parts = h.split(".");
                  return parts[parts.length - 1] || h;
                }).map(h => h.trim());
                const rows = dataLines.map((ln, idx) => {
                  const cols = ln.split(",").map(c => c.trim());
                  const obj = {};
                  headerCols.forEach((hc, i) => {
                    obj[hc || `col${i}`] = cols[i] !== undefined ? cols[i] : "";
                  });
                  obj.__rowid = idx;
                  return obj;
                });
                parsed = { columns: headerCols, rows };
                console.debug("TableView fallback CSV parsed:", { tableName, parsed });
              }
            } catch (e) {
              console.debug("TableView fallback CSV parse failed:", e);
            }
          }

          if (!cancelled) {
            setColumns(parsed.columns);
            setRows(parsed.rows);
          }
        } finally {
          if (!cancelled) setLoading(false);
        }
        return;
      }
      await load();
    }
    doLoad();
    return () => {
      cancelled = true;
    };
    // include dbName so switching database also reloads the table
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [tableName, rawOutput, dbName]);

  const tableColumns = columns.map((c) => ({
    title: c,
    dataIndex: c,
    key: c,
    ellipsis: true,
  }));

  tableColumns.push({
    title: "操作",
    key: "__actions",
    render: (_text, record) => (
      <Space>
        <Button size="small" onClick={() => { setMode("edit"); setEditingRow(record); setDrawerOpen(true); }}>
          编辑
        </Button>
        <Popconfirm
          title="确认删除这一行吗？"
          okText="删除"
          cancelText="取消"
          onConfirm={async () => {
            // construct where clause from non-empty columns
            const where = Object.keys(record)
              .filter((k) => k !== "__rowid")
              .filter((k) => record[k] !== null && record[k] !== undefined && String(record[k]).trim() !== "")
            .map((k) => {
                const colName = k.includes(".") ? k.split(".").pop() : k;
                let s = record[k] === null || record[k] === undefined ? "" : String(record[k]).trim();
                // remove surrounding quotes if present
                if ((s.startsWith("'") && s.endsWith("'")) || (s.startsWith('"') && s.endsWith('"'))) {
                  s = s.slice(1, -1).trim();
                }
                // 根据列类型决定是否添加引号
                if (numericCols.includes(colName)) {
                  // 数值列：不添加引号
                  return `${colName}=${s}`;
                } else {
                  // 字符串列：添加引号并转义内部引号
                  return `${colName}='${s.replace(/'/g, "''")}'`;
                }
              })
              .join(" AND ");
            if (!where) {
              message.error("无法构建删除条件，操作已取消");
              return;
            }
            try {
              const resp = await deleteRow({ database: dbName, table: tableName, where });
              if (resp.returncode === 0) {
                message.success("删除成功");
                await load();
              } else {
                message.error("删除失败: " + (resp.stderr || resp.stdout || JSON.stringify(resp)));
              }
            } catch (e) {
              message.error("删除失败: " + e.message);
            }
          }}
        >
          <Button size="small" danger>删除</Button>
        </Popconfirm>
      </Space>
    ),
  });

  return (
    <Card title={dbName ? `${dbName}-${tableName}` : (tableName || "未选择表")} size="small">
      <Space style={{ marginBottom: 8 }}>
        <Button size="small" onClick={() => onRefresh && onRefresh()}>
          刷新
        </Button>
        <Button
          size="small"
          onClick={() => {
            // open insert mode
            setMode("insert");
            setEditingRow({});
            setDrawerOpen(true);
          }}
        >
          新增行
        </Button>
        <Button
          size="small"
          danger
          onClick={async () => {
            if (!selectedRowKeys || selectedRowKeys.length === 0) {
              message.info("请先选择要删除的行");
              return;
            }
            Modal.confirm({
              title: `确认删除 ${selectedRowKeys.length} 行吗？`,
              okText: "删除",
              okType: "danger",
              cancelText: "取消",
              onOk: async () => {
                try {
                  // find selected rows by rowKey (__rowid) and build batch deletes
                  const rowsToDelete = rows.filter(r => selectedRowKeys.includes(r.__rowid));
                  const deletes = [];
                  for (const r of rowsToDelete) {
                    const where = Object.keys(r)
                      .filter((k) => k !== "__rowid")
                      .filter((k) => r[k] !== null && r[k] !== undefined && String(r[k]).trim() !== "")
                    .map((k) => {
                        const colName = k.includes(".") ? k.split(".").pop() : k;
                        let s = r[k] === null || r[k] === undefined ? "" : String(r[k]).trim();
                        if ((s.startsWith("'") && s.endsWith("'")) || (s.startsWith('"') && s.endsWith('"'))) {
                          s = s.slice(1, -1).trim();
                        }
                        // 根据列类型决定是否添加引号
                        if (numericCols.includes(colName)) {
                          // 数值列：不添加引号
                          return `${colName}=${s}`;
                        } else {
                          // 字符串列：添加引号并转义内部引号
                          return `${colName}='${s.replace(/'/g, "''")}'`;
                        }
                      })
                      .join(" AND ");
                    if (!where) {
                      // skip row if cannot build condition
                      continue;
                    }
                    deletes.push({ database: dbName, table: tableName, where });
                  }
                  if (deletes.length === 0) {
                    message.warning("没有可删除的行或构建条件失败");
                    return;
                  }
                  // call batch delete endpoint once
                  const resp = await deleteRow({ deletes });
                  if (resp && resp.results) {
                    const failed = resp.results.find(r => r && r.returncode && r.returncode !== 0);
                    if (failed) {
                      message.error("部分删除失败: " + (failed.stderr || failed.stdout || JSON.stringify(failed)));
                    } else {
                      message.success("批量删除完成");
                    }
                  } else {
                    message.error("批量删除未知错误");
                  }
                  await load();
                  setSelectedRowKeys([]);
                } catch (e) {
                  message.error("批量删除失败: " + e.message);
                }
              }
            });
          }}
        >
          删除选中
        </Button>
        <Button
          size="small"
          onClick={() => {
            // export CSV
            if (!rows || !rows.length) {
              message.info("暂无可导出的数据");
              return;
            }
            const cols = columns;
            const csv = [cols.join(",")]
              .concat(
                rows.map((r) => cols.map((c) => `"${String(r[c] || "").replace(/"/g, '""')}"`).join(","))
              )
              .join("\n");
            const blob = new Blob([csv], { type: "text/csv;charset=utf-8;" });
            const url = URL.createObjectURL(blob);
            const a = document.createElement("a");
            a.href = url;
            a.download = `${tableName}.csv`;
            a.click();
            URL.revokeObjectURL(url);
          }}
        >
          导出 CSV
        </Button>
      </Space>
      <Table
        rowSelection={{
          selectedRowKeys,
          onChange: (keys) => setSelectedRowKeys(keys),
        }}
        dataSource={rows}
        columns={tableColumns}
        loading={loading}
        rowKey="__rowid"
        pagination={{ pageSize: 10 }}
        size="small"
      />
      <Drawer
        title={`${mode === "insert" ? "新增行" : "编辑行"} - ${tableName}`}
        placement="right"
        width={520}
        onClose={() => setDrawerOpen(false)}
        open={drawerOpen}
      >
        {editingRow && (
          <RowEditor
            tableName={tableName}
            row={editingRow}
            columns={columns}
            numericColumns={numericCols}
            database={dbName}
            mode={mode}
            onSaved={() => {
              setDrawerOpen(false);
              setMode("edit");
              load();
            }}
            onCancel={() => {
              setDrawerOpen(false);
              setMode("edit");
            }}
          />
        )}
      </Drawer>
    </Card>
  );
}


