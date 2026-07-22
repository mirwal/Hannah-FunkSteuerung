// Encoder.h
#pragma once

#include <Arduino.h>

class Encoder
{
public:
    Encoder(uint8_t encoderPinA, uint8_t encoderPinB);

    void begin();
    uint16_t getDegrees() const;
    int32_t getPosition() const;
    int8_t getDirection() const; // -1 links, 0 nichts, +1 rechts

private:
    uint8_t encoderPinA;
    uint8_t encoderPinB;

    volatile int32_t pulses = 0;
    volatile int8_t direction = 0; // -1 links, 0 nichts, +1 rechts
    volatile uint8_t previousState = 0;
    volatile uint32_t lastInterruptTime = 0;

    // Eine Sache fehlt noch: attachInterrupt() kann handleInterrupt() nicht direkt aufrufen,
    // weil es eine normale Objektmethode ist.
    // Wir benötigen eine statische Weiterleitung und einen Zeiger auf das zuständige Objekt:
    static Encoder *instance;
    static void interruptHandler();

    void handleInterrupt();
};