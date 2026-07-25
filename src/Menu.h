// Menu.h
#pragma once
/**
 * update() verarbeitet den allgemeinen Ablauf und Back.
 * handleListNavigation() verarbeitet Bewegung und Auswahl.
 * selectEntry() wechselt die Seite.
 * draw() wählt die passende Displaydarstellung.
 * SenderDisplay zeichnet das tatsächliche Layout.
 */
#include <Arduino.h>
#include "SenderControls.h"
#include "SenderDisplay.h"
#include "FunkSteuerung.h"
#include "TimingData.h"
#include "BatteryMonitor.h"

enum class MenuPage : uint8_t
{
    HOME,
    MAIN,
    DIAGNOSTICS,
    DIAGNOSTICS_ANALOG,
    DIAGNOSTICS_BUTTONS,
    DIAGNOSTICS_RADIO,
    DIAGNOSTICS_TIMING,
    CALIBRATION,
    CALIBRATION_CENTER,
    CALIBRATION_MIN_MAX,
    BATTERY,
    ABOUT
};

struct MenuEntry
{
    // const char *label;
    MenuPage targetPage;
};

class Menu
{
public:
    explicit Menu(SenderDisplay &display, FunkSteuerung &funkSteuerung)
        : display(display), funkSteuerung(funkSteuerung)
    {
    }

    void begin() {};
    void update(const SenderControlEvents &events, const FunkSteuerungData &funkData, const TimingData &timingData, const BatteryData &batteryData);

private:
    // Abhängigkeiten
    SenderDisplay &display;
    FunkSteuerung &funkSteuerung;

    // Menüzustand
    MenuPage currentPage = MenuPage::HOME;
    uint8_t selectedEntry = 0;
    bool redrawRequired = true;

    // Refresh-Timing für dynamische Seiten (Diagnostics und Calibration)
    static constexpr uint32_t DISPLAY_REFRESH_MS = 250;
    uint32_t lastDisplayRefresh = 0;

    // Speichermeldung
    static constexpr uint32_t SAVE_MESSAGE_DURATION_MS = 1000;
    uint32_t saveMessageStart = 0;
    bool showSaveMessage = false;

    // Menüedefinitionen
    const MenuEntry mainMenuEntries[4] = {
        {MenuPage::DIAGNOSTICS},
        {MenuPage::CALIBRATION},
        {MenuPage::BATTERY},
        {MenuPage::ABOUT}};

    const MenuEntry diagnosticsMenuEntries[4] = {
        {MenuPage::DIAGNOSTICS_ANALOG},
        {MenuPage::DIAGNOSTICS_BUTTONS},
        {MenuPage::DIAGNOSTICS_RADIO},
        {MenuPage::DIAGNOSTICS_TIMING}};

    const MenuEntry calibrationMenuEntries[2] = {
        {MenuPage::CALIBRATION_CENTER},
        {MenuPage::CALIBRATION_MIN_MAX}};

    // Methoden
    void handleCurrentPage(const SenderControlEvents &events);
    void handleListNavigation(const SenderControlEvents &events, uint8_t numEntries);
    void moveSelection(int8_t direction, uint8_t numEntries);
    void selectEntry();
    void handleBack(const SenderControlEvents &events);
    void updateDisplayRefresh();
    void draw(const FunkSteuerungData &funkData, const TimingData &timingData, const BatteryData &batteryData);
};