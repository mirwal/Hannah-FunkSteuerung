// Menu.cpp
#include "Menu.h"

void Menu::update(const SenderControlEvents &events, const FunkSteuerungData &funkData, const TimingData &timingData, const BatteryData &batteryData)
{
    handleCurrentPage(events); // Handle menu navigation and page updates
    handleBack(events);        // Navigation: Zurück-Taste
    updateDisplayRefresh();    // Für Diagnostics und Kalibrierung

    if (redrawRequired)
    {
        draw(funkData, timingData, batteryData);
        redrawRequired = false;
    }
}
void Menu::handleCurrentPage(const SenderControlEvents &events)
{

    // handleCurrentPage() verarbeitet Verhalten und Eingaben.
    // updateDisplayRefresh() plant regelmäßige Aktualisierungen.
    // draw() zeichnet die Seite.
    // redrawRequired verbindet Planung und Darstellung.

    const uint8_t mainMenuEntryCount = sizeof(mainMenuEntries) / sizeof(mainMenuEntries[0]);
    const uint8_t diagnosticsMenuEntryCount = sizeof(diagnosticsMenuEntries) / sizeof(diagnosticsMenuEntries[0]);
    const uint8_t calibrationMenuEntryCount = sizeof(calibrationMenuEntries) / sizeof(calibrationMenuEntries[0]);

    switch (currentPage)
    {
    case MenuPage::HOME:

        if (events.encoderPressed)
        {
            currentPage = MenuPage::MAIN;
            selectedEntry = 0;
            redrawRequired = true;
        }
        if (events.clearPressed)
        {
            display.drawSaveConfirmation(static_cast<const char *>(__DATE__));
            saveMessageStart = millis();
            showSaveMessage = true;
        }
        break;
    case MenuPage::MAIN:
        handleListNavigation(events, mainMenuEntryCount);
        break;

    case MenuPage::DIAGNOSTICS:
        handleListNavigation(events, diagnosticsMenuEntryCount);
        break;
    case MenuPage::CALIBRATION:
        handleListNavigation(events, calibrationMenuEntryCount);
        break;

    case MenuPage::DIAGNOSTICS_ANALOG:
        break;
    case MenuPage::DIAGNOSTICS_BUTTONS:
        break;
    case MenuPage::DIAGNOSTICS_RADIO:
        break;
    case MenuPage::DIAGNOSTICS_TIMING:
        break;
    case MenuPage::CALIBRATION_CENTER:
        if (events.clearPressed)
        {

            funkSteuerung.calibrateJoystickCenters(); // hier müssen die werte nur überschrieben werden, die funksteuerung speichert die werte in der eeprom
            display.drawSaveConfirmation("Gespeichert");
            saveMessageStart = millis();
            showSaveMessage = true;
            currentPage = MenuPage::CALIBRATION; // zurück zur Kalibrierungshauptseite, ist erwünscht, da die Kalibrierung abgeschlossen ist
            return;
        }
        break;
    case MenuPage::CALIBRATION_MIN_MAX:
        funkSteuerung.updateMinMaxCalibration();

        if (events.clearPressed)
        {
            funkSteuerung.saveMinMaxCalibration();
            display.drawSaveConfirmation("Gespeichert");
            saveMessageStart = millis();
            showSaveMessage = true;
            currentPage = MenuPage::CALIBRATION; // zurück zur Kalibrierungshauptseite, ist erwünscht, da die Kalibrierung abgeschlossen ist
            return;
        }

        break;
    case MenuPage::BATTERY:
        break;
    case MenuPage::ABOUT:
        break;
    }
}

void Menu::handleListNavigation(const SenderControlEvents &events, uint8_t numEntries)
{
    if (events.encoderDirection != 0)
    {
        moveSelection(events.encoderDirection, numEntries);
        redrawRequired = true;
    }

    if (events.encoderPressed)
    {
        selectEntry();
    }
}

void Menu::moveSelection(int8_t direction, uint8_t numEntries)
{
    if (direction > 0)
    {
        selectedEntry = (selectedEntry + 1) % numEntries;
    }
    else if (direction < 0)
    {
        selectedEntry = (selectedEntry + numEntries - 1) % numEntries;
    }
}

