// BatteryMonitor.h
#pragma once

#include <Arduino.h>
struct BatteryData
{
    float voltage = 0.0f;
    bool isLow = false;
};
class BatteryMonitor
{
public:
    BatteryMonitor(uint8_t analogPin, float warningVoltage, float voltageFactor, uint8_t VIBRATOR_PIN = 5)
        : analogPin(analogPin), warningVoltage(warningVoltage), voltageFactor(voltageFactor), VIBRATOR_PIN(VIBRATOR_PIN) {}

    void begin();
    void update();

    float getVoltage() const;
    bool isLow() const;
    const BatteryData &getData() const { return batteryData; }

private:
    BatteryData batteryData;
    uint8_t analogPin;
    float warningVoltage;
    float voltageFactor;
    uint8_t VIBRATOR_PIN;

    bool isAlarmActive = false;

    uint32_t lastMeasurementTime = 0;
    uint32_t measurementInterval = 1000;

    uint32_t lastAlarmTime_ms = 0;
    uint32_t alarmStartTime_ms = 0;

    uint32_t alarmActiveDuration_ms = 200;                    // 10 minutes
    uint32_t alarmRepeatInterval_ms = 6UL * 60UL * 1000UL;   // 6 minutes
};