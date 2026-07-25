#include "FunkSteuerung.h"
#include "Encoder.h"
#include "InactivityWarning.h"
#include "BatteryMonitor.h"
#include "SenderControls.h"
#include "SenderDisplay.h"
#include "Menu.h"
#include "TimingData.h"
#include <Arduino.h>

FunkSteuerung funkSteuerung(Serial1);

// const int8_t DISABLE_CHIP_SELECT = -1;
// #include <SPI.h>
// #include <SdFat.h>
// in naher zukunft wird die SD-Karte nicht mehr benötigt.
// SdFat sd;
// SD card chip select
// int chipSelect = 53;

constexpr uint8_t VIBRATOR_PIN = 7;
BatteryMonitor batteryMonitor(A15, 7.0, (5.00 / 1023.00 * 2), VIBRATOR_PIN);

TimingDataManager timingDataManager;

constexpr uint32_t INACTIVITY_TIME_MS = 50UL * 60UL * 1000UL;
constexpr uint32_t VIBRATION_TIME_MS = 500UL;
InactivityWarning inactivityWarning(INACTIVITY_TIME_MS, VIBRATOR_PIN, VIBRATION_TIME_MS, 120);

constexpr int ANALOG_TOLERANCE = 20;
uint16_t lastAnalogValues[4];

//  ############################### ############################### ###############################
constexpr uint8_t ENCODER_PIN_A = 3; // Our first hardware interrupt pin is digital pin 2
constexpr uint8_t ENCODER_PIN_B = 2; // Our second hardware interrupt pin is digital pin 3
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);
// ############################### ############################### ###############################

constexpr uint8_t PIN_T_ENCODER = 37;
constexpr uint8_t PIN_CLEAR = 48;
constexpr uint8_t PIN_BACK = 49;
SenderControls senderControls(encoder, PIN_T_ENCODER, PIN_BACK, PIN_CLEAR);

SenderDisplay senderDisplay;

Menu menu(senderDisplay, funkSteuerung);
//! ###############################################################################################

void setup()
{

  Serial.begin(115200);
  inactivityWarning.begin();
  batteryMonitor.begin();
  funkSteuerung.begin(115200);
  menu.begin();
  senderControls.begin();

  Serial.println("FunkSteuerung Startet...");

  pinMode(36, OUTPUT); // encoder VCC
  encoder.begin();
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

void inactivityUpdate()
{
  static uint32_t lastLoopTime = 0;
  uint32_t currentTime = millis();

  if ((currentTime - lastLoopTime) < 500)
    return; // Check every 500 ms

  if (isHebelBewegung())
    inactivityWarning.resetActivity();

  lastLoopTime = currentTime;
  inactivityWarning.update();
}

//! ###############################################################################################

void loop()
{
  timingDataManager.updateLoopInterval(micros());

  senderControls.update();
  funkSteuerung.update();
  inactivityUpdate();
  batteryMonitor.update();

  menu.update(senderControls.getEvents(), funkSteuerung.getData(), timingDataManager.getTimingData(), batteryMonitor.getData());
}
