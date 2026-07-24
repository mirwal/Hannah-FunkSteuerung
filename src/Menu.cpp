// Menu.cpp
#include "Menu.h"
void Menu::updateRefreshTiming()
{
    const uint32_t currentTime = millis();
    // für Speichermeldung refresh pausieren
    if (showSaveMessage)
    {
        if ((currentTime - saveMessageStart) >= SAVE_MESSAGE_DURATION_MS)
        {
            return;
        }
        showSaveMessage = false;
        redrawRequired = true;
    }

    // Für die dynamischen Seiten (Diagnostics und Calibration) wird alle 100 ms ein Refresh durchgeführt
    const bool isDynamicPage =
        currentPage == MenuPage::DIAGNOSTICS_ANALOG ||
        currentPage == MenuPage::DIAGNOSTICS_BUTTONS ||
        currentPage == MenuPage::DIAGNOSTICS_RADIO ||
        currentPage == MenuPage::DIAGNOSTICS_TIMING ||
        currentPage == MenuPage::CALIBRATION_CENTER ||
        currentPage == MenuPage::CALIBRATION_MIN_MAX;

    if (isDynamicPage && (currentTime - lastDiagnosticsRefresh >= DIAGNOSTICS_REFRESH_MS))
    {
        lastDiagnosticsRefresh = currentTime;
        redrawRequired = true;
    }
}
void Menu::update(const SenderControlEvents &events, const FunkSteuerungData &funkData, const TimingData &timingData)
{
    // Handle menu navigation and page updates
    handleCurrentPage(events);

    // Für Diagnostics und Kalibrierung
    updateRefreshTiming();

    // Kalibrierungslogik ????????

    // Zeichnen der aktuellen Seite, wenn erforderlich
    if (redrawRequired)
    {
        draw(funkData, timingData);
        redrawRequired = false;
    }

    // Navigation: Zurück-Taste
    if (events.backPressed)
    {
        goBack();
    }
}

void Menu::handleCurrentPage(const SenderControlEvents &events)
{
    // Navigation
    // Refresh-Timing
    // Kalibrierung speichern
    // Kalibrierung aktualisieren
    // Zeichnen
    switch (currentPage)
    {
    case MenuPage::MAIN:
        handleMenu(events, 3);
        break;

    case MenuPage::DIAGNOSTICS:
        handleMenu(events, 4);
        break;
    case MenuPage::CALIBRATION:
        handleMenu(events, 2);
        break;

    case MenuPage::DIAGNOSTICS_ANALOG:
        // nur anzeigen mit refresh alle 100 ms
        // laufende Messung aktualisieren
        // dienst nur der Anzeige, die Werte werden im Hintergrund gemessen
        // updateDiagnosticsAnalogPage(events);
        break;
    case MenuPage::DIAGNOSTICS_BUTTONS:
        // nur anzeigen mit refresh alle 100 ms
        // laufende Messung aktualisieren
        // dienst nur der Anzeige, die Werte werden im Hintergrund gemessen
        // updateDiagnosticsButtonsPage(events);
        break;
    case MenuPage::DIAGNOSTICS_RADIO:
        // nur anzeigen mit refresh alle 100 ms
        // laufende Messung aktualisieren
        // dienst nur der Anzeige, die Werte werden im Hintergrund gemessen
        // updateDiagnosticsRadioPage(events);
        break;
    case MenuPage::DIAGNOSTICS_TIMING:
        // nur anzeigen mit refresh alle 100 ms
        // laufende Messung aktualisieren
        // dienst nur der Anzeige, die Werte werden im Hintergrund gemessen
        // updateDiagnosticsTimingPage(events);
        break;
    case MenuPage::CALIBRATION_CENTER:
        // nur anzeigen mit refresh alle 100 ms
        // laufende Kalibrierung aktualisieren
        // mit der clear Taste speichern
        // mit der back Taste eine Ebene zurück ohne speichern
        // updateCalibrationCenterPage(events);
        if (events.clearPressed)
        {
            // funkSteuerung.calibrateJoystickCenters();    // hier müssen die werte nur überschrieben werden, die funksteuerung speichert die werte in der eeprom
            display.drawSaveConfirmation("Gespeichert"); // das ist nur eine anzeige, die funksteuerung speichert die werte in der eeprom
            Serial.println("SAVE Calibration: center: ");
            saveMessageStart = millis() + 1000;
            showSaveMessage = true;
            currentPage = MenuPage::CALIBRATION; // zurück zur Kalibrierungshauptseite, ist erwünscht, da die Kalibrierung abgeschlossen ist
            return;
        }
        break;
    case MenuPage::CALIBRATION_MIN_MAX:
        // nur anzeigen mit refresh alle 100 ms
        // laufende Kalibrierung aktualisieren
        // mit der clear Taste speichern
        // mit der back Taste eine Ebene zurück ohne speichern
        // updateCalibrationMinMaxPage(events);
        funkSteuerung.updateMinMaxCalibration();

        if (events.clearPressed)
        {
            // funkSteuerung.saveMinMaxCalibration();
            display.drawSaveConfirmation("Gespeichert");
            Serial.println("SAVE Calibration: min/max: ");
            saveMessageStart = millis() + 1000;
            showSaveMessage = true;
            currentPage = MenuPage::CALIBRATION; // zurück zur Kalibrierungshauptseite, ist erwünscht, da die Kalibrierung abgeschlossen ist
            return;
        }

        break;
    case MenuPage::BATTERY:
        // nur anzeigen mit refresh alle 100 ms
        // ich weiß nicht ob ich hier die logik rein machen soll, oder ob das nur eine anzeige ist
        // updateBatteryPage(events);
        break;
    }
}

