// SenderDisplay.cpp
#include "SenderDisplay.h"
SenderDisplay::SenderDisplay()
    : display(U8G2_R0, OLED_CS, OLED_DC, OLED_RST)
{
}

void SenderDisplay::begin()
{
    display.begin();
    display.clearBuffer();
    display.setContrast(5); // Set contrast based on dimDisplay parameter
    display.setFont(Font_Battery);
    display.setFontPosTop();
    display.enableUTF8Print(); // Disable UTF-8 support for special characters
    display.setCursor(120, 1);
    display.print(0x04); // Print the battery symbol
    display.setFontDirection(0);
    display.setFont(Font_8x8);
    display.setCursor(0, 0);
    display.print("Funksteuerung");
    display.setCursor(0, 18);
    display.setFont(Font_6x12);
    display.print("für hannah hexapod");
    display.setCursor(0, 38);
    SymbolIchon SymbolIchon;
    uint8_t i = 0;
    uint8_t j = 18;

    display.setFont(u8g2_font_open_iconic_embedded_2x_t);
    display.drawUTF8(i += 1, 38, SymbolIchon.alarm);         // Print the alarm symbol
    display.drawUTF8(i += j, 38, SymbolIchon.einstallungen); // Print the settings symbol
    display.drawUTF8(i += j, 38, SymbolIchon.achtung);       // Print the warning symbol
    display.drawUTF8(i += j, 38, SymbolIchon.offline);       // Print the offline symbol
    display.drawUTF8(i += j, 38, SymbolIchon.blitz);         // Print the battery symbol
    display.drawUTF8(i += j, 38, SymbolIchon.zuruck);        // Print the back symbol
    display.drawUTF8(i += j, 38, SymbolIchon.online);        // Print the online symbol
    display.sendBuffer();
}
void SenderDisplay::clear()
{
    display.clearBuffer();
}
void SenderDisplay::show()
{
    display.sendBuffer();
}
uint8_t SenderDisplay::BatteryZeichenHolen(const BatteryData &batteryData)
{
    /// Von 6,6V bis 8,4V wird der Bereich in 6 Stufen aufgeteilt
    uint8_t batteryRange = map(batteryData.voltage * 10, 66, 84, 0, 6); // Map voltage to battery range (0-6)

    switch (batteryRange)
    {
    case 0:
        return (0x00); // Battery empty symbol
    case 1:
        return (0x01); // Battery 1/4 symbol
    case 2:
        return (0x02); // Battery 2/4 symbol
    case 3:
        return (0x03); // Battery 3/4 symbol
    case 4:
        return (0x04); // Battery full symbol
    case 5:
        return (0x05); // Battery full symbol
    default:
        return (0x06); // Default to battery empty symbol
    }
}

void SenderDisplay::drawHome(const FunkSteuerungData &data, const BatteryData &batteryData)
{
    clear();
    drawJoystickPositions(data);
    uint8_t center = 128 / 2;
    display.setCursor(center - 4, 15); // Adjust the position based on the length of the text
    display.setFont(Font_Battery);
    display.print(BatteryZeichenHolen(batteryData)); // Print the battery symbol based on voltage
    display.setFont(Font_6x12);
    show();
}

