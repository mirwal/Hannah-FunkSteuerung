#pragma once

#include <Arduino.h>

class InactivityWarning
{
public:
    InactivityWarning(uint32_t timeout, uint8_t outputPin, uint32_t runTime, uint8_t pwm = 255)
        : timeout(timeout), outputPin(outputPin), runTime(runTime), pwm(pwm) {}

    void begin();
    void update();
    void resetActivity();

    bool isInactive() const { return inactive; }
    bool isWarningActive() const { return warningActive; }

private:
    uint32_t timeout;
    uint8_t outputPin;
    uint32_t runTime;
    uint8_t pwm;

    uint32_t lastActivityTime = 0;
    uint32_t warningStartTime = 0;

    bool inactive = false;
    bool warningActive = false;
};