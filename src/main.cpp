#include "FunkSteuerung.h"
#include "Encoder.h"
#include "InactivityWarning.h"
#include "BatteryMonitor.h"
#include "SenderControls.h"
#include "Menu.h"
#include <U8g2lib.h>
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#include "wifi_search_32_32_7f.h"
// diese ichons und schriften sind in der u8g2 library enthalten und können direkt verwendet werden
// https://github.com/olikraus/u8g2/wiki/fntlist8?utm_source=chatgpt.com#u8g2-font-list
// oberfläche design https://lopaka.app/projects
#define Font_5x8 u8g2_font_5x8_mf // Number font
#define Font_8x8 u8g2_font_artossans8_8r
#define Font_6x12 u8g2_font_6x12_m_symbols
#define Font_Battery u8g2_font_battery19_tn        // Battery symbol font 0x00 bis 0x06
#define Font_VCR u8g2_font_VCR_OSD_mr              // VCR font
#define Font_Icons u8g2_font_open_iconic_arrow_4x4 // Icon font

struct SymbolIchon
{
  const char *alarm = "A";
  const char *einstallungen = "H";
  const char *achtung = "G";
  const char *blitz = "C";
  const char *offline = "D";
  const char *batteryVoll = "I";
  const char *batteryLeer = "@";
  const char *zuruck = "O";
  const char *online = "P";
};

/////////////////////////////// 128x64 SSD1309 ///////////////////////////
constexpr uint8_t OLED_CS = 4;
constexpr uint8_t OLED_DC = 5;
constexpr uint8_t OLED_RST = 6;

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI display(U8G2_R0, OLED_CS, OLED_DC, OLED_RST);

/////////////////////////////////////////////////////////////////////////
FunkSteuerung funkSteuerung(Serial1);

/////////////////////////////////////////////////////////////////////////
const int8_t DISABLE_CHIP_SELECT = -1;
// File system object.
SdFat sd;
// SD card chip select
int chipSelect = 53;

/////////////////////////////////////////////////////////////////////////

BatteryMonitor batteryMonitor(A15, 7.0, 5.00 / 1023.00 * 2);
constexpr uint32_t BATTERY_WARNING_INTERVAL_MS = 60UL * 60UL * 1000UL;
uint32_t lastBatteryWarningTime_ms = 0;

/////////////////////////////////////////////////////////////////////////
constexpr uint8_t VIBRATOR_PIN = 7;
constexpr uint32_t INACTIVITY_TIME_MS = 50UL * 60UL * 1000UL;
constexpr uint32_t VIBRATION_TIME_MS = 500UL;

InactivityWarning inactivityWarning(INACTIVITY_TIME_MS, VIBRATOR_PIN, VIBRATION_TIME_MS, 120);

constexpr int ANALOG_TOLERANCE = 20;
uint16_t lastAnalogValues[4];

/////////////////////////////////////////////////////////////////////////

//  ############################### ############################### ###############################

constexpr uint8_t ENCODER_PIN_A = 3; // Our first hardware interrupt pin is digital pin 2
constexpr uint8_t ENCODER_PIN_B = 2; // Our second hardware interrupt pin is digital pin 3
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);

// ############################### ############################### ###############################

constexpr uint8_t PIN_T_ENCODER = 37;
constexpr uint8_t PIN_CLEAR = 48;
constexpr uint8_t PIN_BACK = 49;
SenderControls senderControls(encoder, PIN_T_ENCODER, PIN_BACK, PIN_CLEAR);

/////////////////////////////////////////////////////////////////////////

Menu menu;

/////////////////////////////////////////////////////////////////////////

bool initializeDisplay();

void updateAnimations()
{
  wifi_search_32_32_7f_frame = millis() / 71 % 8;
}

void drawAnimation_wifi_search_32_32_7f(void)
{
  display.setDrawColor(1);
  display.drawXBMP(48, -8, 32, 32, wifi_search_32_32_7f_frames[wifi_search_32_32_7f_frame]);
}
void drawINTRO(void)
{
  display.setFontMode(1);
  display.setBitmapMode(1);
  display.setFont(Font_8x8);
  display.drawStr(47, 32, "HannaH");
  display.drawStr(25, 46, "Funksteuerung");
  drawAnimation_wifi_search_32_32_7f();
}
//! ###############################################################################################

void setup()
{

  Serial.begin(115200);
  encoder.begin();
  inactivityWarning.begin();
  batteryMonitor.begin();
  funkSteuerung.begin(115200);
  menu.begin();
  senderControls.begin();

  Serial.println("FunkSteuerung Startet...");

  pinMode(36, OUTPUT);    // encoder VCC
  digitalWrite(36, HIGH); // Set the encoder VCC pin to HIGH to power the encoder

  lastAnalogValues[0] = analogRead(A0);
  lastAnalogValues[1] = analogRead(A1);
  lastAnalogValues[2] = analogRead(A2);
  lastAnalogValues[3] = analogRead(A3);

  initializeDisplay();
}

//! ###############################################################################################

bool isHebelBewegung()
{

  for (uint8_t i = 0; i < 4; i++)
  {
    const uint16_t currentValue = analogRead(A0 + i); // A0, A1, A2, A3

    if (abs(static_cast<int>(currentValue) - static_cast<int>(lastAnalogValues[i])) >= ANALOG_TOLERANCE)
    {
      lastAnalogValues[i] = currentValue;
      return true; // Hebelbewegung erkannt
    }
  }

  return false; // Keine Hebelbewegung erkannt
}

//! ###############################################################################################

