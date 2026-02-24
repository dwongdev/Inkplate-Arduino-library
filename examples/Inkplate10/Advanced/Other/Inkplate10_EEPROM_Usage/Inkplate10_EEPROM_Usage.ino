/**
 **************************************************
 * @file        Inkplate10_EEPROM_Usage.ino
 * @brief       EEPROM read/write example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to use the built-in EEPROM on Inkplate 10 to
 *              store data that persists across resets and power cycles.
 *              The example shows how to safely clear, write, and read user data
 *              from EEPROM, while respecting reserved address ranges used by
 *              the e-paper waveform data.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 10.
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
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_START_ADDR 76  // Start EEPROM address for user data. Addresses below address 76 are waveform data!
#define DATA_SIZE         128 // How much data to write to EEPROM in this example
#define EEPROM_SIZE       EEPROM_START_ADDR + DATA_SIZE // End address of the EEPROM


Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    display.begin(); // Init library (you should call this function ONLY ONCE)

    // Init EEPROM library with EEPROM_SIZE size. Do not change this value, it can wipe waveform data!
    EEPROM.begin(EEPROM_SIZE);

    display.setTextSize(6);                  // Set text size
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

// Function for clearing EEPROM data (it will NOT clear waveform data)
void clearEEPROM()
{
    for (int i = 0; i < DATA_SIZE; i++)
    {
        EEPROM.write(i + EEPROM_START_ADDR,
                     0); // Start writing from address 76 (anything below that address number is waveform data!)
    }
    EEPROM.commit();
}

// Function writes data to EEPROM
void writeEEPROM()
{
    for (int i = 0; i < DATA_SIZE; i++)
    {
        EEPROM.write(i + EEPROM_START_ADDR,
                     i); // Start reading from address 76 (anything below that address number is waveform data!)
    }
    EEPROM.commit();
}

// Function reads back previously written data and displays it on screen.
void printEEPROM()
{
    for (int i = 0; i < DATA_SIZE; i++)
    {
        display.print(EEPROM.read(i + EEPROM_START_ADDR), DEC);
        if (i != DATA_SIZE - 1)
            display.print(", ");
    }
    display.partialUpdate();
}