void Menu::selectEntry()
{

    switch (currentPage)
    {
    case MenuPage::MAIN:
        currentPage = mainMenuEntries[selectedEntry].targetPage;
        break;
    case MenuPage::DIAGNOSTICS:
        currentPage = diagnosticsMenuEntries[selectedEntry].targetPage;
        break;

    case MenuPage::CALIBRATION:
        currentPage = calibrationMenuEntries[selectedEntry].targetPage;
        break;
    default:
        currentPage = MenuPage::HOME; // Fallback to HOME if no valid selection
        break;
    }

    selectedEntry = 0;
    redrawRequired = true;
}

void Menu::handleBack(const SenderControlEvents &events)
{
    if (!events.backPressed)
    {
        return;
    }

    switch (currentPage)
    {
    case MenuPage::CALIBRATION_CENTER:
    case MenuPage::CALIBRATION_MIN_MAX:
        currentPage = MenuPage::CALIBRATION;
        break;

    case MenuPage::DIAGNOSTICS_ANALOG:
    case MenuPage::DIAGNOSTICS_BUTTONS:
    case MenuPage::DIAGNOSTICS_RADIO:
    case MenuPage::DIAGNOSTICS_TIMING:
        currentPage = MenuPage::DIAGNOSTICS;
        break;

    case MenuPage::DIAGNOSTICS:
    case MenuPage::CALIBRATION:
    case MenuPage::BATTERY:
    case MenuPage::ABOUT:
        currentPage = MenuPage::MAIN;
        break;

    case MenuPage::MAIN:
        currentPage = MenuPage::HOME;
        break;
    default:
        currentPage = MenuPage::HOME; // Fallback to HOME if no valid back action
        break;
    }

    selectedEntry = 0;
    redrawRequired = true;
}

void Menu::updateDisplayRefresh()
{
    const uint32_t currentTime = millis();
    // für Speichermeldung refresh pausieren
    if (showSaveMessage)
    {
        // vergangene Zeit = aktuelle Zeit - Startzeit
        // wenn (vergangene Zeit) < (Dauer der Speichermeldung) dann return
        if ((currentTime - saveMessageStart) < SAVE_MESSAGE_DURATION_MS)
        {
            return;
        }
        showSaveMessage = false;
        redrawRequired = true;
    }

    // Für die dynamischen Seiten (Diagnostics und Calibration) wird alle 100 ms ein Refresh durchgeführt
    const bool isDynamicPage =
        currentPage == MenuPage::HOME ||
        currentPage == MenuPage::ABOUT ||
        currentPage == MenuPage::DIAGNOSTICS_ANALOG ||
        currentPage == MenuPage::DIAGNOSTICS_BUTTONS ||
        currentPage == MenuPage::DIAGNOSTICS_RADIO ||
        currentPage == MenuPage::DIAGNOSTICS_TIMING ||
        currentPage == MenuPage::CALIBRATION_CENTER ||
        currentPage == MenuPage::CALIBRATION_MIN_MAX;

    // Wenn (istDynamicPage) ( Zeit seit dem letzten Refresh) >= (DISPLAY_REFRESH_MS ) dann setze redrawRequired auf true
    if (isDynamicPage && (currentTime - lastDisplayRefresh >= DISPLAY_REFRESH_MS))
    {
        lastDisplayRefresh = currentTime;
        redrawRequired = true;
    }
}

void Menu::draw(const FunkSteuerungData &funkData, const TimingData &timingData, const BatteryData &batteryData)
{
    switch (currentPage)
    {
    case MenuPage::HOME:
        display.drawHome(funkData, batteryData);
        break;
    case MenuPage::MAIN:
        display.drawMainMenu(selectedEntry);
        break;
    case MenuPage::DIAGNOSTICS:
        display.drawDiagnosticsMenu(selectedEntry);
        break;
    case MenuPage::DIAGNOSTICS_ANALOG:
        display.drawAnalogValues(funkData);
        break;
    case MenuPage::DIAGNOSTICS_BUTTONS:
        display.drawButtonStates(funkData);
        break;
    case MenuPage::DIAGNOSTICS_RADIO:
        display.drawRadioValues(funkData);
        break;
    case MenuPage::DIAGNOSTICS_TIMING:
        display.drawTimingValues(funkData, timingData);
        break;
    case MenuPage::CALIBRATION:
        display.drawCalibrationMenu(selectedEntry);
        break;
    case MenuPage::CALIBRATION_CENTER:
        display.drawCalibrationCenter(funkData);
        break;
    case MenuPage::CALIBRATION_MIN_MAX:
        display.drawCalibrationMinMax(funkData);
        break;
    case MenuPage::BATTERY:
        display.drawBattery(batteryData);
        break;
    case MenuPage::ABOUT:
        display.drawAbout();
        break;
    }
}
