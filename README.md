🔧 Predictive Maintenance System for Table Fan (ESP32-Based)

This project implements a low-cost predictive maintenance system for a table fan using embedded sensors and real-time data processing. The system continuously monitors the fan’s health and detects early signs of failure to prevent breakdowns and improve lifespan.

🚀 Features

📡 Real-time monitoring of vibration, temperature, and rotational speed

🧠 Predictive insights to detect anomalies and potential faults

📊 Sensor fusion using multiple data sources for better accuracy

🌐 IoT-enabled system using ESP32 for data transmission and remote monitoring

🧩 Hardware Components

ESP32 – Microcontroller with Wi-Fi capability

MPU6050 – Accelerometer + gyroscope for vibration analysis

DS18B20 – Temperature sensor for thermal monitoring

A3144 Hall Sensor – Measures rotational speed (RPM)

⚙️ Working Principle

The system collects data from multiple sensors:

Vibration patterns (MPU6050) help identify imbalance or wear

Temperature data (DS18B20) detects overheating issues

RPM readings (Hall sensor) monitor speed consistency

This data is processed on the ESP32 and can be used to:

Detect abnormal behavior

Trigger alerts

Enable predictive maintenance strategies

📈 Applications


Industrial equipment monitoring

Preventive maintenance systems

IoT-based condition monitoring

💡 Goal

To demonstrate how affordable IoT hardware + sensor data can be used to build an intelligent predictive maintenance system for everyday appliances.


| Sensor                 | Pin Name         | ESP32 Pin               |
| ---------------------- | ---------------- | ----------------------- |
| 🌡️ DS18B20            | DATA             | **GPIO 4**              |
|                        | VCC              | 3.3V                    |
|                        | GND              | GND                     |
| ⚠️                     | Pull-up resistor | **4.7kΩ (DATA ↔ 3.3V)** |
| ⚙️ MPU6050             | SDA              | **GPIO 21**             |
|                        | SCL              | **GPIO 22**             |
|                        | VCC              | 3.3V                    |
|                        | GND              | GND                     |
| 🔄 Hall Sensor (A3144) | OUT              | **GPIO 5**              |
|                        | VCC              | 3.3V                    |
|                        | GND              | GND                     |
