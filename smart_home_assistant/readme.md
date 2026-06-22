🏠 Smart Room Assistant (Humidity Monitoring System)

The Smart Room Assistant is an Arduino-based environmental monitoring system designed to track real-time humidity levels and provide instant visual feedback using both an OLED display and an RGB LED indicator.

The system uses a DHT11 sensor to continuously measure room humidity and classifies the environment into three states: good, moderate, and high humidity. Based on these readings, it dynamically changes the RGB LED color (green, yellow, red) to give an immediate visual alert, while simultaneously displaying detailed information on a 0.96” OLED screen.

This project demonstrates core concepts of IoT and embedded systems, including sensor integration, real-time data processing, display interfacing, and basic decision-making logic. It serves as a foundation for building more advanced smart home automation systems.


connections on breadboard:

1. DHT11 Sensor Connections
VCC → 5V (Arduino)
GND → GND (Arduino)
DATA → Digital Pin 2
(Optional: 10k pull-up resistor between VCC and DATA for stable readings)
💡 2. RGB LED Connections (Common Cathode type assumed)
Red pin → Digital Pin 9 (with 220Ω resistor)
Green pin → Digital Pin 10 (with 220Ω resistor)
Blue pin → Digital Pin 11 (with 220Ω resistor)
Common Cathode (longest pin) → GND rail on breadboard

⚠️ If using common anode RGB LED, connect the common pin to 5V instead of GND and invert logic in code.

📺 3. OLED Display (I2C 0.96” SSD1306)
VCC → 5V
GND → GND
SCL → A5 (Arduino UNO)
SDA → A4 (Arduino UNO)

(For ESP32: SDA = GPIO 21, SCL = GPIO 22)

🔋 4. Power Distribution (Breadboard Setup)
Arduino 5V → Breadboard + rail
Arduino GND → Breadboard – rail
All components share common ground (important for stable sensor readings)


ANY DOUBTS OR QUERIES USE AI FOR HELP !!!