void Menu::handleMenu(const SenderControlEvents &events, uint8_t numEntries)
{
    if (events.encoderDirection != 0)
    {
        moveSelection(events.encoderDirection, numEntries);
        redrawRequired = true;
    }

    if (events.encoderPressed)
    {
        selectEntry();
        redrawRequired = true;
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
        switch (selectedEntry)
        {
        case 0:
            currentPage = MenuPage::DIAGNOSTICS;
            Serial.println("Diagnostics selected");
            break;
        case 1:
            currentPage = MenuPage::CALIBRATION;
            Serial.println("Calibration selected");
            break;
        case 2:
            currentPage = MenuPage::BATTERY;
            Serial.println("Battery selected");
            break;
        default:
            break;
        }
        break;
    case MenuPage::DIAGNOSTICS:
        switch (selectedEntry)
        {
        case 0:
            Serial.println("Diagnostics Analog selected");
            currentPage = MenuPage::DIAGNOSTICS_ANALOG;
            break;

        case 1:
            Serial.println("Diagnostics Buttons selected");
            currentPage = MenuPage::DIAGNOSTICS_BUTTONS;
            break;

        case 2:
            Serial.println("Diagnostics Radio selected");
            currentPage = MenuPage::DIAGNOSTICS_RADIO;
            break;

        case 3:
            Serial.println("Diagnostics Timing selected");
            currentPage = MenuPage::DIAGNOSTICS_TIMING;
            break;
        }
        break;
    case MenuPage::CALIBRATION:
        switch (selectedEntry)
        {
        case 0:
            Serial.println("Calibration Center selected");
            currentPage = MenuPage::CALIBRATION_CENTER;
            break;

        case 1:
            Serial.println("Calibration Min/Max selected");
            currentPage = MenuPage::CALIBRATION_MIN_MAX;
            break;
        }
        break;
    case MenuPage::BATTERY:
        break;
    }
    selectedEntry = 0;
    redrawRequired = true;
}

void Menu::goBack()
{
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
        currentPage = MenuPage::MAIN;
        break;

    case MenuPage::MAIN:
        return;
    }

    selectedEntry = 0;
    redrawRequired = true;
}

void Menu::draw(const FunkSteuerungData &funkData, const TimingData &timingData)
{
    switch (currentPage)
    {
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
        break;
    }
}