void SenderDisplay::drawJoystickPositions(const FunkSteuerungData &data)
{

    int16_t range = 15;

    int16_t hl_lr_mapped = map(data.hl_lr, 0, 1023, 2 - range, 2 + range);
    int16_t hl_ud_mapped = map(data.hl_ud, 0, 1023, 2 + range, 2 - range);

    int16_t hr_lr_mapped = map(data.hr_lr, 0, 1023, 2 - range, 2 + range);
    int16_t hr_ud_mapped = map(data.hr_ud, 0, 1023, 2 + range, 2 - range);

    int16_t rechts_x = (128 / 6); // Center of the right frame
    int16_t rechts_y = 22;        // Center of the right frame

    int16_t links_x = 128 - (128 / 6); // Center of the left frame
    int16_t links_y = 22;              // Center of the left  frame

    int16_t frameWidth = 39;

    display.drawRFrame((links_x - frameWidth / 2), (links_y - frameWidth / 2), frameWidth, frameWidth, 1);

    display.drawRFrame((rechts_x - frameWidth / 2), (rechts_y - frameWidth / 2), frameWidth, frameWidth, 1);
    int16_t LineLength = 8;

    display.drawLine(rechts_x - LineLength / 2, rechts_y, rechts_x + LineLength / 2, rechts_y);
    display.drawLine(rechts_x, rechts_y - LineLength / 2, rechts_x, rechts_y + LineLength / 2);
    display.drawLine(links_x - LineLength / 2, links_y, links_x + LineLength / 2, links_y);
    display.drawLine(links_x, links_y - LineLength / 2, links_x, links_y + LineLength / 2);

    int16_t circleRadius = 2;
    display.drawCircle(links_x + hr_lr_mapped - circleRadius, links_y + hr_ud_mapped - circleRadius, circleRadius);
    display.drawCircle(rechts_x + hl_lr_mapped - circleRadius, rechts_y + hl_ud_mapped - circleRadius, circleRadius);
}
void SenderDisplay::drawTitle(const char *title)
{
    display.setFont(Font_Title);
    // display.enableUTF8Print(); // Enable UTF-8 support for special characters
    display.setCursor(5, 0);
    display.print(title);
    display.drawHLine(0, 12, 128);
}
void SenderDisplay::drawMenuEntry(uint8_t row, const char *text, bool selected)
{
    display.setFont(Font_Entry);
    display.setCursor(5, 18 + (row * 10));
    display.print(selected ? SELECTED_ICON : "  ");
    display.print(text);
}
void SenderDisplay::drawMainMenu(uint8_t selectedEntry)
{
    clear();
    drawTitle("Hauptmenü");
    drawMenuEntry(0, "Diagnose", selectedEntry == 0);
    drawMenuEntry(1, "Kalibrierung", selectedEntry == 1);
    drawMenuEntry(2, "Batterie", selectedEntry == 2);
    drawMenuEntry(3, "Information", selectedEntry == 3);
    drawIconEntry(0, selectedEntry == 0);
    drawIconEntry(1, selectedEntry == 1);
    drawIconEntry(2, selectedEntry == 2);
    drawIconEntry(3, selectedEntry == 3);
    show();
}
void SenderDisplay::drawIconEntry(uint8_t row, bool selected)
{
    if (!selected)
        return; // Only draw icons for selected entries
    display.setFont(u8g2_font_open_iconic_embedded_2x_t);
    display.setCursor(90, 18 + (row * 10));

    switch (row)
    {
    case 0:
        display.drawUTF8(90, 18 + (row * 10), SymbolIchon().blitz); // Print the battery symbol
        break;
    case 1:
        display.drawUTF8(90, 18 + (row * 9), SymbolIchon().einstallungen); // Print the settings symbol
        break;
    case 2:
        display.drawUTF8(90, 18 + (row * 8), SymbolIchon().batteryVoll); // Print the battery full symbol
        break;
    case 3:
        display.drawUTF8(90, 18 + (row * 7), SymbolIchon().achtung); // Print the warning symbol
        break;
    default:
        break;
    }
}

void SenderDisplay::drawDiagnosticsMenu(uint8_t selectedEntry)
{
    clear();
    drawTitle("Diagnose");
    drawMenuEntry(0, "Analogwerte", selectedEntry == 0);
    drawMenuEntry(1, "Tasterzustände", selectedEntry == 1);
    drawMenuEntry(2, "Funkstatus", selectedEntry == 2);
    drawMenuEntry(3, "Zeitmessung", selectedEntry == 3);
    show();
}

