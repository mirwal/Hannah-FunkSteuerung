// BatteryMonitor.h
#pragma once

#include <Arduino.h>

class BatteryMonitor
{
public:
    BatteryMonitor(uint8_t analogPin, float warningVoltage, float voltageFactor)
        : analogPin(analogPin), warningVoltage(warningVoltage), voltageFactor(voltageFactor) {}

    void begin();
    void update();

    float getVoltage() const;
    bool isLow() const;

private:
    uint8_t analogPin;

    float warningVoltage;
    float voltageFactor;
    float voltage = 0.0f;

    uint32_t lastMeasurementTime = 0;
    uint32_t measurementInterval = 1000;
};