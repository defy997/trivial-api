import React, { useState } from "react";
import { Card, Input, Button, Space, Typography, message } from "antd";
import { register } from "./api";

const { Title } = Typography;

export default function RegisterPage() {
  const [user, setUser] = useState("");
  const [passw, setPassw] = useState("");
  const [loading, setLoading] = useState(false);

  async function doRegister() {
    if (!user || !passw) {
      message.error("请输入用户名和密码");
      return;
    }
    setLoading(true);
    try {
      const resp = await register({ username: user, password: passw });
      if (resp && resp.status === "created") {
        message.success("注册成功，请登录");
        window.location.href = "/login";
      } else {
        message.error("注册失败");
      }
    } catch (e) {
      message.error("注册失败: " + e.message);
    } finally {
      setLoading(false);
    }
  }

  return (
    <div style={{ padding: 24, display: "flex", justifyContent: "center", alignItems: "center", height: "100vh" }}>
      <Card style={{ width: 420 }}>
        <Title level={4} style={{ textAlign: "center" }}>注册 TrivialDB</Title>
        <div style={{ marginBottom: 12 }}>
          <Input placeholder="用户名" value={user} onChange={(e) => setUser(e.target.value)} />
        </div>
        <div style={{ marginBottom: 16 }}>
          <Input.Password placeholder="密码（至少8位）" value={passw} onChange={(e) => setPassw(e.target.value)} />
        </div>
        <Space>
          <Button type="primary" onClick={doRegister} loading={loading}>注册</Button>
          <Button onClick={() => { window.location.href = "/login"; }}>返回登录</Button>
        </Space>
      </Card>
    </div>
  );
}


