// SenderControls.cpp
#include "SenderControls.h"

void SenderControls::begin()
{
    pinMode(encoderButtonPin, INPUT_PULLUP);
    pinMode(backButtonPin, INPUT_PULLUP);
    pinMode(clearButtonPin, INPUT_PULLUP);

    lastEncoderPosition = encoder.getPosition();
    lastEncoderButtonState = digitalRead(encoderButtonPin) == LOW; // Gehen Sie davon aus, dass beim Start keine Tasten gedrückt sind.
    lastBackButtonState = digitalRead(backButtonPin) == LOW;
    lastClearButtonState = digitalRead(clearButtonPin) == LOW;

    events.encoderDirection = 0;
    events.encoderPressed = false;
    events.backPressed = false;
    events.clearPressed = false;
}
void SenderControls::update()
{
    // Ereignisse jedes Mal zurücksetzen
    events.encoderDirection = 0;
    events.encoderPressed = false;
    events.backPressed = false;
    events.clearPressed = false;
    constexpr int32_t PULSES_PER_STEP = 2;

    const int32_t currentEncoderPosition = encoder.getPosition();

    const int32_t difference = currentEncoderPosition - lastEncoderPosition;

    if (difference >= PULSES_PER_STEP)
    {
        events.encoderDirection = 1;
        lastEncoderPosition += PULSES_PER_STEP;
    }
    else if (difference <= -PULSES_PER_STEP)
    {
        events.encoderDirection = -1;
        lastEncoderPosition -= PULSES_PER_STEP;
    }

    // Aktuelle Tasterzustände einlesen
    encoderButtonState = digitalRead(encoderButtonPin) == LOW;
    backButtonState = digitalRead(backButtonPin) == LOW;
    clearButtonState = digitalRead(clearButtonPin) == LOW;

    // Nur beim Übergang von losgelassen zu gedrückt auslösen
    events.encoderPressed =
        encoderButtonState && !lastEncoderButtonState;

    events.backPressed =
        backButtonState && !lastBackButtonState;

    events.clearPressed =
        clearButtonState && !lastClearButtonState;

    // Aktuelle Zustände für den nächsten Durchlauf merken
    lastEncoderButtonState = encoderButtonState;
    lastBackButtonState = backButtonState;
    lastClearButtonState = clearButtonState;
}
