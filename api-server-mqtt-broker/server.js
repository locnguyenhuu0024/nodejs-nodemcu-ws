// Tải biến môi trường từ file .env
require('dotenv').config();

const express = require('express');
const mqtt = require('mqtt');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3001;

// Middleware
app.use(express.json());
app.use(cors());

// Cấu hình MQTT từ biến môi trường
const MQTT_BROKER = process.env.MQTT_BROKER || 'mqtt://broker.emqx.io:1883';
const MQTT_TOPIC = process.env.MQTT_TOPIC || 'huuloc/sensors/data';
const MQTT_OPTIONS = {
  clientId: 'nodejs-api-server-' + Math.random().toString(16).substring(2, 8),
  clean: true,
  connectTimeout: 10000, // Tăng timeout lên 10 giây
  reconnectPeriod: 5000, // Thử kết nối lại sau 5 giây
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD
};

// Chế độ giả lập (đặt thành false nếu bạn muốn kết nối với MQTT broker thực tế)
let SIMULATION_MODE = process.env.SIMULATION_MODE === 'true';
let simulationInterval = null;

// Lưu trữ dữ liệu cảm biến mới nhất
let latestSensorData = {
  temperature: null,
  humidity: null,
  light: null,
  motion: null,
  timestamp: null,
  isConnected: false
};

// Hàm tạo dữ liệu giả lập
function generateSimulatedData() {
  latestSensorData = {
    temperature: parseFloat((20 + Math.random() * 10).toFixed(1)), // 20-30°C
    humidity: parseFloat((50 + Math.random() * 30).toFixed(1)),    // 50-80%
    light: Math.floor(100 + Math.random() * 900),      // 100-1000
    motion: Math.random() > 0.5,                       // true/false ngẫu nhiên
    timestamp: new Date().toISOString(),
    isConnected: true
  };
  
  console.log('Dữ liệu giả lập mới:', latestSensorData);
}

// Bắt đầu chế độ giả lập
function startSimulation() {
  if (!simulationInterval) {
    console.log('Bắt đầu chế độ giả lập...');
    SIMULATION_MODE = true;
    // Tạo dữ liệu giả lập mỗi 2 giây
    simulationInterval = setInterval(generateSimulatedData, 2000);
    // Tạo dữ liệu ban đầu
    generateSimulatedData();
  }
}

// Dừng chế độ giả lập
function stopSimulation() {
  if (simulationInterval) {
    console.log('Dừng chế độ giả lập...');
    clearInterval(simulationInterval);
    simulationInterval = null;
    SIMULATION_MODE = false;
  }
}

// Kết nối MQTT hoặc sử dụng chế độ giả lập
let mqttClient;
let mqttConnectAttempts = 0;
const MAX_MQTT_CONNECT_ATTEMPTS = 3;

