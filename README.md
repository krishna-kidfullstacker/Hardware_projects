# 🛠️ Hardware Projects & Embedded Systems
**By Krishna Kanth**

A collection of embedded systems, PCB designs, hardware experiments, and IoT prototypes built from fundamental electrical principles to finished hardware implementations.

---

## 📂 Projects in this Repository

### 1. [⏰ Smart Digital Clock — Version 1](./smart_digital_clock)
> *From Digital Logic Fundamentals to a Custom ESP32-S3 & DS3231M PCB*

- **Controller**: ESP32-S3 (Xtensa Dual-Core 32-bit, 240 MHz)
- **Timekeeping**: DS3231M High-Precision MEMS RTC with backup power (I²C)
- **Display**: SPI Color TFT Display
- **Audio & Haptics**: Transistor-driven Piezo buzzer + 5-button tactile navigation pad
- **Visuals**: 3-LED status array (Blue, Red, Green) with current-limiting axial resistors
- **Custom PCB**: 2-layer routing with chamfered ergonomic outline
- 👉 **[Read Full Journey & Hardware Documentation](./smart_digital_clock/README.md)**

---

### 2. [🏠 Smart Room Assistant](./smart_home_assistant)
> *Real-time environmental humidity monitoring system with instant OLED and RGB visual feedback.*

- **Controller**: Arduino UNO / ESP32
- **Sensor**: DHT11 Temperature & Humidity Sensor
- **Display**: 0.96” I2C OLED (SSD1306)
- **Indicators**: RGB LED (Common Cathode) with tri-state environmental alerts (Good, Moderate, High)
- 👉 **[Read Project Guide](./smart_home_assistant/readme.md)**

---

## 🧭 Engineering Philosophy

> *"Going from digital logic concepts ➔ system architecture ➔ schematic design ➔ GPIO planning ➔ PCB layout ➔ routing decisions ➔ hardware implementation."*

Every project in this repository emphasizes understanding the foundational electronics, component choices, power management, and layout optimizations rather than simply hooking up pre-packaged modules.
