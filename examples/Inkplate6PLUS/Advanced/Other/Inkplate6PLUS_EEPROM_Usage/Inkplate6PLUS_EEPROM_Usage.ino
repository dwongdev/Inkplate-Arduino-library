/**
 **************************************************
 * @file        Inkplate6PLUS_EEPROM_Usage.ino
 * @brief       EEPROM read/write example for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to use the built-in EEPROM on Inkplate 6PLUS to
 *              store data that persists across resets and power cycles.
 *              The example shows how to safely clear, write, and read user data
 *              from EEPROM, while respecting reserved address ranges used by
 *              the e-paper waveform data.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6PLUS
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6PLUS.
 * 2) The display will first clear user EEPROM data.
 * 3) Sample data is written to EEPROM.
 * 4) Stored data is read back and shown on the display.
 *
 * Expected output:
 * - Messages indicating EEPROM clearing, writing, and reading.
 * - A list of values read from EEPROM displayed on the screen.
 *
 * Notes:
 * - EEPROM addresses 0–75 are reserved for waveform data.
 * - DO NOT read from or write to addresses below 76.
 * - Changing EEPROM size may erase waveform data and affect display operation.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-13
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_SIZE 128 // How much data to write to EEPROM in this example

Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin(); // Init library (you should call this function ONLY ONCE)

    // Init EEPROM library with 128 of EEPROM size.
    EEPROM.begin(EEPROM_SIZE);

    display.setTextSize(5);                  // Set text size
    display.println("Clearing EEPROM...\n"); // Print message
    display.display();                       // Full refresh the display
    clearEEPROM();                           // Clear user EEPROM data
    delay(500);                              // Wait a little bit...

    display.println("Writing data to EEPROM...\n"); // Print message
    display.partialUpdate();                        // Use partial updates for refreshing the display
    writeEEPROM();                                  // Write some data to EEPROM
    delay(500);                                     // Wait a little bit...

    display.println("Reading data from EEPROM:\n"); // Print message
    display.partialUpdate();                        // Use partial updates for refreshing the display
    display.setTextSize(3);                         // Use smaller text so everything can fit on display
    printEEPROM();                                  // Read data from EEPROM and display it on screen
    delay(500);                                     // Wait a little bit...
}

void loop()
{
    // Empty...
}

// Function for clearing EEPROM data
void clearEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0); 
    }
    EEPROM.commit();
}

// Function writes data to EEPROM
void writeEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, i);
    }
    EEPROM.commit();
}

// Function reads back previously written data and displays it on screen.
void printEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        display.print(EEPROM.read(i), DEC);
        if (i != EEPROM_SIZE - 1)
            display.print(", ");
    }
    display.partialUpdate();
}
