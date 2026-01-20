import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  server: {
    port: 3000,
    // --- 关键修改：允许 Nginx 访问并支持热更新 ---
    host: '0.0.0.0', 
    hmr: {
      clientPort: 80, 
    },
    // ----------------------------------------
    proxy: {
      '/api': {
        target: 'http://127.0.0.1:8000',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api/, ''),
      },
    },
  },
})