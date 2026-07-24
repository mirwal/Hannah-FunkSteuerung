// FunkSteuerung.cpp
#include "FunkSteuerung.h"

FunkSteuerung::FunkSteuerung(HardwareSerial &serial)
    : port(serial)
{
}

void FunkSteuerung::begin(uint32_t baudrate)
{
    port.begin(baudrate);
    loadCalibration();
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
        funkeAuswerten();
        updatePaketRate();
        result = sendePaket();
        data.lastPaketSentTime = currentTime - lastSendTime;
        data.paketePerSecond = paketePerSecond;
        lastSendTime = currentTime;
    }
    return result;
}
void FunkSteuerung::saveCalibration()
{
    JoystickCalibration storedCalibration;

    storedCalibration.magicNumber = CALIBRATION_MAGIC_NUMBER;

    storedCalibration.hlUd = calibrationHlUd;
    storedCalibration.hlLr = calibrationHlLr;
    storedCalibration.hrUd = calibrationHrUd;
    storedCalibration.hrLr = calibrationHrLr;

    EEPROM.put(EEPROM_CALIBRATION_ADDRESS, storedCalibration);

    Serial.println("Joystick-Kalibrierung im EEPROM gespeichert");
}
bool FunkSteuerung::isCalibrationValid(
    const AxisCalibration &calibration) const
{
    return calibration.minValue < calibration.center &&
           calibration.center < calibration.maxValue &&
           calibration.maxValue <= 1023;
}

void FunkSteuerung::loadCalibration()
{
    JoystickCalibration storedCalibration;

    EEPROM.get(
        EEPROM_CALIBRATION_ADDRESS,
        storedCalibration);

    const bool valid =
        storedCalibration.magicNumber == CALIBRATION_MAGIC_NUMBER &&
        isCalibrationValid(storedCalibration.hlUd) &&
        isCalibrationValid(storedCalibration.hlLr) &&
        isCalibrationValid(storedCalibration.hrUd) &&
        isCalibrationValid(storedCalibration.hrLr);

    if (!valid)
    {
        Serial.println(
            "Keine gueltige Kalibrierung im EEPROM gefunden");

        return;
    }

    calibrationHlUd = storedCalibration.hlUd;
    calibrationHlLr = storedCalibration.hlLr;
    calibrationHrUd = storedCalibration.hrUd;
    calibrationHrLr = storedCalibration.hrLr;

    Serial.println(
        "Joystick-Kalibrierung aus EEPROM geladen");
}

uint16_t FunkSteuerung::readAverage(uint8_t pin) const
{
    constexpr uint8_t SAMPLE_COUNT = 32;
    uint32_t sum = 0;

    for (uint8_t i = 0; i < SAMPLE_COUNT; ++i)
    {
        sum += analogRead(pin);
    }

    return static_cast<uint16_t>(sum / SAMPLE_COUNT);
}

void FunkSteuerung::calibrateJoystickCenters()
{
    calibrationHlUd.center = readAverage(PIN_HL_UD);
    calibrationHlLr.center = 1023 - readAverage(PIN_HL_LR);
    calibrationHrUd.center = readAverage(PIN_HR_UD);
    calibrationHrLr.center = readAverage(PIN_HR_LR);
    saveCalibration();
}

void FunkSteuerung::updatePaketRate()
{
    const uint32_t currentTime = millis();
    const uint32_t elapsedTime = currentTime - lastRateMeasureTime;

    if (elapsedTime >= 1000)
    {
        const uint32_t sentPakets =
            sentPaketCount - lastPaketCount;

        paketePerSecond =
            static_cast<uint16_t>((sentPakets * 1000UL) / elapsedTime);

        lastPaketCount = sentPaketCount;
        lastRateMeasureTime = currentTime;
    }
}

