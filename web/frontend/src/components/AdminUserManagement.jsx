import React, { useState, useEffect } from "react";
import { Card, Table, Button, Modal, Form, Input, message, Space, Tag, Popconfirm } from "antd";
import { UserOutlined, DatabaseOutlined, PlusOutlined, DeleteOutlined } from "@ant-design/icons";
import { listUsers, createUser, deleteUser } from "../api";

const { Title } = require("@ant-design/typography").Typography;

export default function AdminUserManagement() {
  const [users, setUsers] = useState([]);
  const [loading, setLoading] = useState(false);
  const [createModalVisible, setCreateModalVisible] = useState(false);
  const [form] = Form.useForm();

  useEffect(() => {
    loadUsers();
  }, []);

  const loadUsers = async () => {
    setLoading(true);
    try {
      const res = await listUsers();
      setUsers(Object.entries(res.users || {}).map(([username, data]) => ({
        username,
        is_admin: data.is_admin || false,
        patterns: data.patterns || []
      })));
    } catch (err) {
      message.error("加载用户列表失败: " + err.message);
    } finally {
      setLoading(false);
    }
  };

  const handleCreateUser = async (values) => {
    try {
      await createUser({
        username: values.username,
        password: values.password,
        is_admin: values.is_admin || false,
        patterns: [] // 新用户初始没有数据库权限
      });
      message.success("用户创建成功");
      setCreateModalVisible(false);
      form.resetFields();
      loadUsers();
    } catch (err) {
      message.error("创建用户失败: " + err.message);
    }
  };

  const handleDeleteUser = async (username) => {
    try {
      await deleteUser(username);
      message.success("用户删除成功");
      loadUsers();
    } catch (err) {
      message.error("删除用户失败: " + err.message);
    }
  };

  const columns = [
    {
      title: "用户名",
      dataIndex: "username",
      key: "username",
      render: (text, record) => (
        <Space>
          <UserOutlined />
          {text}
          {record.is_admin && <Tag color="red">管理员</Tag>}
        </Space>
      )
    },
    {
      title: "数据库权限",
      dataIndex: "patterns",
      key: "patterns",
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
      title: "操作",
      key: "action",
      render: (_, record) => (
        <Space>
          <Popconfirm
            title={`确认删除用户 "${record.username}" 吗？`}
            description="此操作不可恢复，将同时删除用户的数据库权限。"
            onConfirm={() => handleDeleteUser(record.username)}
            okText="删除"
            cancelText="取消"
            okType="danger"
          >
            <Button
              type="text"
              danger
              icon={<DeleteOutlined />}
              disabled={record.username === "admin"} // 防止删除admin用户
            >
              删除
            </Button>
          </Popconfirm>
        </Space>
      )
    }
  ];

  return (
    <div style={{ padding: 24 }}>
      <Card>
        <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center", marginBottom: 16 }}>
          <Title level={3} style={{ margin: 0 }}>
            <UserOutlined style={{ marginRight: 8 }} />
            用户管理
          </Title>
          <Button
            type="primary"
            icon={<PlusOutlined />}
            onClick={() => setCreateModalVisible(true)}
          >
            创建用户
          </Button>
        </div>

        <Table
          columns={columns}
          dataSource={users}
          rowKey="username"
          loading={loading}
          pagination={false}
        />
      </Card>

      <Modal
        title="创建新用户"
        open={createModalVisible}
        onCancel={() => {
          setCreateModalVisible(false);
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
            rules={[{ required: true, message: "请输入用户名" }]}
          >
            <Input placeholder="输入用户名" />
          </Form.Item>

          <Form.Item
            name="password"
            label="密码"
            rules={[
              { required: true, message: "请输入密码" },
              { min: 8, message: "密码至少8位" }
            ]}
          >
            <Input.Password placeholder="输入密码（至少8位）" />
          </Form.Item>

          <Form.Item
            name="confirmPassword"
            label="确认密码"
            dependencies={['password']}
            rules={[
              { required: true, message: "请确认密码" },
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
            name="is_admin"
            label="管理员权限"
            valuePropName="checked"
          >
            <input type="checkbox" />
          </Form.Item>

          <Form.Item style={{ marginBottom: 0, textAlign: "right" }}>
            <Space>
              <Button onClick={() => {
                setCreateModalVisible(false);
                form.resetFields();
              }}>
                取消
              </Button>
              <Button type="primary" htmlType="submit">
                创建
              </Button>
            </Space>
          </Form.Item>
        </Form>
      </Modal>
    </div>
  );
}
