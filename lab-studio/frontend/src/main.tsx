import React from "react";
import { createRoot } from "react-dom/client";
import App from "./App";
import "./style.css";
class ErrorBoundary extends React.Component<
  { children: React.ReactNode },
  { error: boolean }
> {
  state = { error: false };
  static getDerivedStateFromError() {
    return { error: true };
  }
  render() {
    return this.state.error ? (
      <div className="loading-screen">
        <h1>Let’s recover your workspace.</h1>
        <p>
          A display error occurred. Your saved inputs remain on this device.
        </p>
        <button className="primary" onClick={() => location.reload()}>
          Reload workbench
        </button>
        <button
          onClick={() => {
            localStorage.removeItem("daa-lab-studio-v1");
            location.reload();
          }}
        >
          Reset saved workspace and reload
        </button>
      </div>
    ) : (
      this.props.children
    );
  }
}
createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <ErrorBoundary>
      <App />
    </ErrorBoundary>
  </React.StrictMode>,
);