void FunkSteuerung::funkeAuswerten()
{

    // data.hl_ud = static_cast<uint16_t>(analogRead(PIN_HL_UD));
    // data.hl_lr = static_cast<uint16_t>(analogRead(PIN_HL_LR));
    // data.hr_ud = static_cast<uint16_t>(analogRead(PIN_HR_UD));
    // data.hr_lr = static_cast<uint16_t>(analogRead(PIN_HR_LR));

    const uint16_t rawHlUd = analogRead(PIN_HL_UD);
    const uint16_t rawHlLr = 1023 - analogRead(PIN_HL_LR);
    const uint16_t rawHrUd = analogRead(PIN_HR_UD);
    const uint16_t rawHrLr = analogRead(PIN_HR_LR);

    data.hl_ud = calibrateAxis(rawHlUd, calibrationHlUd);
    data.hl_lr = calibrateAxis(rawHlLr, calibrationHlLr);
    data.hr_ud = calibrateAxis(rawHrUd, calibrationHrUd);
    data.hr_lr = calibrateAxis(rawHrLr, calibrationHrLr);

    data.poti = static_cast<uint16_t>(analogRead(PIN_POTI));
    data.fader = static_cast<uint16_t>(analogRead(PIN_FADER));
    data.flap = static_cast<uint16_t>(analogRead(PIN_FLAP));

    data.schalter = buildButtonMask();
    data.taster = readTasterDebouncedAndBuildMask();
    data.sonderTaste = readSonderTasterDebouncedAndBuildMask();

    uint8_t mask = data.schalter;
    data.hr1 = bitRead(mask, 0);
    data.hr2 = bitRead(mask, 1);
    data.hr3 = bitRead(mask, 2);
    data.hl1 = bitRead(mask, 3);
    data.hl2 = bitRead(mask, 4);
    data.hl3 = bitRead(mask, 5);

    data.trainer = !bitRead(data.sonderTaste, 0);
    data.sentPaketCount = sentPaketCount;
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
    uint8_t Paket[20] = {0};
    Paket[0] = 0xF1; // Startbyte
    Paket[1] = 0x7E; // Startbyte

    writeU16(Paket, 2, data.hl_ud);
    writeU16(Paket, 4, data.hl_lr);
    writeU16(Paket, 6, data.hr_ud);
    writeU16(Paket, 8, data.hr_lr);
    writeU16(Paket, 10, data.poti);
    writeU16(Paket, 12, data.flap);
    writeU16(Paket, 14, data.fader);

    Paket[16] = data.schalter;
    Paket[17] = data.taster;
    Paket[18] = data.sonderTaste; // Sondertaste, falls benötigt

    Paket[19] = calculateChecksum(Paket, 19);

    const uint32_t currentTime = micros();
    if (lastPaketTime != 0)
    {
        paketInterval = currentTime - lastPaketTime;

        if (paketInterval > maxPaketInterval)
        {
            maxPaketInterval = paketInterval;
        }
    }

    lastPaketTime = currentTime;

    port.write(Paket, sizeof(Paket));

    sentPaketCount++;
    data.paketInterval = paketInterval;
    data.maxPaketInterval = maxPaketInterval;
    data.sentPaketCount = sentPaketCount;

    //! für dwbug hillfen ########################################
    // if (digitalRead(PIN_T_ENCODER))
    // {

    //     Serial.print("Sende Paket: ");
    //     for (size_t i = 0; i < sizeof(Paket); i++)
    //     {
    //         Serial.print(Paket[i], HEX);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    return true;
}

uint16_t FunkSteuerung::normalizeAxisToU16(uint16_t raw, const AxisCalibration &calibration) const
{
    if (raw <= calibration.center)
    {
        return static_cast<uint16_t>(
            map(raw,
                calibration.minValue,
                calibration.center,
                0,
                32768));
    }

    return static_cast<uint16_t>(
        map(raw,
            calibration.center,
            calibration.maxValue,
            32768,
            65535));
}
uint16_t FunkSteuerung::calibrateAxis(uint16_t raw, const AxisCalibration &calibration) const
{
    int32_t result;

    if (raw <= calibration.center)
    {
        result = map(
            raw,
            calibration.minValue,
            calibration.center,
            0,
            512);
    }
    else
    {
        result = map(
            raw,
            calibration.center,
            calibration.maxValue,
            512,
            1024);
    }

    return static_cast<uint16_t>(
        constrain(result, 0L, 1023L));
}

bool FunkSteuerung::isChecksumValid(const uint8_t *data, size_t length)
{
    //
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
