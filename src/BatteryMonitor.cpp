// BatteryMonitor.cpp
#include "BatteryMonitor.h"

void BatteryMonitor::begin()
{
    pinMode(analogPin, INPUT); // Initialize the analog pin (optional)

    int rawValue = analogRead(analogPin);
    batteryData.voltage = rawValue * voltageFactor;

    batteryData.isLow = batteryData.voltage <= warningVoltage;
    lastMeasurementTime = millis();
}

void BatteryMonitor::update()
{
    uint32_t currentTime = millis();
    // Messen der Batteriespannung
    if (currentTime - lastMeasurementTime >= measurementInterval)
    {
        lastMeasurementTime = currentTime;
        int rawValue = analogRead(analogPin);
        batteryData.voltage = rawValue * voltageFactor;
        batteryData.isLow = batteryData.voltage <= warningVoltage;
    }

    // Überprüfen ob Alarm aktiv ist und ob die Zeit seit der letzten Messung größer als lastIsAlarmActiveTime_ms ist
    if (isAlarmActive && currentTime - alarmStartTime_ms >= alarmActiveDuration_ms)
    {
        //! muss noch aus bleiben da ich noch kein Akku angeschlossen habe und ich nicht will das der Alarm losgeht
        //   analogWrite(VIBRATOR_PIN, 0);
        isAlarmActive = false;
        lastAlarmTime_ms = currentTime;
    }

    // Überprüfen ob die Batteriespannung unter dem Warnwert liegt und ob die Zeit seit der letzten Warnung größer als batteryWarningInterval_ms ist
    if (isLow() && !isAlarmActive && currentTime - lastAlarmTime_ms >= alarmRepeatInterval_ms)
    {
        //! muss noch aus bleiben da ich noch kein Akku angeschlossen habe und ich nicht will das der Alarm losgeht
        //  analogWrite(VIBRATOR_PIN, 120);
        isAlarmActive = true;
        batteryData.isLow = true;
        alarmStartTime_ms = currentTime;
    }
}
float BatteryMonitor::getVoltage() const
{
    return batteryData.voltage;
}

bool BatteryMonitor::isLow() const
{
    return batteryData.isLow;
}
