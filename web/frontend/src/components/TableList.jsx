import React, { useEffect, useState } from "react";
import { List, Button, Spin, Typography } from "antd";
import { runSQL } from "../api";

const { Text } = Typography;

export default function TableList({ onSelect }) {
  const [tables, setTables] = useState([]);
  const [loading, setLoading] = useState(false);

  async function load() {
    setLoading(true);
    try {
      const res = await runSQL("SHOW TABLE;");
      const out = res.stdout || "";
      // naive parsing: split lines, ignore empty and header lines
      const lines = out
        .split("\n")
        .map((l) => l.trim())
        .filter((l) => l && !l.toLowerCase().startsWith("name") && !l.startsWith("-"));
      setTables(lines);
    } catch (err) {
      setTables([]);
    } finally {
      setLoading(false);
    }
  }

  useEffect(() => {
    load();
  }, []);

  return (
    <div>
      <div style={{ marginBottom: 8, display: "flex", justifyContent: "space-between" }}>
        <Text strong>表</Text>
        <Button size="small" onClick={load}>
          刷新
        </Button>
      </div>
      {loading ? (
        <Spin />
      ) : (
        <List
          size="small"
          bordered
          dataSource={tables}
          renderItem={(item) => (
            <List.Item style={{ cursor: "pointer" }} onClick={() => onSelect(item)}>
              {item}
            </List.Item>
          )}
        />
      )}
    </div>
  );
}


