/**
 **************************************************
 * @file        Inkplate6COLOR_EEPROM_Usage.ino
 * @brief       Demonstrates basic EEPROM operations on Inkplate 6COLOR.
 *
 * @details     This example shows how to use the EEPROM memory available on
 *              the ESP32 used by Inkplate boards. EEPROM is non-volatile
 *              storage, meaning the data remains stored even after power loss
 *              or device reset.
 *
 *              The sketch demonstrates three basic EEPROM operations:
 *              clearing previously stored data, writing new values, and
 *              reading the stored values back. After initialization, the
 *              example clears a defined EEPROM region, writes sequential
 *              numbers into memory, and then reads those values back.
 *
 *              The read data is printed both to the Serial Monitor and the
 *              Inkplate display so that the stored values can be verified.
 *
 *              This technique is useful for storing persistent configuration
 *              values such as device settings, counters, calibration values,
 *              or user preferences.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial Monitor: 115200 baud
 * - EEPROM_SIZE defines how many bytes are used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the sketch.
 * 2) Open the Serial Monitor at 115200 baud.
 * 3) The sketch clears the defined EEPROM region.
 * 4) Sequential values are written into EEPROM.
 * 5) The stored values are read back and displayed on both the Serial Monitor
 *    and the Inkplate screen.
 *
 * Expected output:
 * - Serial Monitor: A list of stored EEPROM values.
 * - Display: The same list of values printed on the screen.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode (used only for text
 *   output in this example).
 * - EEPROM on ESP32 is emulated in flash memory. Frequent writes may reduce
 *   flash lifespan, so avoid unnecessary write operations in production code.
 * - Always call EEPROM.commit() after writing to ensure changes are stored.
 * - The EEPROM region size must be defined during EEPROM.begin().
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-05
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "EEPROM.h"   // Include ESP32 EEPROM library
#include "Inkplate.h" // Include Inkplate library to the sketch

#define EEPROM_SIZE 128 // How much data to write to EEPROM in this example

Inkplate display; // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    Serial.begin(115200);                 // Init serial monitor to display what's happening
    display.begin();                      // Init library (you should call this function ONLY ONCE)
    display.setTextColor(INKPLATE_BLACK); // Set text color to black

    // Init EEPROM library with 128 of EEPROM size.
    EEPROM.begin(EEPROM_SIZE);

    display.setTextSize(4);               // Set text size
    Serial.println("Cleaning EEPROM..."); // Print message on serial monitor
    clearEEPROM();                        // Clear user EEPROM data
    delay(500);                           // Wait a little bit...

    Serial.println("Writing data to EEPROM..."); // Print message on serial monitor
    writeEEPROM();                               // Write some data to EEPROM
    delay(500);                                  // Wait a little bit...

    display.println("Reading data from EEPROM:\n"); // Print message on the screen
    Serial.println("Reading data from EEPROM...");  // Print message on the Sserial Monitor
    display.setTextSize(2);                         // Use smaller text so everything can fit on display
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

// Function reads back previously written data and displays it on serial monitor and screen.
void printEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        display.print(EEPROM.read(i), DEC);
        Serial.print(EEPROM.read(i), DEC);
        if (i != EEPROM_SIZE - 1)
        {
            display.print(", ");
            Serial.print(", ");
        }
    }
    display.display();
}
