/**
 **************************************************
 * @file        Inkplate6COLOR_SD_TXT_Write.ino
 * @brief       Demonstrates writing text data to a file on an SD card using
 *              Inkplate 6COLOR.
 *
 * @details     This example shows how to initialize the SD card interface on
 *              Inkplate 6COLOR and create or open a text file for writing.
 *              The sketch writes a predefined string into a file named
 *              "test.txt" stored on the microSD card.
 *
 *              The example uses the SdFat-based file interface provided by
 *              the Inkplate library. After initializing the SD card, the
 *              program opens a file in write mode, writes a text string,
 *              closes the file, and powers down the SD card interface.
 *
 *              Status messages are printed to the Serial Monitor to show
 *              whether the SD card initialization and file operations were
 *              successful.
 *
 *              This workflow can be used for logging data, storing settings,
 *              writing sensor measurements, or generating files for later
 *              retrieval.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      microSD card formatted as FAT/FAT32
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial Monitor: 115200 baud
 * - Insert a compatible microSD card before running the sketch
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Format a microSD card using FAT or FAT32.
 * 2) Insert the SD card into the Inkplate 6COLOR slot.
 * 3) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the sketch.
 * 4) Open the Serial Monitor at 115200 baud.
 * 5) The sketch initializes the SD card and attempts to create/open
 *    "test.txt".
 * 6) A text string is written to the file, then the file is closed.
 *
 * Expected output:
 * - Serial Monitor: Messages indicating SD card initialization success and
 *   confirmation that data has been written to the file.
 * - SD Card: A file named "test.txt" containing the example text string.
 *
 * Notes:
 * - Display mode: not used in this example (no screen output).
 * - The SdFat file interface allows more advanced file operations such as
 *   appending data or managing multiple files.
 * - Always close files after writing to ensure data is flushed to the card.
 * - The SD card power rail is disabled with sdCardSleep() after operations
 *   to reduce power consumption.
 * - Ensure the SD card is properly formatted and inserted before running
 *   the sketch.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-01-23
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library
SdFile file;          // Create SdFile object used for accessing files on SD card

char *fileName = "test.txt"; // The name of a file with the extension.

// The text you want to write in the file
char *dataToWrite = "Hello! This is the file writing example for Inkplate 6COLOR.\n"; // "\n" represents a new line

void setup()
{
    Serial.begin(115200); // Init serial communication
    display.begin();      // Init Inkplate library (you should call this function ONLY ONCE)

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        Serial.println("SD Card ok!");

        // Try to create text.txt file.
        if (!file.open(fileName, FILE_WRITE))
        { // If it fails to create, send an error message to display, otherwise write to file.
            Serial.println("Error while creating the file!");
            display.sdCardSleep();
        }
        else
        {
            Serial.println("Writing in the file...");
            file.write(dataToWrite); // Write data string in the file
            Serial.println("Data has been written successfully!");
            file.close();          // Close the file
            display.sdCardSleep(); // Put sd card in sleep mode
        }
    }
    else
    { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program
      // (using infinite loop)
        Serial.println("SD Card error!");
        display.sdCardSleep();
        while (true)
            ;
    }
}

void loop()
{
    // Nothing...
}
