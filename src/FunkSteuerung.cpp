// FunkSteuerung.cpp
#include "FunkSteuerung.h"

FunkSteuerung::FunkSteuerung(HardwareSerial &serial)
    : port(serial)
{
}

void FunkSteuerung::begin(uint32_t baudrate)
{
    port.begin(baudrate);

    for (uint8_t i = 0; i < TASTER_COUNT; i++)
    {
        pinMode(tasterPins[i], INPUT_PULLUP);

        bool raw = digitalRead(tasterPins[i]);

        tasterStable[i] = raw;
        tasterLastRaw[i] = raw;
        tasterLastChange[i] = millis();
    }
    for (uint8_t i = 0; i < SONDERTASTER_COUNT; i++)
    {
        pinMode(sonderTasterPins[i], INPUT_PULLUP);

        bool raw = digitalRead(sonderTasterPins[i]);

        sonderTasterStable[i] = raw;
        sonderTasterLastRaw[i] = raw;
        sonderTasterLastChange[i] = millis();
    }

    pinMode(PIN_HL_UD, INPUT); // Joystick rechts		up	 down
    pinMode(PIN_HL_LR, INPUT); // Joystick rechts		left right
    pinMode(PIN_HR_UD, INPUT); // Joystick links		up	 down
    pinMode(PIN_HR_LR, INPUT); // Joystick links		left right
    pinMode(PIN_POTI, INPUT);  // Poti orginal  aux2
    pinMode(PIN_FADER, INPUT); // Taster dreier
    pinMode(PIN_FLAP, INPUT);  // Faider

    pinMode(PIN_HR1, INPUT); // Taster CH[#] nutzung
    pinMode(PIN_HR2, INPUT);
    pinMode(PIN_HR3, INPUT);
    pinMode(PIN_HL1, INPUT);
    pinMode(PIN_HL2, INPUT);
    pinMode(PIN_HL3, INPUT);

    pinMode(PIN_TRAINER, INPUT);
    pinMode(PIN_T_ENCODER, INPUT); // encoder Taster

    pinMode(PIN_T1, INPUT_PULLUP); // Taster (JUSTIERUNG)
    pinMode(PIN_T2, INPUT_PULLUP);
    pinMode(PIN_T3, INPUT_PULLUP);
    pinMode(PIN_T4, INPUT_PULLUP);
    pinMode(PIN_T5, INPUT_PULLUP);
    pinMode(PIN_T6, INPUT_PULLUP);
    pinMode(PIN_T7, INPUT_PULLUP);
    pinMode(PIN_T8, INPUT_PULLUP);

    pinMode(PIN_CLEAR, INPUT);
    pinMode(PIN_BACK, INPUT);
}

bool FunkSteuerung::update()
{
    readTasterDebouncedAndBuildMask();
    readSonderTasterDebouncedAndBuildMask();
    bool result = false;
    const uint32_t currentTime = millis();

    if (currentTime - lastSendTime >= SEND_INTERVAL_MS)
    {
        lastSendTime = currentTime;

        funkeAuswerten();
        result = sendePaket();
    }
    return result;
}

void FunkSteuerung::funkeAuswerten()
{
    data.hl_ud = static_cast<uint16_t>(analogRead(PIN_HL_UD));
    data.hl_lr = static_cast<uint16_t>(analogRead(PIN_HL_LR));
    data.hr_ud = static_cast<uint16_t>(analogRead(PIN_HR_UD));
    data.hr_lr = static_cast<uint16_t>(analogRead(PIN_HR_LR));
    data.poti = static_cast<uint16_t>(analogRead(PIN_POTI));
    data.fader = static_cast<uint16_t>(analogRead(PIN_FADER));
    data.flap = static_cast<uint16_t>(analogRead(PIN_FLAP));

    data.schalter = buildButtonMask();
    data.taster = readTasterDebouncedAndBuildMask();
    data.sonderTaste = readSonderTasterDebouncedAndBuildMask();
}

uint8_t FunkSteuerung::calculateChecksum(const uint8_t *data, size_t length)
{
    uint8_t checksum = 0;

    for (size_t i = 0; i < length; i++)
    {
        checksum ^= data[i];
    }

    return checksum;
}

