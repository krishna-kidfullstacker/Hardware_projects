#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ---------- BUTTONS ----------
const int BTN_UP = 2;
const int BTN_DOWN = 33;
const int BTN_COLOR = 41;
const int BTN_MODE = 42;
const int BTN_OK = 44;

// ---------- RGB ----------
const int LED_R = 38;
const int LED_G = 39;
const int LED_B = 40;

// ---------- BUZZER ----------
const int BUZZER = 48;

// ---------- TFT ----------
const int TFT_SCLK = 12;
const int TFT_MOSI = 11;
const int TFT_MISO = 13;

const int TFT_CS = 10;
const int TFT_DC = 9;
const int TFT_RST = 8;

// ---------- RTC ----------
const int RTC_SDA = 10;
const int RTC_SCL = 9;


// ---------- RTC OBJECT ----------
RTC_DS3231 rtc;
DateTime now;

unsigned long lastRTCUpdate = 0;
const unsigned long RTC_INTERVAL = 1000;


// ---------- TFT OBJECT ----------
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);


// ---------- CLOCK MODES ----------
enum ClockMode
{
  HOME,
  SETTINGS,
  STOPWATCH,
  ALARM
};

ClockMode currentMode = HOME;
ClockMode lastDisplayedMode = HOME;


// ---------- BUTTON STATES ----------
bool lastUp = HIGH;
bool lastDown = HIGH;
bool lastColor = HIGH;
bool lastMode = HIGH;
bool lastOk = HIGH;

bool upPressed = false;
bool downPressed = false;
bool colorPressed = false;
bool modePressed = false;
bool okPressed = false;


// ---------- STOPWATCH ----------
bool stopwatchRunning = false;

unsigned long stopwatchStartTime = 0;
unsigned long stopwatchElapsed = 0;


// ---------- ALARM ----------
int alarmHour = 7;
int alarmMinute = 0;

bool alarmEnabled = false;
bool alarmRinging = false;

int lastAlarmDay = -1;


// ---------- RGB ----------
int selectedColor = 0;


// ---------- DISPLAY ----------
int lastSecond = -1;
unsigned long lastStopwatchDisplay = 0;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  setupHardware();
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  readButtons();
  readRTC();
  handleButtons();
  updateStopwatch();
  checkAlarm();
  updateDisplay();
  updateRGB();
}


// =====================================================
// HARDWARE SETUP
// =====================================================

void setupHardware()
{
  Serial.begin(115200);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_COLOR, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);

  // RGB
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);

  // I2C
  Wire.begin(RTC_SDA, RTC_SCL);

  // RTC
  if (!rtc.begin())
  {
    Serial.println("RTC not found!");

    while (1);
  }

  setupTFT();

  Serial.println("Digital Clock Started!");
}


// =====================================================
// RTC
// =====================================================

void readRTC()
{
  if (millis() - lastRTCUpdate >= RTC_INTERVAL)
  {
    lastRTCUpdate = millis();

    now = rtc.now();

    Serial.print("Time: ");

    if (now.hour() < 10)
      Serial.print("0");

    Serial.print(now.hour());
    Serial.print(":");

    if (now.minute() < 10)
      Serial.print("0");

    Serial.print(now.minute());
    Serial.print(":");

    if (now.second() < 10)
      Serial.print("0");

    Serial.println(now.second());
  }
}


// =====================================================
// BUTTON READING
// =====================================================

void readButtons()
{
  bool up = digitalRead(BTN_UP);
  bool down = digitalRead(BTN_DOWN);
  bool color = digitalRead(BTN_COLOR);
  bool mode = digitalRead(BTN_MODE);
  bool ok = digitalRead(BTN_OK);

  upPressed = (up == LOW && lastUp == HIGH);
  downPressed = (down == LOW && lastDown == HIGH);
  colorPressed = (color == LOW && lastColor == HIGH);
  modePressed = (mode == LOW && lastMode == HIGH);
  okPressed = (ok == LOW && lastOk == HIGH);

  lastUp = up;
  lastDown = down;
  lastColor = color;
  lastMode = mode;
  lastOk = ok;
}


// =====================================================
// BUTTON HANDLING
// =====================================================

