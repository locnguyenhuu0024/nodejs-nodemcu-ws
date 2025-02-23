const express = require("express");
const WebSocket = require("ws");
const http = require("http");
const dotenv = require("dotenv");

dotenv.config();

const app = express();
app.use(express.json());

// Tạo HTTP server
const server = http.createServer(app);

// Tạo WebSocket server
const wss = new WebSocket.Server({ server });

// Lưu trữ các kết nối
const clients = new Set();

wss.on("connection", (ws) => {
  console.log("New client connected");
  clients.add(ws);

  ws.on("message", (message) => {
    try {
      const data = JSON.parse(message);
      console.log("Received:", data);

      // Broadcast to all connected clients
      clients.forEach((client) => {
        if (client !== ws && client.readyState === WebSocket.OPEN) {
          client.send(JSON.stringify(data));
        }
      });
    } catch (e) {
      console.error("Error parsing message:", e);
    }
  });

  ws.on("close", () => {
    console.log("Client disconnected");
    clients.delete(ws);
  });
});

// API endpoints
app.post("/api/control", (req, res) => {
  const { command, value } = req.body;

  // Validate command và value
  if (command !== "LED_CONTROL" || !["ON", "OFF"].includes(value)) {
    return res.status(400).json({
      success: false,
      message: "Invalid command or value. Use LED_CONTROL with ON/OFF value",
    });
  }

  // Gửi command tới tất cả clients
  const message = JSON.stringify({ command, value });
  clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(message);
    }
  });

  res.json({
    success: true,
    message: `LED ${value === "ON" ? "turned on" : "turned off"}`,
  });
});

app.get("/api/status", (req, res) => {
  res.json({
    status: "online",
    connectedClients: clients.size,
  });
});

const PORT = process.env.PORT || 8080;
server.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