function connectMQTT() {
  if (SIMULATION_MODE) {
    startSimulation();
    return;
  }
  
  console.log(`Đang kết nối đến MQTT broker: ${MQTT_BROKER}`);
  console.log(`Đăng ký topic: ${MQTT_TOPIC}`);
  
  // Kết nối đến MQTT broker
  mqttClient = mqtt.connect(MQTT_BROKER, MQTT_OPTIONS);
  
  // Xử lý sự kiện kết nối
  mqttClient.on('connect', () => {
    console.log('Đã kết nối đến MQTT broker');
    mqttConnectAttempts = 0; // Reset số lần thử kết nối
    latestSensorData.isConnected = true;
    
    // Dừng chế độ giả lập nếu đang chạy
    stopSimulation();
    
    // Đăng ký nhận dữ liệu từ topic
    mqttClient.subscribe(MQTT_TOPIC, (err) => {
      if (err) {
        console.error('Lỗi khi đăng ký topic:', err);
      } else {
        console.log(`Đã đăng ký topic: ${MQTT_TOPIC}`);
      }
    });
  });
  
  // Xử lý sự kiện nhận tin nhắn
  mqttClient.on('message', (topic, message) => {
    console.log(`Nhận tin nhắn từ topic ${topic}:`, message.toString());
    
    try {
      // Phân tích dữ liệu JSON
      const data = JSON.parse(message.toString());
      
      // Cập nhật dữ liệu cảm biến
      latestSensorData = {
        temperature: data.temperature,
        humidity: data.humidity,
        light: data.light,
        motion: data.motion,
        timestamp: new Date().toISOString(),
        isConnected: true
      };
      
      console.log('Dữ liệu cảm biến mới:', latestSensorData);
    } catch (error) {
      console.error('Lỗi khi phân tích dữ liệu JSON:', error);
    }
  });
  
  // Xử lý sự kiện lỗi
  mqttClient.on('error', (err) => {
    console.error('Lỗi MQTT:', err);
    latestSensorData.isConnected = false;
    
    mqttConnectAttempts++;
    if (mqttConnectAttempts >= MAX_MQTT_CONNECT_ATTEMPTS) {
      console.log(`Đã thử kết nối ${MAX_MQTT_CONNECT_ATTEMPTS} lần không thành công. Chuyển sang chế độ giả lập.`);
      if (mqttClient) {
        mqttClient.end(true); // Đóng kết nối MQTT
      }
      startSimulation(); // Bắt đầu chế độ giả lập
    }
  });
  
  // Xử lý sự kiện mất kết nối
  mqttClient.on('offline', () => {
    console.log('MQTT client đã ngắt kết nối');
    latestSensorData.isConnected = false;
  });
  
  // Xử lý sự kiện kết nối lại
  mqttClient.on('reconnect', () => {
    console.log('MQTT client đang kết nối lại...');
  });
}

// Bắt đầu kết nối MQTT hoặc chế độ giả lập
connectMQTT();

// API endpoint để lấy dữ liệu cảm biến
app.get('/api/sensors', (req, res) => {
  res.json(latestSensorData);
});

// API endpoint để kiểm tra trạng thái kết nối
app.get('/api/status', (req, res) => {
  res.json({
    isConnected: latestSensorData.isConnected,
    lastUpdate: latestSensorData.timestamp,
    simulationMode: SIMULATION_MODE,
    mqttBroker: MQTT_BROKER,
    mqttTopic: MQTT_TOPIC
  });
});

// API endpoint để chuyển đổi chế độ giả lập
app.post('/api/simulation', (req, res) => {
  const { enabled } = req.body;
  
  if (enabled === true) {
    // Dừng kết nối MQTT nếu đang kết nối
    if (mqttClient && mqttClient.connected) {
      mqttClient.end(true);
    }
    startSimulation();
    res.json({ success: true, message: 'Đã bật chế độ giả lập', simulationMode: SIMULATION_MODE });
  } else if (enabled === false) {
    stopSimulation();
    connectMQTT();
    res.json({ success: true, message: 'Đã tắt chế độ giả lập', simulationMode: SIMULATION_MODE });
  } else {
    res.status(400).json({ success: false, message: 'Tham số không hợp lệ' });
  }
});

// API endpoint để lấy dữ liệu từng cảm biến riêng biệt
app.get('/api/sensors/temperature', (req, res) => {
  res.json({ temperature: latestSensorData.temperature, timestamp: latestSensorData.timestamp });
});

app.get('/api/sensors/humidity', (req, res) => {
  res.json({ humidity: latestSensorData.humidity, timestamp: latestSensorData.timestamp });
});

app.get('/api/sensors/light', (req, res) => {
  res.json({ light: latestSensorData.light, timestamp: latestSensorData.timestamp });
});

app.get('/api/sensors/motion', (req, res) => {
  res.json({ motion: latestSensorData.motion, timestamp: latestSensorData.timestamp });
});

// Khởi động server
app.listen(PORT, () => {
  console.log(`Server đang chạy tại http://localhost:${PORT}`);
  console.log(`Chế độ giả lập: ${SIMULATION_MODE ? 'Bật' : 'Tắt'}`);
  if (!SIMULATION_MODE) {
    console.log(`MQTT Broker: ${MQTT_BROKER}`);
    console.log(`MQTT Topic: ${MQTT_TOPIC}`);
  }
});