void handleButtons()
{
  // ---------- MODE BUTTON ----------

  if (modePressed)
  {
    if (currentMode == HOME)
      currentMode = SETTINGS;

    else if (currentMode == SETTINGS)
      currentMode = STOPWATCH;

    else if (currentMode == STOPWATCH)
      currentMode = ALARM;

    else if (currentMode == ALARM)
      currentMode = HOME;
  }


  // ---------- UP BUTTON ----------

  if (upPressed)
  {
    if (currentMode == STOPWATCH)
    {
      stopwatchRunning = true;

      stopwatchStartTime =
        millis() - stopwatchElapsed;
    }

    else if (currentMode == ALARM)
    {
      alarmHour++;

      if (alarmHour >= 24)
        alarmHour = 0;
    }
  }


  // ---------- DOWN BUTTON ----------

  if (downPressed)
  {
    if (currentMode == STOPWATCH)
    {
      stopwatchRunning = false;
      stopwatchElapsed = 0;
    }

    else if (currentMode == ALARM)
    {
      alarmMinute += 5;

      if (alarmMinute >= 60)
      {
        alarmMinute = 0;
        alarmHour++;

        if (alarmHour >= 24)
          alarmHour = 0;
      }
    }
  }


  // ---------- COLOR BUTTON ----------

  if (colorPressed)
  {
    selectedColor++;

    if (selectedColor > 2)
      selectedColor = 0;
  }


  // ---------- OK BUTTON ----------

  if (okPressed)
  {
    // Stopwatch start / pause
    if (currentMode == STOPWATCH)
    {
      stopwatchRunning = !stopwatchRunning;

      if (stopwatchRunning)
      {
        stopwatchStartTime =
          millis() - stopwatchElapsed;
      }
    }

    // Alarm enable / disable
    if (currentMode == ALARM)
    {
      alarmEnabled = !alarmEnabled;

      if (!alarmEnabled)
      {
        alarmRinging = false;
        noTone(BUZZER);
      }
    }

    // Stop alarm
    if (alarmRinging)
    {
      alarmRinging = false;
      noTone(BUZZER);
    }
  }
}


// =====================================================
// STOPWATCH
// =====================================================

void updateStopwatch()
{
  if (stopwatchRunning)
  {
    stopwatchElapsed =
      millis() - stopwatchStartTime;
  }
}


// =====================================================
// ALARM
// =====================================================

void checkAlarm()
{
  if (!alarmEnabled)
  {
    noTone(BUZZER);
    return;
  }


  // Trigger only once per day

  if (now.hour() == alarmHour &&
      now.minute() == alarmMinute &&
      now.second() == 0 &&
      lastAlarmDay != now.day())
  {
    alarmRinging = true;

    lastAlarmDay = now.day();
  }


  if (alarmRinging)
  {
    tone(BUZZER, 2000);
  }
  else
  {
    noTone(BUZZER);
  }
}


// =====================================================
// DISPLAY
// =====================================================

void updateDisplay()
{
  // ---------- MODE CHANGE ----------

  if (currentMode != lastDisplayedMode)
  {
    tft.fillScreen(ST77XX_BLACK);

    lastDisplayedMode = currentMode;

    lastSecond = -1;

    lastStopwatchDisplay = 0;
  }


  // ---------- HOME ----------

  if (currentMode == HOME)
  {
    if (now.second() != lastSecond)
    {
      lastSecond = now.second();

      drawHomeScreen();
    }
  }


  // ---------- SETTINGS ----------

  else if (currentMode == SETTINGS)
  {
    static int lastColor = -1;

    if (lastColor != selectedColor)
    {
      lastColor = selectedColor;

      drawSettingsScreen();
    }
  }


  // ---------- STOPWATCH ----------

  else if (currentMode == STOPWATCH)
  {
    // Update stopwatch display every 100 ms

    if (millis() - lastStopwatchDisplay >= 100)
    {
      lastStopwatchDisplay = millis();

      drawStopwatchScreen();
    }
  }


  // ---------- ALARM ----------

  else if (currentMode == ALARM)
  {
    static int lastAlarmHour = -1;
    static int lastAlarmMinute = -1;
    static bool lastAlarmEnabled = false;
    static bool lastAlarmRinging = false;

    if (lastAlarmHour != alarmHour ||
        lastAlarmMinute != alarmMinute ||
        lastAlarmEnabled != alarmEnabled ||
        lastAlarmRinging != alarmRinging)
    {
      lastAlarmHour = alarmHour;
      lastAlarmMinute = alarmMinute;
      lastAlarmEnabled = alarmEnabled;
      lastAlarmRinging = alarmRinging;

      drawAlarmScreen();
    }
  }
}


