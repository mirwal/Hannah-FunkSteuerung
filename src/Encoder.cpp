// Encoder.cpp
#include "Encoder.h"

Encoder *Encoder::instance = nullptr;

Encoder::Encoder(uint8_t encoderPinA, uint8_t encoderPinB) : encoderPinA(encoderPinA), encoderPinB(encoderPinB), pulses(0), previousState(0)
{
    instance = this;
}

void Encoder::begin()
{
    pinMode(encoderPinA, INPUT_PULLUP);
    pinMode(encoderPinB, INPUT_PULLUP);

    previousState =
        (digitalRead(encoderPinA) << 1) |
        digitalRead(encoderPinB);

    attachInterrupt(
        digitalPinToInterrupt(encoderPinA),
        interruptHandler,
        CHANGE);

    attachInterrupt(
        digitalPinToInterrupt(encoderPinB),
        interruptHandler,
        CHANGE);
}

uint16_t Encoder::getDegrees() const
{
    noInterrupts();
    const int32_t currentPulses = pulses;
    interrupts();
    const uint16_t encoderDeg = ((currentPulses * 10L) % 360 + 360) % 360; // Update the encoder degree value
    return encoderDeg;
}

int32_t Encoder::getPosition() const
{
    noInterrupts();
    const int32_t currentPulses = pulses;
    interrupts();
    return currentPulses;
}

int8_t Encoder::getDirection() const
{
    return direction;
}

void Encoder::interruptHandler()
{
    if (instance)
    {
        instance->handleInterrupt();
    }
}

void Encoder::handleInterrupt()
{
    //! Hier musss noch einer entprellung eingebaut werden, sonst werden die pulses zu schnell verändert und es kommt zu fehlern.
    // delay(10); // Kurze Verzögerung, um Prellen zu vermeiden

    uint8_t currentState = (digitalRead(encoderPinA) << 1) | digitalRead(encoderPinB);
    uint8_t stateChange = (previousState << 2) | currentState;

    if (previousState == currentState)
    {
        return; // Kein Zustandswechsel, keine Aktion
    }

    switch (stateChange)
    {
    case 0b0001:
    case 0b0111:
    case 0b1110:
    case 0b1000:
        pulses++;
        direction = 1; // +1 rechts
        break;
    case 0b0010:
    case 0b0100:
    case 0b1101:
    case 0b1011:
        pulses--;
        direction = -1; // -1 links
        break;
    default:
        // bei ungültigem Zustandswechsel, keine Aktion
        break;
    }
    previousState = currentState;
}