bool FunkSteuerung::sendePaket()
{
    uint8_t packet[20] = {0};
    packet[0] = 0xF1; // Startbyte
    packet[1] = 0x7E; // Startbyte

    writeU16(packet, 2, data.hl_ud);
    writeU16(packet, 4, data.hl_lr);
    writeU16(packet, 6, data.hr_ud);
    writeU16(packet, 8, data.hr_lr);
    writeU16(packet, 10, data.poti);
    writeU16(packet, 12, data.flap);
    writeU16(packet, 14, data.fader);

    packet[16] = data.schalter;
    packet[17] = data.taster;
    packet[18] = data.sonderTaste; // Sondertaste, falls benötigt

    packet[19] = calculateChecksum(packet, 19);

    port.write(packet, sizeof(packet));

    //! für dwbug hillfen ########################################
    if (digitalRead(PIN_T_ENCODER))
    {

        Serial.print("Sende Paket: ");
        for (size_t i = 0; i < sizeof(packet); i++)
        {
            Serial.print(packet[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    return true;
}

bool FunkSteuerung::isChecksumValid(const uint8_t *data, size_t length)
{
    if (length < 1)
    {
        return false; // Not enough data to validate checksum
    }

    uint8_t receivedChecksum = data[length - 1];
    uint8_t calculatedChecksum = calculateChecksum(data, length - 1);

    return receivedChecksum == calculatedChecksum;
}

uint8_t FunkSteuerung::readTasterDebouncedAndBuildMask()
{
    uint8_t mask = 0;
    unsigned long now = millis();

    for (uint8_t i = 0; i < TASTER_COUNT; i++)
    {
        bool raw = digitalRead(tasterPins[i]);

        if (raw != tasterLastRaw[i])
        {
            tasterLastRaw[i] = raw;
            tasterLastChange[i] = now;
        }

        if ((now - tasterLastChange[i]) >= DEBOUNCE_TIME_MS)
        {
            tasterStable[i] = raw;
        }

        bool pressed = (tasterStable[i] == LOW); // wegen INPUT_PULLUP

        bitWrite(mask, i, pressed);
    }
    maskData.tasterMask = mask;
    ///! für dwbug hillfen
    // Serial.print("Taster mask: ");
    // for (uint8_t i = 0; i < TASTER_COUNT; i++)
    // {
    //     Serial.print(bitRead(mask, i));
    // }
    // Serial.println();

    return mask;
}
uint8_t FunkSteuerung::readSonderTasterDebouncedAndBuildMask()
{
    uint8_t mask = 0;
    unsigned long now = millis();

    for (uint8_t i = 0; i < SONDERTASTER_COUNT; i++)
    {
        bool raw = digitalRead(sonderTasterPins[i]);

        if (raw != sonderTasterLastRaw[i])
        {
            sonderTasterLastRaw[i] = raw;
            sonderTasterLastChange[i] = now;
        }

        if ((now - sonderTasterLastChange[i]) >= DEBOUNCE_TIME_MS)
        {
            sonderTasterStable[i] = raw;
        }

        bool pressed = (sonderTasterStable[i] == LOW); // wegen INPUT_PULLUP

        bitWrite(mask, i, pressed);
    }

    ///! für dwbug hillfen
    // Serial.print("Taster mask: ");
    // for (uint8_t i = 0; i < TASTER_COUNT; i++)
    // {
    //     Serial.print(bitRead(mask, i));
    // }
    // Serial.println();

    maskData.sonderTasterMask = mask;
    return mask;
}

uint8_t FunkSteuerung::buildButtonMask()
{
    uint8_t mask = 0;

    bitWrite(mask, 0, digitalRead(PIN_HR1));
    bitWrite(mask, 1, digitalRead(PIN_HR2));
    bitWrite(mask, 2, digitalRead(PIN_HR3));
    bitWrite(mask, 3, digitalRead(PIN_HL1));
    bitWrite(mask, 4, digitalRead(PIN_HL2));
    bitWrite(mask, 5, digitalRead(PIN_HL3));
    bitWrite(mask, 6, 0x00);
    bitWrite(mask, 7, 0x00);

    ///! für dwbug hillfen
    // Serial.println("Schalter mask: ");
    // for (uint8_t i = 0; i < 8; i++)
    // {
    //     Serial.print(digitalRead(PIN_HR1 + i));
    // }
    // Serial.println();
    // for (uint8_t i = 0; i < 8; i++)
    // {
    //     Serial.print(bitRead(mask, i));
    // }
    // Serial.print("\t ");
    // Serial.println(mask, HEX);
    maskData.schalterMask = mask;
    return mask;
}