void SenderDisplay::drawAnalogValues(const FunkSteuerungData &data)
{
    clear();
    drawTitle("Analogwerte");

    display.setFont(Font_Entry);

    // Linke Spalte
    display.setCursor(2, 18);
    display.print("HL UD:");
    display.print(data.hl_ud);

    display.setCursor(2, 29);
    display.print("HL LR:");
    display.print(data.hl_lr);

    display.setCursor(2, 40);
    display.print("Poti:");
    display.print(data.poti);

    // Rechte Spalte
    display.setCursor(66, 18);
    display.print("HR UD:");
    display.print(data.hr_ud);

    display.setCursor(66, 29);
    display.print("HR LR:");
    display.print(data.hr_lr);

    display.setCursor(66, 40);
    display.print("Fader:");
    display.print(data.fader);

    display.setCursor(2, 51);
    display.print("Flap:");
    display.print(data.flap);

    show();
}
void SenderDisplay::drawRadioValues(const FunkSteuerungData &data)
{

    clear();
    drawTitle("Funkstatus");

    display.setFont(Font_Entry);

    // Linke Spalte
    uint8_t x_RightColumn = 66;
    uint8_t x_LeftColumn = 2;

    display.setCursor(x_LeftColumn, 18);
    display.print("Intervall");

    display.setCursor(x_RightColumn, 18);
    display.print(data.lastPaketSentTime);
    display.print(" ms");

    display.setCursor(x_LeftColumn, 29);
    display.print("Pakete/s");

    display.setCursor(x_RightColumn, 29);
    display.print(data.paketePerSecond);

    display.setCursor(x_LeftColumn, 40);
    display.print("Pakete");

    if (data.sentPaketCount < 10000)
    {
        display.setCursor(x_RightColumn, 40);
        display.print(data.sentPaketCount);
    }
    else
    {
        display.setCursor(x_RightColumn, 40);
        display.print(data.sentPaketCount / 1000);
        display.print(" k");
    }
    display.setCursor(x_LeftColumn, 51);
    display.print("");
    display.setCursor(x_RightColumn, 51);

    show();
}

void SenderDisplay::drawTimingValues(const FunkSteuerungData &data, const TimingData &timingData)
{
    clear();
    drawTitle("Zeitmessung");

    display.setFont(Font_Entry);

    // Linke Spalte
    uint8_t x_RightColumn = 66;
    uint8_t x_LeftColumn = 2;

    display.setCursor(x_LeftColumn, 18);
    display.print("Intervall");

    display.setCursor(x_RightColumn, 18);
    display.print(data.paketInterval);
    display.print(" µs");

    display.setCursor(x_LeftColumn, 29);
    display.print("Max");

    display.setCursor(x_RightColumn, 29);
    display.print(data.maxPaketInterval);
    display.print(" µs");

    display.setCursor(x_LeftColumn, 40);
    display.print("Loop");
    display.setCursor(x_RightColumn, 40);
    display.print(timingData.loopInterval);
    display.print(" µs");

    display.setCursor(x_LeftColumn, 51);
    display.print("Loop Max");
    display.setCursor(x_RightColumn, 51);
    display.print(timingData.maxLoopInterval);
    display.print(" µs");

    show();
}

