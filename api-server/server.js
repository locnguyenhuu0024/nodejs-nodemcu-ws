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
  const { command, value, angle } = req.body;

  // Validate command và value cho LED_CONTROL
  if (command === "LED_CONTROL") {
    if (!["ON", "OFF"].includes(value)) {
      return res.status(400).json({
        success: false,
        message: "Invalid value for LED_CONTROL. Use ON/OFF",
      });
    }

    // Gửi command tới tất cả clients
    const message = JSON.stringify({ command, value });
    clients.forEach((client) => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });

    return res.json({
      success: true,
      message: `LED ${value === "ON" ? "turned on" : "turned off"}`,
    });
  } 
  // Validate command và angle cho SERVO_CONTROL
  else if (command === "SERVO_CONTROL") {
    // Kiểm tra angle có phải là số và nằm trong khoảng 0-180
    const servoAngle = parseInt(angle);
    if (isNaN(servoAngle) || servoAngle < 0 || servoAngle > 180) {
      return res.status(400).json({
        success: false,
        message: "Invalid angle for SERVO_CONTROL. Use a number between 0-180",
      });
    }

    // Gửi command tới tất cả clients
    const message = JSON.stringify({ command, angle: servoAngle });
    clients.forEach((client) => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });

    return res.json({
      success: true,
      message: `Servo moved to angle: ${servoAngle}`,
    });
  }
  else {
    return res.status(400).json({
      success: false,
      message: "Invalid command. Use LED_CONTROL or SERVO_CONTROL",
    });
  }
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