void printDisplays()
{
  static unsigned long lastPrintTime_ms = 0;
  static MaskData displays_MaskData;
  static MaskData newDisplays_MaskData;
  if (millis() - lastPrintTime_ms >= 20)
  {

    lastPrintTime_ms = millis();
    funkSteuerung.getMaskData(newDisplays_MaskData);
    const int analog6 = analogRead(6);

    if (displays_MaskData != newDisplays_MaskData)
    {

      displays_MaskData = newDisplays_MaskData;
    }

    // uint16_t y1 = map(analogRead(A0), 0, 1023, -200, 200);
    // uint16_t x1 = map(analogRead(A1), 0, 1023, 200, -200);
    // uint16_t y2 = map(analogRead(A2) + 24, 0, 1023, -200, 200);
    // uint16_t x2 = map(analogRead(A3) - 2, 0, 1023, -200, 200);

    if (analog6 < 1000) ///! nur mit debnug schalter stellung
    {

      if ((displays_MaskData.tasterMask != newDisplays_MaskData.tasterMask) && (analog6 < 1000))
      {

        if (bitRead(displays_MaskData.tasterMask, 0) == 1)
          Serial.println("Taster 1 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 1) == 1)
          Serial.println("Taster 2 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 2) == 1)
          Serial.println("Taster 3 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 3) == 1)
          Serial.println("Taster 4 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 4) == 1)
          Serial.println("Taster 5 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 5) == 1)
          Serial.println("Taster 6 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 6) == 1)
          Serial.println("Taster 7 gedrückt");
        if (bitRead(displays_MaskData.tasterMask, 7) == 1)
          Serial.println("Taster 8 gedrückt");
      }
    }
  }
}
//! ###############################################################################################

void loop()
{
  inactivityWarning.update();
  batteryMonitor.update();
  senderControls.update();
  menu.update(senderControls.getEvents());

  // Battery überwachung und Vibration bei niedrigem Batteriestand
  if (batteryMonitor.isLow() && ((millis() - lastBatteryWarningTime_ms) >= BATTERY_WARNING_INTERVAL_MS))
  {
    lastBatteryWarningTime_ms = millis();

    analogWrite(VIBRATOR_PIN, 160);
    delay(2000);
    analogWrite(VIBRATOR_PIN, 0);
  }
  const uint32_t loopStart = micros();
  static unsigned long lastPrintTime_ms = 0, countDatenGesamt = 0;

  if (funkSteuerung.update()) // die Funksteuerung hat neue Daten empfangen, dann erst weiter dann darf der restliche Code ausgeführt werden
  {

    const int analog6 = analogRead(6);
    countDatenGesamt++;

    display.clearBuffer();
    // updateAnimations();
    drawAnimation_wifi_search_32_32_7f();
    display.setFont(Font_5x8);
    display.drawStr(10, 25, static_cast<String>(countDatenGesamt).c_str());
    display.setFont(Font_6x12);
    display.drawStr(1, 50, "Funksteuerung");
    display.sendBuffer();

    if (isHebelBewegung())
    {
      inactivityWarning.resetActivity();
    }

    if (analogRead(6) < 200)
    {

      Serial.println("Encoder Grad: " + String(encoder.getDegrees()));
    }

    const uint32_t loopDuration = micros() - loopStart;

    static uint32_t minimumDuration = UINT32_MAX;
    static uint32_t maximumDuration = 0;
    static uint32_t durationSum = 0;
    static uint32_t loopCount = 0;

    if (loopDuration < minimumDuration)
    {
      minimumDuration = loopDuration;
    }

    if (loopDuration > maximumDuration)
    {
      maximumDuration = loopDuration;
    }

    durationSum += loopDuration;
    loopCount++;

    if (millis() - lastPrintTime_ms >= 1000 && analog6 >= 1000) // Print every second or if analog pin 6 is high
    {
      lastPrintTime_ms = millis();

      const uint32_t averageDuration = durationSum / loopCount;

      Serial.print("Loop min: ");
      Serial.print(minimumDuration);
      Serial.print(" us, mittel: ");
      Serial.print(averageDuration);
      Serial.print(" us, max: ");
      Serial.print(maximumDuration);
      Serial.println(" us");
      minimumDuration = UINT32_MAX;
      maximumDuration = 0;
      durationSum = 0;
      loopCount = 0;
    }
  }
}
//! ###############################################################################################

bool initializeDisplay()
{
  display.begin();
  delay(100);

  display.clearBuffer();
  display.setContrast(5); // Set contrast based on dimDisplay parameter
  display.setFont(Font_Battery);
  display.setFontPosTop();
  display.enableUTF8Print(); // Disable UTF-8 support for special characters
  display.setCursor(120, 1);
  display.print(0x04); // Print the battery symbol
  display.setFontDirection(0);
  display.setFont(Font_8x8);
  display.setCursor(0, 0);
  display.print("Funksteuerung");
  display.setCursor(0, 18);
  display.setFont(Font_6x12);
  display.print("für hannah hexapod");
  display.setCursor(0, 38);
  SymbolIchon SymbolIchon;
  uint8_t i = 0;
  uint8_t j = 18;

  display.setFont(u8g2_font_open_iconic_embedded_2x_t);
  display.drawUTF8(i += 1, 38, SymbolIchon.alarm);         // Print the alarm symbol
  display.drawUTF8(i += j, 38, SymbolIchon.einstallungen); // Print the settings symbol
  display.drawUTF8(i += j, 38, SymbolIchon.achtung);       // Print the warning symbol
  display.drawUTF8(i += j, 38, SymbolIchon.offline);       // Print the offline symbol
  display.drawUTF8(i += j, 38, SymbolIchon.blitz);         // Print the battery symbol
  display.drawUTF8(i += j, 38, SymbolIchon.zuruck);        // Print the back symbol
  display.drawUTF8(i += j, 38, SymbolIchon.online);        // Print the online symbol
  display.setFont(Font_6x12);
  display.sendBuffer();
  return true;
}