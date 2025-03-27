# 🌍 ESP8266 Carbon Emission Logger

This project collects carbon emission data using an ESP8266-based hardware setup and integrates it into a centralized logging system. The server provides real-time air quality metrics and logs emissions for analysis.

---
![image](https://github.com/user-attachments/assets/6466fec3-d8b0-4186-8ec3-50c724ef4aca)

## 🛠️ Features

### ✅ ESP8266-Based Emission Logging
- Reads user input via DIP switches.
- Determines emission category (Light, Moderate, Heavy).
- Sends timestamped data to the server.

### ✅ Centralized Server
- Displays real-time air quality and carbon data.
- Logs emissions from multiple devices.
- Provides an API for emission logging.

---

## 📡 Hardware Setup

### Components:
- **ESP8266**: Handles user input and data transmission.
- **DIP Switches**: User input for fuel type, traffic conditions, etc.
- **Flash Button**: Confirms selections and sends data.

### Pin Configuration:
| Component  | ESP8266 Pin |
|------------|------------|
| DIP Switch 1 | D1 (GPIO5) |
| DIP Switch 2 | D2 (GPIO4) |
| DIP Switch 3 | D5 (GPIO14) |
| Flash Button | GPIO0 |

### Workflow:
1. The user selects options using DIP switches.
2. The ESP8266 determines emission type.
3. Upon button press, the data is sent to the server.
4. The server logs the data and updates metrics.

---

## 🌐 Server Functionality

### Live Dashboard:
- Displays **current air quality** and **carbon emission trends**.
- Shows real-time **emission logs**.

### API Endpoints:
- **POST `/log_emission`**  
  Logs an emission entry with a timestamp.

- **GET `/get_logs`**  
  Retrieves all logged emissions.

- **GET `/dashboard`**  
  Serves the live dashboard.

---

## 🚀 Deployment

### ESP8266 Code:
- Connects to WiFi (`SSID: <ssid>`, `Password: <password>`).
- Uses an **NTP server** to fetch accurate timestamps.
- Sends data in **ISO 8601 format** (`YYYY-MM-DDTHH:MM:SS.ssssss`).

### Server:
- Built using **Flask + MongoDB**.
- Stores emissions and calculates real-time statistics.
- Displays a web dashboard with emission data.

---

## 🔧 Future Improvements
- Add **sensor-based CO2 detection** for automatic emission classification.
- Implement **mobile app** for real-time monitoring.
- Enhance **data visualization** for trends over time.

---

## 📜 License
This project is open-source under the MIT License.
