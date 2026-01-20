# TrivialDB 云数据库 - ACL 引擎集成部署说明

## 概述

本次部署实现方案1：在引擎层（C++ TrivialDB）集成 ACL（访问控制列表）支持，通过在 `AUTH` 认证时从后端同步的映射文件加载用户数据库权限，并合并到会话的允许模式列表。后端在授权时写入映射文件以确保引擎能正确读取。

## 架构变更

### 引擎层 (TrivialDB C++)
- **文件**: `TrivialDB-master/src/parser/execute.cpp`
- **变更**: 在 `execute_auth()` 函数中，解析 JWT token 后，从 `admin_user_dbs.txt` 文件读取当前用户的数据库映射，并追加到会话的 `allowed_db_patterns`。
- **格式**: 映射文件格式为每行 `username=db1,db2,...`（引擎读取时忽略无效行）。

### 后端层 (FastAPI)
- **文件**: `trivial-api/web/backend/app.py`
- **变更**:
  - `_grant_db_to_user()` 函数现在除了更新内存/DB/users.json，还写入 `admin_user_dbs.txt` 文件。
  - 格式：`username=db1,db2,...`（一行一个用户）。
- **兼容性**: 保留原有的 DB 表插入和 users.json 备份作为兜底。

### 前端层 (React)
- 无变更 - 现有的 token 认证和数据库列表过滤逻辑保持不变。

## 部署步骤

### 1. 重新编译引擎 (必需)
```bash
cd /root/TrivialDB-master
mkdir -p build && cd build
cmake -S .. -B .
cmake --build . -j4
```

### 2. 复制新二进制 (必需)
```bash
cp /root/TrivialDB-master/build/build/trivial_db /root/trivial-api/build/trivial_db
# 或者 cp /root/TrivialDB-master/build/trivial_db /root/trivial-api/build/trivial_db
```

### 3. 重启后端容器 (必需)
```bash
docker restart my-api
```

### 4. 验证部署
```bash
# 健康检查
curl http://127.0.0.1:8000/health

# 登录测试
TOKEN=$(curl -sS -X POST http://127.0.0.1:8000/api/auth/login \
  -H 'Content-Type: application/json' \
  -d '{"username":"defy","password":"12345678"}' | jq -r .token)

# 创建数据库测试
curl -X POST http://127.0.0.1:8000/api/exec \
  -H "Authorization: Bearer $TOKEN" \
  -H 'Content-Type: application/json' \
  -d '{"sql":"CREATE DATABASE tenant_defy_test;"}'

# 验证ACL生效
curl -H "Authorization: Bearer $TOKEN" http://127.0.0.1:8000/api/databases
```

## 关键文件位置

- **引擎二进制**: `/root/trivial-api/build/trivial_db`
- **映射文件**: `/root/trivial-api/web/backend/admin_user_dbs.txt`
- **引擎源码**: `/root/TrivialDB-master/src/parser/execute.cpp`
- **后端源码**: `/root/trivial-api/web/backend/app.py`

## 回滚步骤

如果需要回滚到方案前的状态：

1. 停止容器: `docker stop my-api`
2. 恢复旧引擎二进制 (如果有备份): `cp /path/to/old/trivial_db /root/trivial-api/build/trivial_db`
3. 删除映射文件: `rm /root/trivial-api/web/backend/admin_user_dbs.txt`
4. 重启容器: `docker start my-api`

## 监控与日志

- **后端日志**: `docker logs my-api` 查看授权和映射写入日志
- **引擎调试**: 添加 `DEBUG` 前缀的日志会显示在后端日志中
- **映射文件**: 实时检查 `/root/trivial-api/web/backend/admin_user_dbs.txt` 确认同步状态
 
## Nginx reverse proxy (recommended)

如果 frontend 与 backend 在不同端口或你希望使用标准 80 端口对外暴露服务，建议在主机上配置 Nginx 将 `/api/*` 代理到 `http://127.0.0.1:8000`（后端）。示例如下：

1) 将示例配置 `deploy/nginx_trivial.conf` 复制到 `/etc/nginx/sites-available/trivial.conf` 并创建启用链接：
```bash
sudo cp /root/trivial-api/deploy/nginx_trivial.conf /etc/nginx/sites-available/trivial.conf
sudo ln -s /etc/nginx/sites-available/trivial.conf /etc/nginx/sites-enabled/trivial.conf
```

2) 将前端 build 部署到 `/var/www/trivial-frontend`（或修改配置文件中的 root）：
```bash
sudo mkdir -p /var/www/trivial-frontend
sudo rm -rf /var/www/trivial-frontend/* && sudo cp -r /root/trivial-api/web/frontend/dist/* /var/www/trivial-frontend/
```

3) 测试并重载 nginx：
```bash
sudo nginx -t
sudo systemctl reload nginx
```

回滚：
- 删除启用链接并重载 nginx：
```bash
sudo unlink /etc/nginx/sites-enabled/trivial.conf
sudo systemctl reload nginx
```

Dockerized nginx 例子（不修改宿主 nginx）：
```bash
docker run -d --name trivial-nginx -p 80:80 \
  -v /root/trivial-api/deploy/nginx_trivial.conf:/etc/nginx/conf.d/trivial.conf:ro \
  -v /root/trivial-api/web/frontend/dist:/var/www/trivial-frontend:ro \
  nginx:stable
```

注意：
- 如果后端以容器方式运行并非绑定到 127.0.0.1:8000，请调整 `proxy_pass` 到容器可访问地址（例如 `http://host.docker.internal:8000` 或容器 IP）。
- 该 proxy 方案将使前端对 `/api/*` 的请求透明地到达后端，避免端口相关 404/跨域问题。

## 已知限制

- 映射文件是纯文本格式，适合小规模部署
- 如果并发写入映射文件，可能需要额外的文件锁（当前未实现）
- 依赖后端正确写入映射文件；如果后端崩溃，映射可能不一致

## 扩展建议

如果需要更复杂的 ACL：
- 考虑将映射存储在 Redis 或数据库中
- 实现映射文件的原子写入（临时文件 + mv）
- 添加映射文件的定期清理和一致性检查
