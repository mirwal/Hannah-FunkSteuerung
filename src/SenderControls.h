// SenderControls.h
#pragma once
#include <Arduino.h>
#include "Encoder.h"
#include "FunkSteuerung.h"
struct SenderControlEvents
{
    int8_t encoderDirection = 0; // -1 links, 0 nichts, +1 rechts

    bool encoderPressed = false;
    bool backPressed = false;
    bool clearPressed = false;
};

class SenderControls
{
public:
    SenderControls(Encoder &encoder, uint8_t encoderButtonPin, uint8_t backButtonPin, uint8_t clearButtonPin)
        : encoder(encoder), encoderButtonPin(encoderButtonPin), backButtonPin(backButtonPin), clearButtonPin(clearButtonPin)
    {
    }

    void begin();
    void update();

    bool isEncoderPressed() const { return events.encoderPressed; }
    bool isBackPressed() const { return events.backPressed; }
    bool isClearPressed() const { return events.clearPressed; }
    int8_t isEncoderRotated() const { return events.encoderDirection; }

    const SenderControlEvents &getEvents() const { return events; }

private:
    Encoder &encoder;

    uint8_t encoderButtonPin;
    uint8_t backButtonPin;
    uint8_t clearButtonPin;

    int32_t lastEncoderPosition = 0;
    uint32_t lastEncoderUpdate = 0;

    bool lastEncoderButtonState = false;
    bool lastBackButtonState = false;
    bool lastClearButtonState = false;

    bool encoderButtonState = false;
    bool backButtonState = false;
    bool clearButtonState = false;

    SenderControlEvents events;
};