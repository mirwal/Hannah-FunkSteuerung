#include "_Display.h"
#define Font_8x8 u8g2_font_artossans8_8r
#define Font_6x12 u8g2_font_6x12_m_symbols
#define Font_Battery u8g2_font_battery19_tn        // Battery symbol font 0x00 bis 0x06
#define Font_VCR u8g2_font_VCR_OSD_mr              // VCR font for "mirwal edition" text
#define Font_Icons u8g2_font_open_iconic_arrow_4x4 // Icon font for joystick and switch symbols
                                                   // Joystick symbol in the icon font

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

bool Display::begin()
{

    display.setBusClock(400000); // Set I2C bus clock to 400kHz
    display.begin();
    display.setPowerSave(0); // Wake up the display
    display.setContrast(0);  // Set contrast based on dimDisplay parameter
    display.clearBuffer();
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
    display.setFont(u8g2_font_open_iconic_embedded_2x_t);
    display.setCursor(0, 26);

    display.setCursor(0, 38);
    SymbolIchon SymbolIchon;
    uint8_t i = 0;
    uint8_t j = 18;

    display.drawUTF8(i += 1, 38, SymbolIchon.alarm);         // Print the alarm symbol
    display.drawUTF8(i += j, 38, SymbolIchon.einstallungen); // Print the settings symbol
    display.drawUTF8(i += j, 38, SymbolIchon.achtung);       // Print the warning symbol
    display.drawUTF8(i += j, 38, SymbolIchon.offline);       // Print the offline symbol
    display.drawUTF8(i += j, 38, SymbolIchon.blitz);         // Print the battery symbol
    display.drawUTF8(i += j, 38, SymbolIchon.zuruck);        // Print the back symbol
    display.drawUTF8(i += j, 38, SymbolIchon.online);        // Print the online symbol
    display.sendBuffer();
    return true;
}

void Display::update(int16_t x1, int16_t y1)
{
    zeichneJoystick(x1, y1);
};

void Display::zeichneJoystick(
    int16_t x,
    int16_t y)
{
    if (x <= 10 && x >= -10)
        x = 0;
    if (y <= 10 && y >= -10)
        y = 0;

    int16_t MITTE_X_offset = (SCREEN_WIDTH / 4) + 10;
    int16_t MITTE_Y_offset = -SCREEN_HEIGHT / 4;

    int16_t MITTE_X_ = SCREEN_WIDTH / 2 + MITTE_X_offset;
    int16_t MITTE_Y_ = SCREEN_HEIGHT / 2 + MITTE_Y_offset;

    constexpr int16_t BEWEGUNG_X_ = 20;
    constexpr int16_t BEWEGUNG_Y_ = 15;

    constexpr int16_t KREUZ_GROESSE = 4;
    constexpr int16_t PUNKT_RADIUS = 4;

    // Eingabewerte absichern
    x = constrain(x, -100, 100);
    y = constrain(y, -100, 100);

    int16_t verschiebung_X = map(
        x,
        -100,
        100,
        -BEWEGUNG_X_,
        BEWEGUNG_X_);

    int16_t verschiebung_Y = map(
        y,
        -100,
        100,
        BEWEGUNG_Y_,
        -BEWEGUNG_Y_);

    // Punkt: normale Bewegungsrichtung
    int16_t punkt_X_ = MITTE_X_ + verschiebung_X;
    int16_t punkt_Y_ = MITTE_Y_ + verschiebung_Y;

    // Kreuz: entgegengesetzte Bewegungsrichtung
    int16_t kreuz_X_ = MITTE_X_ - verschiebung_X / 2;
    int16_t kreuz_Y_ = MITTE_Y_ - verschiebung_Y / 2;
    display.setDrawColor(0);
    display.drawBox(
        MITTE_X_ - (BEWEGUNG_X_ + 5), MITTE_Y_ - (BEWEGUNG_Y_ + 5),
        (BEWEGUNG_X_ + 5) * 2, (BEWEGUNG_Y_ + 5) * 2);

    display.setDrawColor(1);
    display.drawBox(
        MITTE_X_ - (BEWEGUNG_X_ + 1),
        MITTE_Y_ - (BEWEGUNG_Y_ + 1),
        (BEWEGUNG_X_ + 1) * 2,
        (BEWEGUNG_Y_ + 1) * 2);

    // Senkrechte Linie des Kreuzes
    display.setDrawColor(0);
    display.drawLine(
        kreuz_X_, kreuz_Y_ - KREUZ_GROESSE,
        kreuz_X_, kreuz_Y_ + KREUZ_GROESSE);
    display.setDrawColor(1);
    display.drawLine(
        kreuz_X_ - KREUZ_GROESSE, kreuz_Y_,
        kreuz_X_ + KREUZ_GROESSE, kreuz_Y_);

    display.drawCircle(punkt_X_, punkt_Y_, PUNKT_RADIUS);
    display.display();
}

void Display::zeichneBatterieStatus(uint16_t batteryLevel)
{
    display.print("Batterie: ");
    display.println(batteryLevel); // Example value, replace with actual battery reading
    display.display();
}

void Display::zeichneEncoderStatus(int16_t counter)
{
    if (encoderCounterAlt == counter)
        return; // No change, no need to update

    int16_t angle = counter; // Limit the counter to a reasonable range

    Serial.print("Encoder: ");
    Serial.println(angle); // Debugging output to Serial Monitor

    display.display();
    encoderCounterAlt = angle;
}
