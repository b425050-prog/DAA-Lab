import { defineConfig } from "vitest/config";
import react from "@vitejs/plugin-react";
export default defineConfig({
  plugins: [react()],
  test: {
    environment: "jsdom",
    setupFiles: ["src/tests/setup.ts"],
    testTimeout: 15000,
    reporters: ["default", "json"],
    outputFile: { json: "../.cache/frontend-tests.json" },
  },
});
