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
    show();
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

    show();
}

void SenderDisplay::drawBattery(uint16_t millivolts)
{
}
