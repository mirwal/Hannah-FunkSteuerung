#include "FunkSteuerung.h"
#include "Encoder.h"
#include "InactivityWarning.h"
#include "BatteryMonitor.h"
#include "SenderControls.h"
#include "SenderDisplay.h"
#include "Menu.h"
#include "TimingData.h"
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#include "wifi_search_32_32_7f.h"

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
TimingData timingData;
uint32_t lastLoopTime = 0;

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

SenderDisplay senderDisplay;
Menu menu(senderDisplay);

/////////////////////////////////////////////////////////////////////////

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

  senderDisplay.begin();
  delay(100);
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

void loop()
{
  const uint32_t currentTime = micros();

  if (lastLoopTime != 0)
  {
    timingData.loopInterval = currentTime - lastLoopTime;

    if (timingData.loopInterval > timingData.maxLoopInterval)
    {
      timingData.maxLoopInterval = timingData.loopInterval;
    }
  }

  lastLoopTime = currentTime;

  inactivityWarning.update();
  batteryMonitor.update();
  senderControls.update();

  if (senderControls.getEvents().clearPressed)
  {
    timingData.maxLoopInterval = timingData.loopInterval;
    funkSteuerung.resetMaxPacketInterval();
    Serial.println("Max Loop Interval and Max Packet Interval reset.");
  }

  menu.update(senderControls.getEvents(), funkSteuerung.getData(), timingData);

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

    if (isHebelBewegung())
    {
      inactivityWarning.resetActivity();
    }

    if (analogRead(6) < 200)
    {

      // Serial.println("Encoder Grad: " + String(encoder.getDegrees()));
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
      Serial.print(", avg: ");
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