#pragma once
#include <U8g2lib.h>
#include <Arduino.h>
#include <SPI.h>

constexpr uint8_t OLED_CS = 4;
constexpr uint8_t OLED_DC = 5;
constexpr uint8_t OLED_RST = 6;

class Display
{
public:
    Display(U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI &display)
        : display(display)
    {
    }

    Display(const Display &) = delete;            // Prevent copy-construction
    Display &operator=(const Display &) = delete; // Prevent assignment

    bool begin();

    void update(int16_t x1, int16_t y1);
    void zeichneJoystick(
        int16_t x,
        int16_t y);
    void zeichneBatterieStatus(uint16_t batteryLevel);
    void zeichneEncoderStatus(int16_t counter);
    void zeichneSchalterStatus(int16_t schalterMask)
    {

        uint8_t size = 5; // Size of the filled rectangle for each switch
        uint8_t y = 1;    // Starting X position for the first filled rectangle
        uint8_t x = 56;   // Spacing between the filled rectangles

        bitRead(schalterMask, 2) ? display.drawBox(x + 0, y, size, size) : display.drawBox(x + 0, y, size, size);
        bitRead(schalterMask, 1) ? display.drawBox(x + 10, y, size, size) : display.drawBox(x + 10, y, size, size);
        bitRead(schalterMask, 0) ? display.drawBox(x + 20, y, size, size) : display.drawBox(x + 20, y, size, size);
        bitRead(schalterMask, 5) ? display.drawBox(x + 0, y, size, size) : display.drawBox(x + 0, y, size, size);
        bitRead(schalterMask, 4) ? display.drawBox(x - 10, y, size, size) : display.drawBox(x - 10, y, size, size);
        bitRead(schalterMask, 3) ? display.drawBox(x - 20, y, size, size) : display.drawBox(x - 20, y, size, size);

        display.display();
    }
    void clearDisplay()
    {
        display.home();
        display.clearBuffer();
        display.print("Display cleared");
        display.clearDisplay();
        display.drawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); // Clear the entire display area
        display.display();
    }
    void run() { display.display(); }

private:
    U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI display;
    static constexpr uint8_t SCREEN_WIDTH = 128;
    static constexpr uint8_t SCREEN_HEIGHT = 64;
    int encoderCounterAlt = 0;
    static constexpr int8_t OLED_RESET = -1;
};
