// BatteryMonitor.cpp
#include "BatteryMonitor.h"

void BatteryMonitor::begin()
{
    pinMode(analogPin, INPUT); // Initialize the analog pin (optional)

    int rawValue = analogRead(analogPin);
    voltage = rawValue * voltageFactor;

    lastMeasurementTime = millis();
}

void BatteryMonitor::update()
{
    uint32_t currentTime = millis();

    if (currentTime - lastMeasurementTime >= measurementInterval)
    {
        lastMeasurementTime = currentTime;

        int rawValue = analogRead(analogPin);
        voltage = rawValue * voltageFactor;
    }
}

float BatteryMonitor::getVoltage() const
{
    return voltage;
}

bool BatteryMonitor::isLow() const
{
    return voltage <= warningVoltage;
}