// =====================================================
// HOME SCREEN
// =====================================================

void drawHomeScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);

  tft.setCursor(75, 20);
  tft.println("CLOCK");


  // TIME

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);

  tft.setCursor(35, 90);

  if (now.hour() < 10)
    tft.print("0");

  tft.print(now.hour());

  tft.print(":");

  if (now.minute() < 10)
    tft.print("0");

  tft.print(now.minute());


  // SECONDS

  tft.setTextSize(2);

  tft.setCursor(90, 145);

  if (now.second() < 10)
    tft.print("0");

  tft.print(now.second());


  // MENU

  tft.setTextColor(ST77XX_YELLOW);

  tft.setCursor(65, 190);

  tft.println("MODE = MENU");
}


// =====================================================
// SETTINGS SCREEN
// =====================================================

void drawSettingsScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);

  tft.setCursor(40, 30);

  tft.println("SETTINGS");


  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.setCursor(30, 90);

  tft.println("COLOR");


  tft.setCursor(30, 130);

  if (selectedColor == 0)
    tft.println("RED");

  else if (selectedColor == 1)
    tft.println("GREEN");

  else
    tft.println("BLUE");


  tft.setCursor(30, 190);

  tft.println("COLOR = CHANGE");
}


// =====================================================
// STOPWATCH SCREEN
// =====================================================

void drawStopwatchScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(3);

  tft.setCursor(35, 30);

  tft.println("STOPWATCH");


  unsigned long totalSeconds =
    stopwatchElapsed / 1000;

  unsigned long minutes =
    totalSeconds / 60;

  unsigned long seconds =
    totalSeconds % 60;


  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);

  tft.setCursor(40, 95);


  if (minutes < 10)
    tft.print("0");

  tft.print(minutes);

  tft.print(":");

  if (seconds < 10)
    tft.print("0");

  tft.print(seconds);


  tft.setTextSize(2);

  tft.setCursor(55, 170);


  if (stopwatchRunning)
    tft.println("RUNNING");

  else
    tft.println("STOPPED");


  tft.setCursor(25, 210);

  tft.println("UP=START DOWN=RESET");
}


// =====================================================
// ALARM SCREEN
// =====================================================

void drawAlarmScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(3);

  tft.setCursor(55, 30);

  tft.println("ALARM");


  // ALARM TIME

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);

  tft.setCursor(55, 95);


  if (alarmHour < 10)
    tft.print("0");

  tft.print(alarmHour);

  tft.print(":");

  if (alarmMinute < 10)
    tft.print("0");

  tft.print(alarmMinute);


  // STATUS

  tft.setTextSize(2);

  tft.setCursor(60, 165);


  if (alarmEnabled)
    tft.println("ENABLED");

  else
    tft.println("DISABLED");


  // RINGING

  if (alarmRinging)
  {
    tft.setTextColor(ST77XX_RED);

    tft.setCursor(60, 200);

    tft.println("RINGING!");
  }
}


// =====================================================
// RGB
// =====================================================

void updateRGB()
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);


  if (selectedColor == 0)
  {
    digitalWrite(LED_R, HIGH);
  }

  else if (selectedColor == 1)
  {
    digitalWrite(LED_G, HIGH);
  }

  else if (selectedColor == 2)
  {
    digitalWrite(LED_B, HIGH);
  }
}


// =====================================================
// TFT SETUP
// =====================================================

void setupTFT()
{
  SPI.begin(
    TFT_SCLK,
    TFT_MISO,
    TFT_MOSI,
    TFT_CS
  );

  tft.init(240, 240);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.println("DIGITAL CLOCK");
}
