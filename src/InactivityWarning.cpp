#include "InactivityWarning.h"

void InactivityWarning::begin()
{
    pinMode(outputPin, OUTPUT);
    analogWrite(outputPin, 0);

    lastActivityTime = millis();
    warningStartTime = 0;
    inactive = false;
    warningActive = false;
}

void InactivityWarning::update()
{
    uint32_t currentTime = millis();

    // Inaktivität wurde gerade neu erreicht
    if (!inactive && currentTime - lastActivityTime >= timeout)
    {
        inactive = true;
        warningActive = true;
        warningStartTime = currentTime;

        analogWrite(outputPin, pwm);
    }

    // Laufzeit der Warnung ist vorbei
    if (warningActive && currentTime - warningStartTime >= runTime)
    {
        warningActive = false;
        analogWrite(outputPin, 0);
    }
}
void InactivityWarning::resetActivity()
{
    lastActivityTime = millis();
    inactive = false;

    if (warningActive)
    {
        warningActive = false;
        analogWrite(outputPin, 0);
    }
}
