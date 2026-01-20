import React from "react";
import { Form, Input, Button, Space, message } from "antd";
import { execSQL } from "../api";

function escape(v) {
  if (v === null || v === undefined) return "NULL";
  const s = String(v).trim();
  if (s === "") return "NULL";
  // numeric literal (integer or float) - leave unquoted
  if (/^-?\d+(\.\d+)?$/.test(s)) return s;
  return `'${s.replace(/'/g, "''")}'`;
}

export default function RowEditor({ tableName, row = {}, onSaved, onCancel, mode = "edit", columns: propColumns = [], numericColumns = [], database = "" }) {
  const [form] = Form.useForm();

  const initial = {};
  Object.keys(row).forEach((k) => {
    if (k === "__rowid") return;
    let v = row[k];
    // trim surrounding quotes if present to avoid numeric detection issues like "'1'"
    if (typeof v === "string") {
      v = v.trim();
      if ((v.startsWith("'") && v.endsWith("'")) || (v.startsWith('"') && v.endsWith('"'))) {
        v = v.slice(1, -1);
      }
    }
    initial[k] = v;
  });

  // Decide which columns to render:
  // - In insert mode prefer provided propColumns (from TableView)
  // - Otherwise use keys from initial (existing row), fallback to propColumns or ["col1"]
  let columns;
  if (mode === "insert") {
    columns = propColumns && propColumns.length ? propColumns : (Object.keys(initial).length ? Object.keys(initial) : ["col1"]);
  } else {
    columns = Object.keys(initial).length ? Object.keys(initial) : (propColumns && propColumns.length ? propColumns : ["col1"]);
  }

  async function onFinish(values) {
    try {
      let sql = "";
      // sanitize values: trim surrounding quotes and coerce numeric-like strings
      const cleanValues = {};
      Object.keys(values).forEach((k) => {
        const v = values[k];
        if (v === null || v === undefined) {
          cleanValues[k] = null;
          return;
        }
        let s = String(v).trim();
        // remove surrounding single or double quotes if present
        if ((s.startsWith("'") && s.endsWith("'")) || (s.startsWith('"') && s.endsWith('"'))) {
          s = s.slice(1, -1);
        }
        cleanValues[k] = s;
      });

      if (mode === "insert") {
        // Prefer propColumns order when provided so we can emit explicit column list.
        const colOrder = (propColumns && propColumns.length) ? propColumns : Object.keys(cleanValues);
        const colsStr = colOrder.map(k => (k.includes(".") ? k.split(".").pop() : k)).join(", ");
        const vals = colOrder.map((k) => escape(cleanValues[k] !== undefined ? cleanValues[k] : "")).join(", ");
        sql = `INSERT INTO ${tableName} (${colsStr}) VALUES (${vals});`;
        if (database) {
          sql = `USE ${database}; ` + sql;
        }
      } else {
        // update - only include changed columns in SET and avoid trailing commas
        const changedKeys = Object.keys(values).filter((k) => {
          const newVal = values[k];
          const oldVal = initial[k];
          const newS = newVal === null || newVal === undefined ? "" : String(newVal).trim();
          const oldS = oldVal === null || oldVal === undefined ? "" : String(oldVal).trim();
          return newS !== oldS;
        });
        if (changedKeys.length === 0) {
          message.info("未检测到修改，操作已取消");
          return;
        }
        const sets = changedKeys
          .map((k) => {
            const colName = k.includes(".") ? k.split(".").pop() : k;
            return `${colName}=${escape(values[k])}`;
          })
          .filter(Boolean)
          .join(", ");

        // Prefer a primary key column if present to build WHERE (common: id)
        const initKeys = Object.keys(initial);
        const primaryKey = initKeys.find(k => k && String(k).toLowerCase() === "id");
        let where = "";
        if (primaryKey && initial[primaryKey] !== undefined && initial[primaryKey] !== null && String(initial[primaryKey]).trim() !== "") {
          const colName = primaryKey.includes(".") ? primaryKey.split(".").pop() : primaryKey;
          where = `${colName}=${escape(initial[primaryKey])}`;
        } else {
          // fallback: build WHERE from all original non-empty columns
          where = initKeys
            .filter((k) => k !== "__rowid")
            .filter((k) => initial[k] !== null && initial[k] !== undefined && String(initial[k]).trim() !== "")
            .map((k) => {
              const colName = k.includes(".") ? k.split(".").pop() : k;
              return `${colName}=${escape(initial[k])}`;
            })
            .join(" AND ");
        }

        if (!sets || !where) {
          message.error("构建更新语句失败（缺少要更新的字段或条件）");
          return;
        }

        sql = `UPDATE ${tableName} SET ${sets} WHERE ${where};`;
        if (database) {
          sql = `USE ${database}; ` + sql;
        }
      }

      const res = await execSQL({ sql });
      if (res.returncode === 0) {
        message.success(mode === "insert" ? "插入成功" : "保存成功");
        onSaved();
      } else {
        message.error("操作失败: " + (res.stderr || res.stdout));
      }
    } catch (e) {
      message.error("操作失败: " + e.message);
    }
  }

  return (
    <Form form={form} initialValues={initial} layout="vertical" onFinish={onFinish}>
      {columns.map((k) => {
        const isNumeric = (numericColumns || []).includes(k);
        return (
        <Form.Item
          label={k}
          name={k}
          key={k}
          rules={[{ required: false, whitespace: true, message: `${k} 不能为空` }]}
        >
            <Input type={isNumeric ? "number" : "text"} />
        </Form.Item>
        );
      })}
      <Form.Item>
        <Space>
          <Button type="primary" htmlType="submit">
            {mode === "insert" ? "插入" : "保存"}
          </Button>
          <Button onClick={onCancel}>取消</Button>
        </Space>
      </Form.Item>
    </Form>
  );
}


