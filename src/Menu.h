// Menu.h
#pragma once

#include <Arduino.h>
#include "SenderControls.h"

enum class MenuPage : uint8_t
{
    MAIN,
    DIAGNOSTICS,
    DIAGNOSTICS_ANALOG,
    DIAGNOSTICS_BUTTONS,
    DIAGNOSTICS_RADIO,
    DIAGNOSTICS_TIMING,
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

    // DIAGNOSTICS menu handling
    void handleDiagnosticsMenu(const SenderControlEvents &events, uint8_t numEntries);
    void selectDiagnosticsEntry();
    void drawDiagnosticsMenu() const;

    // moveSelection is used for both main menu and diagnostics menu
    void moveSelection(int8_t direction, uint8_t numEntries);
    void selectEntry();
    void goBack();

    void draw();

    void drawMainMenu() const;
    void drawCurrentPage() const;
};