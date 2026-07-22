#include "Menu.h"

void Menu::update(const SenderControlEvents &events)
{
    {
        // Handle input events based on the current page
        switch (currentPage)
        {
        case MenuPage::MAIN:
            handleMainMenu(events, 3);
            break;
        case MenuPage::DIAGNOSTICS:
            handleSubmenu(events);
            break;
        case MenuPage::CALIBRATION:
            handleSubmenu(events);
            break;
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
    currentPage = MenuPage::MAIN;
    selectedEntry = 0;
    redrawRequired = true;

    Serial.println("Zurück zum Hauptmenü");
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
    case MenuPage::DIAGNOSTICS:
        Serial.println("== Diagnose ==");
        Serial.println();
        Serial.println("Diagnosedaten kommen später");
        break;

    case MenuPage::CALIBRATION:
        Serial.println("== Kalibrierung ==");
        Serial.println();
        Serial.println("Kalibrierung kommt später");
        break;

    case MenuPage::BATTERY:
        Serial.println("== Batterie ==");
        Serial.println();
        Serial.println("Batteriedaten kommen später");
        break;

    case MenuPage::MAIN:
        break;
    }

    Serial.println();
    Serial.println("Back: zurück");
}