// Menu.h
#pragma once
/**
 * update() verarbeitet den allgemeinen Ablauf und Back.
 * handleMenu() verarbeitet Bewegung und Auswahl.
 * selectEntry() wechselt die Seite.
 * draw() wählt die passende Displaydarstellung.
 * SenderDisplay zeichnet das tatsächliche Layout.
 */
#include <Arduino.h>
#include "SenderControls.h"
#include "SenderDisplay.h"
#include "FunkSteuerung.h"
#include "TimingData.h"

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
    explicit Menu(SenderDisplay &display)
        : display(display)
    {
    }
    void begin() {};
    void update(
        const SenderControlEvents &events,
        const FunkSteuerungData &funkData,
        const TimingData &timingData);

private:
    SenderDisplay &display;

    MenuPage currentPage = MenuPage::MAIN;
    uint8_t selectedEntry = 0;

    static constexpr uint32_t DIAGNOSTICS_REFRESH_MS = 100;
    uint32_t lastDiagnosticsRefresh = 0;
    bool redrawRequired = true;

    void handleMenu(const SenderControlEvents &events, uint8_t numEntries);

    // moveSelection is used for both main menu and diagnostics menu
    void moveSelection(int8_t direction, uint8_t numEntries);
    void selectEntry();
    void goBack();

    void draw(
        const FunkSteuerungData &funkData,
        const TimingData &timingData);
};