void SenderDisplay::drawButtonStates(const FunkSteuerungData &data)
{
    clear();
    drawTitle("Tasterzustände");

    display.setFont(Font_Entry);

    // Linke Spalte
    display.setCursor(2, 18);
    display.print("HL1:");
    display.print(data.hl1 ? "ON" : "OFF");

    display.setCursor(2, 29);
    display.print("HL2:");
    display.print(data.hl2 ? "ON" : "OFF");

    display.setCursor(2, 40);
    display.print("HL3:");
    display.print(data.hl3 ? "ON" : "OFF");

    // Rechte Spalte
    display.setCursor(66, 18);
    display.print("HR1:");
    display.print(data.hr1 ? "ON" : "OFF");

    display.setCursor(66, 29);
    display.print("HR2:");
    display.print(data.hr2 ? "ON" : "OFF");

    display.setCursor(66, 40);
    display.print("HR3:");
    display.print(data.hr3 ? "ON" : "OFF");

    // Untere Reihe
    display.setCursor(2, 51);
    display.print("Trainer:");
    display.print(data.trainer ? "ON" : "OFF");

    display.setCursor(66, 51);
    display.print("Flap:");
    if (data.flap <= 400)
        display.print("0");
    else if (data.flap > 800)
        display.print("2");
    else
        display.print("1");
    show();
}
void SenderDisplay::drawCalibrationMenu(uint8_t selectedEntry)
{
    clear();
    drawTitle("Kalibrierung");
    drawMenuEntry(0, "Kalibrierung Center", selectedEntry == 0);
    drawMenuEntry(1, "Kalibrierung Min/Max", selectedEntry == 1);
    show();
}

void SenderDisplay::drawCalibrationCenter(const FunkSteuerungData &data)
{
    clear();
    drawTitle("Kalibrierung Center");
    display.setFont(Font_Entry);
    display.setCursor(2, 18);
    display.print("Joysticks loslassen");

    display.setCursor(2, 29);
    display.print("HL   ");
    display.print(data.hl_lr);
    display.print(" / ");
    display.print(data.hl_ud);

    display.setCursor(2, 40);
    display.print("HR   ");
    display.print(data.hr_lr);
    display.print(" / ");
    display.print(data.hr_ud);
    display.setCursor(2, 51);
    display.print("Clear = übernehmen");

    show();
}

void SenderDisplay::drawCalibrationMinMax(const FunkSteuerungData &data)
{
    clear();
    drawTitle("Kalibrierung Min/Max");
    show();
}

void SenderDisplay::drawBattery(const BatteryData &batteryData)
{
    clear();
    drawTitle("Batterie");
    display.setFont(Font_Entry);
    display.setCursor(2, 25);
    display.print("Spannung: ");
    display.print(batteryData.voltage, 2);
    display.print(" V");
    display.setCursor(2, 35);
    display.print("Warnung:  ");

    if (batteryData.voltage != 0)
    {
        if (batteryData.isLow)
            display.print("zu niedrig");
        else if (batteryData.voltage > 8.4)
            display.print("zu hoch");
        else
            display.print("alles OK");
    }
    else
    {
        display.print("USB verbunden");
    }

    display.setCursor(100, 20);
    display.setFont(Font_Battery);
    display.print(BatteryZeichenHolen(batteryData)); // Print the battery symbol based on voltage
    display.setFont(Font_6x12);

    show();
}
void SenderDisplay::drawAbout()
{
    static uint8_t wifi_search_32_32_7f_frame = 0;
    clear();
    display.setFont(Font_Entry);
    display.setCursor(2, 25);
    display.print("Funksteuerung");
    display.setCursor(2, 35);
    display.print("hannah hexapod");
    display.setCursor(2, 45);
    display.print("Version 1.0");
    display.setCursor(2, 55);
    display.print("Entwickler: mirwal");
    display.drawXBMP(19, 6, 90, 6, mirwal);
    display.setDrawColor(1);
    display.drawXBMP(90, 18, 32, 32, wifi_search_32_32_7f_frames[wifi_search_32_32_7f_frame]);
    wifi_search_32_32_7f_frame = millis() / 250 % 8; // Update the frame index based on time
    show();
}

void SenderDisplay::drawSaveConfirmation(const char *message)
{
    display.setDrawColor(0);
    display.drawBox(16, 11, 96, 43);
    display.setDrawColor(1);
    display.drawFrame(17, 12, 95, 42);
    display.setFont(Font_Title);
    display.setCursor(2, 29);
    display.drawStr(26, 28, (message));
    display.setFont(u8g2_font_open_iconic_embedded_2x_t);
    display.drawUTF8(90, 28, SymbolIchon().einstallungen); // Print the settings symbol
    show();
}
