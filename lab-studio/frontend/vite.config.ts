import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
export default defineConfig({
  plugins: [react()],
  server: { strictPort: true, proxy: { "/api": "http://127.0.0.1:8765" } },
  preview: { strictPort: true, proxy: { "/api": "http://127.0.0.1:8765" } },
  build: {
    rollupOptions: {
      output: {
        manualChunks: {
          charts: ["chart.js", "react-chartjs-2"],
          markdown: ["react-markdown", "remark-gfm"],
        },
      },
    },
  },
});
