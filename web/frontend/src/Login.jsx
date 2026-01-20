import React, { useState } from "react";
import { Card, Input, Button, Space, Typography, message } from "antd";
import { login, setAuthToken } from "./api";

const { Title } = Typography;

export default function LoginPage({ onLogin }) {
  const [user, setUser] = useState("");
  const [passw, setPassw] = useState("");
  const [loading, setLoading] = useState(false);

  // mark global flag to indicate login modal is open so background 401 handlers won't redirect during login
  React.useEffect(() => {
    try { window.__trivial_login_active = true; } catch (e) {}
    return () => { try { window.__trivial_login_active = false; } catch (e) {} };
  }, []);

  async function doLogin() {
    if (!user || !passw) {
      message.error("请输入用户名和密码");
      return;
    }
    setLoading(true);
    try {
      const resp = await login({ username: user, password: passw });
      if (resp && resp.token) {
        setAuthToken(resp.token);
        message.success("登录成功");
        if (onLogin) onLogin(resp.token);
        // navigate to root
        window.history.replaceState({}, "", "/");
        window.location.reload();
      } else {
        message.error("登录失败");
      }
    } catch (e) {
      message.error("登录失败: " + e.message);
    } finally {
      setLoading(false);
    }
  }

  return (
    <div style={{ padding: 24, display: "flex", justifyContent: "center", alignItems: "center", height: "100vh" }}>
      <Card style={{ width: 420 }}>
        <Title level={4}>登录 TrivialDB</Title>
        <div style={{ marginBottom: 12 }}>
          <Input placeholder="用户名" value={user} onChange={(e) => setUser(e.target.value)} />
        </div>
        <div style={{ marginBottom: 16 }}>
          <Input.Password placeholder="密码" value={passw} onChange={(e) => setPassw(e.target.value)} />
        </div>
        <Space>
          <Button type="primary" onClick={doLogin} loading={loading}>登录</Button>
          <Button onClick={() => { setUser(""); setPassw(""); }}>重置</Button>
        </Space>
        <div style={{ textAlign: "center", marginTop: 8 }}>
          <a onClick={() => (window.location.href = "/register")}>没有账号？注册</a>
        </div>
      </Card>
    </div>
  );
}


