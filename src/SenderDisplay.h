// SenderDisplay.h
#pragma once
#include "FunkSteuerung.h"
#include "TimingData.h"
#include <Arduino.h>
#include <U8g2lib.h>

// diese ichons und schriften sind in der u8g2 library enthalten und können direkt verwendet werden
// https://github.com/olikraus/u8g2/wiki/fntlist8?utm_source=chatgpt.com#u8g2-font-list
// oberfläche design https://lopaka.app/projects
#define Font_5x8 u8g2_font_5x8_mf
#define Font_8x8 u8g2_font_artossans8_8r
#define Font_6x12 u8g2_font_6x12_m_symbols
#define Font_Battery u8g2_font_battery19_tn        // Battery symbol font 0x00 bis 0x06
#define Font_VCR u8g2_font_VCR_OSD_mr              // VCR font
#define Font_Icons u8g2_font_open_iconic_arrow_4x4 // Icon font
#define Font_Title u8g2_font_6x10_tf
#define Font_Entry u8g2_font_5x8_mf
#define SELECTED_ICON "> "

struct SymbolIchon
{
    const char *alarm = "A";
    const char *einstallungen = "H";
    const char *achtung = "G";
    const char *blitz = "C";
    const char *offline = "D";
    const char *batteryVoll = "I";
    const char *batteryLeer = "@";
    const char *zuruck = "O";
    const char *online = "P";
};

class SenderDisplay
{

public:
    SenderDisplay();

    void begin();
    void clear();
    void show();

    void drawTitle(const char *title);

    void drawMainMenu(uint8_t selectedEntry);
    void drawMenuEntry(uint8_t row, const char *text, bool selected);

    void drawDiagnosticsMenu(uint8_t selectedEntry);
    void drawAnalogValues(const FunkSteuerungData &data);
    void drawButtonStates(const FunkSteuerungData &data);
    void drawRadioValues(const FunkSteuerungData &data);
    void drawTimingValues(const FunkSteuerungData &data, const TimingData &timingData);

    void drawCalibrationMenu(uint8_t selectedEntry);
    void drawCalibrationCenter(const FunkSteuerungData &data);
    void drawCalibrationMinMax(const FunkSteuerungData &data);

    void drawBattery(uint16_t millivolts);
    void drawSaveConfirmation(const char *message);

private:
    static constexpr uint8_t OLED_CS = 4;
    static constexpr uint8_t OLED_DC = 5;
    static constexpr uint8_t OLED_RST = 6;

    /////////////////////////////// 128x64 SSD1309 ///////////////////////////
    U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI display;
    /////////////////////////////////////////////////////////////////////////
};
