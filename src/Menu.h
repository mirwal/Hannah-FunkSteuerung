// Menu.h
#pragma once

#include <Arduino.h>
#include "SenderControls.h"

enum class MenuPage : uint8_t
{
    MAIN,
    DIAGNOSTICS,
    CALIBRATION,
    BATTERY
};

class Menu
{
public:
    void begin() {};
    void update(const SenderControlEvents &events);

private:
    MenuPage currentPage = MenuPage::MAIN;
    uint8_t selectedEntry = 0;
    bool redrawRequired = true;

    void handleMainMenu(const SenderControlEvents &events, uint8_t numEntries);

    void handleSubmenu(const SenderControlEvents &events);

    void moveSelection(int8_t direction, uint8_t numEntries);
    void selectEntry();
    void goBack();

    void draw();

    void drawMainMenu() const;
    void drawCurrentPage() const;
};