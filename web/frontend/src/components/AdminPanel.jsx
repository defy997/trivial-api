import React, { useState, useEffect } from "react";
import { Card, Table, Button, Space, Modal, Form, Input, message, Tag, Popconfirm, Checkbox } from "antd";
import { UserOutlined, DatabaseOutlined, PlusOutlined, DeleteOutlined } from "@ant-design/icons";
import { listUsers, createUser, deleteUser } from "../api";

export default function AdminPanel() {
  const [users, setUsers] = useState([]);
  const [loading, setLoading] = useState(false);
  const [createUserVisible, setCreateUserVisible] = useState(false);
  const [form] = Form.useForm();
  const [userDbs, setUserDbs] = useState([]);

  // 加载用户列表
  async function loadData() {
    setLoading(true);
    try {
      const res = await listUsers();
      const usersData = Object.entries(res.users || {}).map(([username, data]) => ({
        username,
        is_admin: data.is_admin || false,
        patterns: data.patterns || []
      }));
      setUsers(usersData);
      setUserDbs([]); // 暂时清空，后续可以通过API获取详细映射
    } catch (err) {
      message.error("加载用户列表失败: " + err.message);
    } finally {
      setLoading(false);
    }
  }


  // 创建新用户
  async function handleCreateUser(values) {
    try {
      await createUser({
        username: values.username,
        password: values.password,
        is_admin: values.isAdmin || false,
        patterns: values.patterns ? values.patterns.split(',').map(p => p.trim()) : []
      }, values.adminPassword);

      message.success("用户创建成功");
      setCreateUserVisible(false);
      form.resetFields();
      loadData();
    } catch (err) {
      message.error("创建用户失败: " + err.message);
    }
  }

  // 删除用户
  // accept optional adminPassword passed from confirmation modal
  async function handleDeleteUser(username, adminPassword) {
    try {
      await deleteUser(username, adminPassword);
      message.success("用户删除成功");
      loadData();
    } catch (err) {
      message.error("删除用户失败: " + err.message);
    }
  }


  useEffect(() => {
    loadData();
  }, []);

  const userColumns = [
    {
      title: '用户名',
      dataIndex: 'username',
      key: 'username',
      render: (text) => (
        <Space>
          <UserOutlined />
          {text}
        </Space>
      )
    },
    {
      title: '管理员',
      dataIndex: 'is_admin',
      key: 'is_admin',
      render: (isAdmin) => (
        isAdmin ? <Tag color="red">管理员</Tag> : <Tag>普通用户</Tag>
      )
    },
    {
      title: '数据库权限',
      dataIndex: 'patterns',
      key: 'patterns',
      render: (patterns) => (
          <Space wrap>
          {patterns.length === 0 ? (
            <Tag color="default">无权限</Tag>
          ) : (
            patterns.map((pattern, index) => (
              <Tag key={index} color="blue" icon={<DatabaseOutlined />}>
                {pattern}
                </Tag>
              ))
            )}
          </Space>
      )
    },
    {
      title: '操作',
      key: 'action',
      render: (_, record) => (
        <Space>
          <Button
            size="small"
            danger
            icon={<DeleteOutlined />}
            onClick={() => {
              // prompt for admin password before deleting
              let pwd = "";
              Modal.confirm({
                title: `删除用户 "${record.username}"`,
                content: (
                  <div>
                    <p>请输入管理员密码以确认删除：</p>
                    <Input.Password onChange={(e) => { pwd = e.target.value; }} />
                  </div>
                ),
                okText: "删除",
                okType: "danger",
                cancelText: "取消",
                onOk: async () => {
                  try {
                    await handleDeleteUser(record.username, pwd);
                  } catch (e) {
                    message.error("删除用户失败: " + e.message);
                  }
                }
              });
            }}
          >
              删除
            </Button>
        </Space>
      )
    }
  ];

  return (
    <div style={{ padding: 16 }}>
      <Card
        title={
          <Space>
            <UserOutlined />
            用户管理面板
          </Space>
        }
        extra={
          <Button
            type="primary"
            icon={<PlusOutlined />}
            onClick={() => setCreateUserVisible(true)}
          >
            创建用户
          </Button>
        }
      >
        <Table
          columns={userColumns}
          dataSource={users}
          loading={loading}
          rowKey="username"
          pagination={false}
        />

        <div style={{ marginTop: 16 }}>
          <Button onClick={loadData} loading={loading}>
            刷新数据
          </Button>
        </div>
      </Card>

      {/* 创建用户对话框 */}
      <Modal
        title="创建新用户"
        open={createUserVisible}
        onCancel={() => {
          setCreateUserVisible(false);
          form.resetFields();
        }}
        footer={null}
      >
        <Form
          form={form}
          layout="vertical"
          onFinish={handleCreateUser}
        >
          <Form.Item
            name="username"
            label="用户名"
            rules={[{ required: true, message: '请输入用户名' }]}
          >
            <Input placeholder="用户名" />
          </Form.Item>

          <Form.Item
            name="password"
            label="密码"
            rules={[
              { required: true, message: '请输入密码' },
              { min: 8, message: '密码至少8位' }
            ]}
          >
            <Input.Password placeholder="密码（至少8位）" />
          </Form.Item>

          <Form.Item
            name="confirmPassword"
            label="确认密码"
            dependencies={['password']}
            rules={[
              { required: true, message: '请确认密码' },
              ({ getFieldValue }) => ({
                validator(_, value) {
                  if (!value || getFieldValue('password') === value) {
                    return Promise.resolve();
                  }
                  return Promise.reject(new Error('两次输入的密码不一致'));
                },
              }),
            ]}
          >
            <Input.Password placeholder="再次输入密码" />
          </Form.Item>

          <Form.Item
            name="isAdmin"
            label="管理员权限"
            valuePropName="checked"
          >
            <Checkbox />
          </Form.Item>

          <Form.Item
            name="adminPassword"
            label="管理员密码（确认修改）"
            help="需要输入当前管理员密码以确认创建/修改用户（若未填写且非首次创建，后端会拒绝）"
          >
            <Input.Password placeholder="管理员密码（用于确认）" />
          </Form.Item>

          <Form.Item
            name="patterns"
            label="数据库权限"
            help="用逗号分隔的数据库模式，如: tenant_*,db1,db2"
          >
            <Input placeholder="数据库权限模式 (可选)" />
          </Form.Item>

          <Form.Item style={{ marginBottom: 0 }}>
            <Space>
              <Button type="primary" htmlType="submit">
                创建
              </Button>
              <Button onClick={() => {
                setCreateUserVisible(false);
                form.resetFields();
              }}>
                取消
              </Button>
            </Space>
          </Form.Item>
        </Form>
      </Modal>
    </div>
  );
}
