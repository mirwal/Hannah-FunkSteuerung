// Menu.cpp
#include "Menu.h"

void Menu::update(
    const SenderControlEvents &events,
    const FunkSteuerungData &funkData,
    const TimingData &timingData)
{
    switch (currentPage)
    {
    case MenuPage::MAIN:
        handleMenu(events, 3);
        break;

    case MenuPage::DIAGNOSTICS:
        handleMenu(events, 4);
        break;

    case MenuPage::DIAGNOSTICS_ANALOG:
    case MenuPage::DIAGNOSTICS_BUTTONS:
    case MenuPage::DIAGNOSTICS_RADIO:
    case MenuPage::DIAGNOSTICS_TIMING:
    case MenuPage::CALIBRATION:
    case MenuPage::BATTERY:
        break;
    }

    // Dynamische Diagnoseseiten alle 100 ms neu zeichnen
    if (currentPage == MenuPage::DIAGNOSTICS_ANALOG || currentPage == MenuPage::DIAGNOSTICS_BUTTONS || currentPage == MenuPage::DIAGNOSTICS_RADIO ||
        currentPage == MenuPage::DIAGNOSTICS_TIMING)
    {
        const uint32_t currentTime = millis();

        if (currentTime - lastDiagnosticsRefresh >= DIAGNOSTICS_REFRESH_MS)
        {
            lastDiagnosticsRefresh = currentTime;
            redrawRequired = true;
        }
    }

    // if (events.clearPressed)
    // {
    //     Serial.println("Clear pressed, going back to main menu");
    //     // redrawRequired = true;
    // }

    if (redrawRequired)
    {
        draw(funkData, timingData);
        redrawRequired = false;
    }
    if (events.backPressed)
    {
        goBack();
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
    case MenuPage::BATTERY:
        break;
    }
}
