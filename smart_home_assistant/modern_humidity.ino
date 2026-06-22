#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define DHTPIN 2
#define DHTTYPE DHT11

#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

DHT dht(DHTPIN, DHTTYPE);

// CHANGE THIS IF NEEDED (0x3C or 0x3D)
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setColor(bool r, bool g, bool b)
{
  digitalWrite(RED_PIN, r);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  setColor(0, 0, 0); // RGB OFF at startup

  Serial.begin(9600);
  Serial.println("Starting system...");

  dht.begin();

  // OLED INIT
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED NOT FOUND! Check wiring or address.");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  Serial.println("System Ready");
}

void loop() {

  float humidity = dht.readHumidity();

  Serial.print("Humidity: ");
  Serial.println(humidity);

  // If DHT fails, don't crash system
  if (isnan(humidity)) {
    Serial.println("DHT11 ERROR");
    setColor(1, 0, 0); // RED alert
    delay(1000);
    return;
  }

  // RGB logic
  if (humidity < 40) {
    setColor(0, 1, 0); // Green
  }
  else if (humidity < 70) {
    setColor(1, 1, 0); // Yellow
  }
  else {
    setColor(1, 0, 0); // Red
  }

  // OLED DISPLAY
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SMART ROOM");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(humidity);
  display.print("%");

  display.setTextSize(1);
  display.setCursor(0, 50);

  if (humidity < 40) {
    display.print("GOOD AIR");
  }
  else if (humidity < 70) {
    display.print("MODERATE");
  }
  else {
    display.print("HIGH HUMIDITY");
  }

  display.display();

  delay(1000);
}