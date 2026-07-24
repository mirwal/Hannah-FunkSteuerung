// FunkSteuerung.h
#pragma once
#include <Arduino.h>
#include <EEPROM.h>

struct AxisCalibration
{
    uint16_t minValue;
    uint16_t center;
    uint16_t maxValue;

    AxisCalibration(uint16_t minValue = 512, uint16_t centerValue = 512, uint16_t maxValue = 512)
        : minValue(minValue), center(centerValue), maxValue(maxValue)
    {
    }
};

struct JoystickCalibration
{
    uint16_t magicNumber;

    AxisCalibration hlUd;
    AxisCalibration hlLr;
    AxisCalibration hrUd;
    AxisCalibration hrLr;
};

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
    uint16_t flap = 0;

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

    uint32_t lastPaketSentTime = 0;
    uint32_t sentPaketCount = 0;
    uint16_t paketePerSecond = 0;

    uint32_t paketInterval = 0;
    uint32_t maxPaketInterval = 0;
};

class FunkSteuerung
{
public:
    FunkSteuerung(HardwareSerial &serial);

    void begin(uint32_t baudrate);
    bool update();

    void loadCalibration();
    void saveCalibration();

    void getSchalterMask(uint8_t &mask) { mask = maskData.schalterMask; }
    void getTasterMask(uint8_t &mask) { mask = maskData.tasterMask; }
    const FunkSteuerungData &getData() const { return data; }
    void resetMaxPacketInterval() { maxPaketInterval = paketInterval; }
    //! test
    // void getSonderTasterMask(uint8_t &mask) { mask = maskData.sonderTasterMask; }
    uint16_t readAverage(uint8_t pin) const;
    void getMaskData(MaskData &outMaskData) { outMaskData = maskData; }

    void calibrateJoystickCenters();
    void updateMinMax(AxisCalibration &calibration, uint16_t raw)
    {
        calibration.minValue = min(calibration.minValue, raw);
        calibration.maxValue = max(calibration.maxValue, raw);
    }
    const AxisCalibration &getPendingHlUd() const { return pendingHlUd; }
    const AxisCalibration &getPendingHlLr() const { return pendingHlLr; }
    const AxisCalibration &getPendingHrUd() const { return pendingHrUd; }
    const AxisCalibration &getPendingHrLr() const { return pendingHrLr; }

    void beginMinMaxCalibration()
    {

        const uint16_t hlUd = analogRead(PIN_HL_UD);
        const uint16_t hlLr = 1023 - analogRead(PIN_HL_LR);
        const uint16_t hrUd = analogRead(PIN_HR_UD);
        const uint16_t hrLr = analogRead(PIN_HR_LR);

        pendingHlUd = {hlUd, calibrationHlUd.center, hlUd};
        pendingHlLr = {hlLr, calibrationHlLr.center, hlLr};
        pendingHrUd = {hrUd, calibrationHrUd.center, hrUd};
        pendingHrLr = {hrLr, calibrationHrLr.center, hrLr};
    }
    // Removed duplicate calibrateJoystickMinMax function
    void updateMinMaxCalibration()
    {
        updateMinMax(pendingHlUd, analogRead(PIN_HL_UD));
        updateMinMax(pendingHlLr, 1023 - analogRead(PIN_HL_LR));
        updateMinMax(pendingHrUd, analogRead(PIN_HR_UD));
        updateMinMax(pendingHrLr, analogRead(PIN_HR_LR));
    }
    void saveMinMaxCalibration()
    {
        calibrationHlUd.minValue = pendingHlUd.minValue;
        calibrationHlUd.maxValue = pendingHlUd.maxValue;

        calibrationHlLr.minValue = pendingHlLr.minValue;
        calibrationHlLr.maxValue = pendingHlLr.maxValue;

        calibrationHrUd.minValue = pendingHrUd.minValue;
        calibrationHrUd.maxValue = pendingHrUd.maxValue;

        calibrationHrLr.minValue = pendingHrLr.minValue;
        calibrationHrLr.maxValue = pendingHrLr.maxValue;
        saveCalibration();
    }

private:
    HardwareSerial &port;
    FunkSteuerungData data;
    MaskData maskData;
    static constexpr int EEPROM_CALIBRATION_ADDRESS = 0;
    static constexpr uint16_t CALIBRATION_MAGIC_NUMBER = 0xCA71;
    static constexpr uint32_t SEND_INTERVAL_MS = 20;

    uint32_t lastSendTime = 0;

    uint32_t lastMeasureTime = 0;

    uint32_t sentPaketCount = 0;
    uint32_t lastPaketCount = 0;
    uint32_t lastRateMeasureTime = 0;
    uint16_t paketePerSecond = 0;

    uint32_t lastPaketTime = 0;
    uint32_t paketInterval = 0;
    uint32_t maxPaketInterval = 0;

    void updatePaketRate();

    void funkeAuswerten();
    bool sendePaket();
    void writeU16(uint8_t *buffer, uint8_t index, uint16_t value)
    {
        buffer[index] = value & 0xFF;            // Low-Byte
        buffer[index + 1] = (value >> 8) & 0xFF; // High-Byte
    }

    // Joystick Calibration
    AxisCalibration calibrationHlUd;
    AxisCalibration calibrationHlLr;
    AxisCalibration calibrationHrUd;
    AxisCalibration calibrationHrLr;

    bool isCalibrationValid(const AxisCalibration &calibration) const;

    AxisCalibration pendingHlUd;
    AxisCalibration pendingHlLr;
    AxisCalibration pendingHrUd;
    AxisCalibration pendingHrLr;

    uint16_t normalizeAxisToU16(
        uint16_t raw,
        const AxisCalibration &calibration) const;
    uint16_t calibrateAxis(uint16_t raw, const AxisCalibration &calibration) const;

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