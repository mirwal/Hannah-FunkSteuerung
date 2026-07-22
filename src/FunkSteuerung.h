// FunkSteuerung.h
#pragma once
#include <Arduino.h>

// 8bit mask für schalter, taster und sonderTaster

struct MaskData
{
    uint8_t schalterMask = 0;
    uint8_t tasterMask = 0;
    uint8_t sonderTasterMask = 0;
    bool changed = false;
    bool operator!=(const MaskData &other) const
    {
        return schalterMask != other.schalterMask ||
               tasterMask != other.tasterMask ||
               sonderTasterMask != other.sonderTasterMask;
    }
};

struct FunkSteuerungData
{
    // Analogwerte 0–1023
    uint16_t hl_ud = 0;
    uint16_t hl_lr = 0;
    uint16_t hr_ud = 0;
    uint16_t hr_lr = 0;
    uint16_t poti = 0;
    uint16_t fader = 0;

    // Schalter / Sonderfunktionen
    uint8_t flap = 0;

    // Kompakte Bitmaske für Übertragung
    uint8_t schalter = 0;
    uint8_t taster = 0;
    uint8_t sonderTaste = 0;

    // Einzelne Tasten, gut lesbar im Code
    bool hr1 = false;
    bool hr2 = false;
    bool hr3 = false;

    bool hl1 = false;
    bool hl2 = false;
    bool hl3 = false;

    bool trainer = false;

    bool taster1 = false;
    bool taster2 = false;
    bool taster3 = false;
    bool taster4 = false;
    bool taster5 = false;
    bool taster6 = false;
    bool taster7 = false;
    bool taster8 = false;
};

class FunkSteuerung
{
public:
    FunkSteuerung(HardwareSerial &serial);
    void begin(uint32_t baudrate);
    bool update();
    void getSchalterMask(uint8_t &mask) { mask = maskData.schalterMask; }
    void getTasterMask(uint8_t &mask) { mask = maskData.tasterMask; }

    //! test
    void getSonderTasterMask(uint8_t &mask) { mask = maskData.sonderTasterMask; }
    void getFunkSteuerungData(FunkSteuerungData &outData) { outData = data; }
    void getMaskData(MaskData &outMaskData) { outMaskData = maskData; }

private:
    HardwareSerial &port;
    FunkSteuerungData data;
    MaskData maskData;

    static constexpr uint32_t SEND_INTERVAL_MS = 20;
    uint32_t lastSendTime = 0;

    void funkeAuswerten();
    bool sendePaket();
    void writeU16(uint8_t *buffer, uint8_t index, uint16_t value)
    {
        buffer[index] = value & 0xFF;            // Low-Byte
        buffer[index + 1] = (value >> 8) & 0xFF; // High-Byte
    }

    uint16_t hl_UD = 0;
    uint16_t hr_UD = 2;
    uint16_t hl_LR = 1;
    uint16_t hr_LR = 3;
    uint16_t poti = 4;
    uint16_t fader = 5;
    uint16_t flap = 6;
    uint8_t batterie = 15;

    uint8_t hr1 = 0;
    uint8_t hr2 = 0;
    uint8_t hr3 = 0;

    uint8_t hl1 = 0;
    uint8_t hl2 = 0;
    uint8_t hl3 = 0;

    uint8_t schalter = 0;
    uint8_t taster = 0;

    static constexpr uint8_t PIN_HL_UD = 0;
    static constexpr uint8_t PIN_HL_LR = 1;
    static constexpr uint8_t PIN_HR_UD = 2;
    static constexpr uint8_t PIN_HR_LR = 3;
    static constexpr uint8_t PIN_POTI = 4;
    static constexpr uint8_t PIN_FADER = 5;
    static constexpr uint8_t PIN_FLAP = 6;

    static constexpr uint8_t PIN_HR1 = 22;       // 22
    static constexpr uint8_t PIN_HR2 = 24;       // 24
    static constexpr uint8_t PIN_HR3 = 27;       // 27
    static constexpr uint8_t PIN_HL1 = 25;       // 25
    static constexpr uint8_t PIN_HL2 = 28;       // 28
    static constexpr uint8_t PIN_HL3 = 29;       // 29
    static constexpr uint8_t PIN_TRAINER = 23;   // 23
    static constexpr uint8_t PIN_T_ENCODER = 37; // 37

    static constexpr uint8_t PIN_T1 = 40;
    static constexpr uint8_t PIN_T2 = 41;
    static constexpr uint8_t PIN_T3 = 42;
    static constexpr uint8_t PIN_T4 = 43;
    static constexpr uint8_t PIN_T5 = 44;
    static constexpr uint8_t PIN_T6 = 45;
    static constexpr uint8_t PIN_T7 = 46;
    static constexpr uint8_t PIN_T8 = 47;

    static constexpr uint8_t PIN_CLEAR = 48;
    static constexpr uint8_t PIN_BACK = 49;

    static constexpr uint16_t DEBOUNCE_TIME_MS = 50;

    static constexpr uint8_t TASTER_COUNT = 8;
    const uint8_t tasterPins[TASTER_COUNT] = {
        PIN_T1, PIN_T2, PIN_T3, PIN_T4,
        PIN_T5, PIN_T6, PIN_T7, PIN_T8};

    bool tasterStable[TASTER_COUNT];
    bool tasterLastRaw[TASTER_COUNT];
    uint32_t tasterLastChange[TASTER_COUNT];

    static constexpr uint8_t SONDERTASTER_COUNT = 4;
    const uint8_t sonderTasterPins[SONDERTASTER_COUNT] = {
        PIN_TRAINER, PIN_CLEAR, PIN_BACK, PIN_T_ENCODER};

    bool sonderTasterStable[SONDERTASTER_COUNT];
    bool sonderTasterLastRaw[SONDERTASTER_COUNT];
    uint32_t sonderTasterLastChange[SONDERTASTER_COUNT];

    uint8_t calculateChecksum(const uint8_t *data, size_t length);
    bool isChecksumValid(const uint8_t *data, size_t length);
    uint8_t readTasterDebouncedAndBuildMask();
    uint8_t readSonderTasterDebouncedAndBuildMask();
    uint8_t buildButtonMask();
};