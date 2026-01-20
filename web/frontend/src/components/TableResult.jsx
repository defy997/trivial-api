import React from "react";
import { Card, Typography } from "antd";

const { Paragraph } = Typography;

export default function TableResult({ data }) {
  if (!data) return null;
  if (data.error) {
    return (
      <Card title="错误">
        <Paragraph type="danger">{data.error}</Paragraph>
      </Card>
    );
  }

  return (
    <Card title="结果">
      <pre style={{ whiteSpace: "pre-wrap" }}>{data.stdout || data.stderr}</pre>
    </Card>
  );
}


