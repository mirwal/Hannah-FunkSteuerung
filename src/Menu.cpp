// Menu.cpp
#include "Menu.h"

void Menu::update(const SenderControlEvents &events)
{

    // Handle input events based on the current page
    switch (currentPage)
    {
    case MenuPage::MAIN:
        handleMainMenu(events, 3);
        break;

    case MenuPage::DIAGNOSTICS:
        handleDiagnosticsMenu(events, 4);
        break;

    case MenuPage::DIAGNOSTICS_ANALOG:
    case MenuPage::DIAGNOSTICS_BUTTONS:
    case MenuPage::DIAGNOSTICS_RADIO:
    case MenuPage::DIAGNOSTICS_TIMING:
    case MenuPage::CALIBRATION:
    case MenuPage::BATTERY:
        handleSubmenu(events);
        break;
    }

    if (redrawRequired)
    {
        draw();
        redrawRequired = false;
    }
}

void Menu::handleMainMenu(const SenderControlEvents &events, uint8_t numEntries)
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

void Menu::handleSubmenu(const SenderControlEvents &events)
{
    if (events.backPressed)
    {
        goBack();
    }
}
// DIAGNOSTICS
void Menu::handleDiagnosticsMenu(const SenderControlEvents &events, uint8_t numEntries)
{
    if (events.encoderDirection != 0)
    {
        moveSelection(events.encoderDirection, numEntries);
        redrawRequired = true;
    }

    if (events.encoderPressed)
    {
        selectDiagnosticsEntry();
        // redrawRequired = true;
    }

    if (events.backPressed)
    {
        goBack();
    }
}

void Menu::selectDiagnosticsEntry()
{
    switch (selectedEntry)
    {
    case 0:
        currentPage = MenuPage::DIAGNOSTICS_ANALOG;
        break;
    case 1:
        currentPage = MenuPage::DIAGNOSTICS_BUTTONS;
        break;
    case 2:
        currentPage = MenuPage::DIAGNOSTICS_RADIO;
        break;
    case 3:
        currentPage = MenuPage::DIAGNOSTICS_TIMING;
        break;
    default:
        break;
    }
    selectedEntry = 0;
    redrawRequired = true;
}

void Menu::drawDiagnosticsMenu() const
{
    Serial.println("== Diagnose ==");
    Serial.println();
    selectedEntry == 0 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 1. Analogwerte");
    selectedEntry == 1 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 2. Tasterzustände");
    selectedEntry == 2 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 3. Funkstatus");
    selectedEntry == 3 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 4. Zeitmessung");
    Serial.println();
    Serial.println("Back: Hauptmenü");
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
        }
        break;

    case MenuPage::DIAGNOSTICS:
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

void Menu::draw()
{
    Serial.println();
    Serial.println("--------------------");
    switch (currentPage)
    {
    case MenuPage::MAIN:
        drawMainMenu();
        break;

    case MenuPage::DIAGNOSTICS:
        drawDiagnosticsMenu();
        break;
    case MenuPage::DIAGNOSTICS_ANALOG:
    case MenuPage::DIAGNOSTICS_BUTTONS:
    case MenuPage::DIAGNOSTICS_RADIO:
    case MenuPage::DIAGNOSTICS_TIMING:
    case MenuPage::CALIBRATION:
    case MenuPage::BATTERY:
        drawCurrentPage();
        break;
    }
}

void Menu::drawMainMenu() const
{

    Serial.println("== Hauptmenü ==");
    Serial.println("");
    selectedEntry == 0 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 1. Diagnose");
    selectedEntry == 1 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 2. Kalibrierung");
    selectedEntry == 2 ? Serial.print(">") : Serial.print(" ");
    Serial.println(" 3. Batterie");
}

//! ###############################################################################################
void Menu::drawCurrentPage() const
{
    switch (currentPage)
    {
    case MenuPage::DIAGNOSTICS_ANALOG:
        Serial.println("== Analogwerte ==");
        Serial.println();
        Serial.println("Analogwerte kommen später");
        break;

    case MenuPage::DIAGNOSTICS_BUTTONS:
        Serial.println("== Tasterzustände ==");
        Serial.println();
        Serial.println("Tasterzustände kommen später");
        break;

    case MenuPage::DIAGNOSTICS_RADIO:
        Serial.println("== Funkstatus ==");
        Serial.println();
        Serial.println("Funkstatus kommt später");
        break;

    case MenuPage::DIAGNOSTICS_TIMING:
        Serial.println("== Zeitmessung ==");
        Serial.println();
        Serial.println("Zeitmessung kommt später");
        break;

    case MenuPage::CALIBRATION:
        Serial.println("== Kalibrierung ==");
        break;

    case MenuPage::BATTERY:
        Serial.println("== Batterie ==");
        break;

    case MenuPage::MAIN:
    case MenuPage::DIAGNOSTICS:
        break;
    }

    Serial.println();
    Serial.println("Back: zurück");
}