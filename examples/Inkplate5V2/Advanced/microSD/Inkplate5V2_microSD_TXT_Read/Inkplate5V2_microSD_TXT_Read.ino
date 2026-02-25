/**
 **************************************************
 * @file        Inkplate5v2_SD_TXT_Read.ino
 * @brief       Read and display text file from SD card on Soldered Inkplate 5v2.
 *
 * @details     Demonstrates how to open a .txt file from a FAT-formatted SD card
 *              and display its contents on the Inkplate 5v2 e-paper display.
 *              The example reads a file named "text.txt" from the SD card and
 *              prints its content on screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable, microSD card
 * - Extra:      text.txt file on SD card (≤ 200 characters)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5v2
 * - Additional library required:
 *   https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library
 * - SD card format: FAT / FAT32
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 *
 * How to use:
 * 1) Copy a text file named "text.txt" (max 200 characters) to a FAT-formatted SD card.
 * 2) Insert the SD card into the Inkplate.
 * 3) Install the required SDFat library.
 * 4) Upload the sketch to Inkplate 5v2.
 * 5) The file contents will be read and displayed on the e-paper screen.
 *
 * Expected output:
 * - The contents of text.txt are displayed on the Inkplate display.
 *
 * Notes:
 * - File name must be exactly "text.txt" for this example.
 * - Text length should not exceed 200 characters.
 * - Ensure the required SDFat library is properly installed.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

// Define how many characters will be read from the .txt file. Change if you want to read larger files
#define MAX_LENGTH 1500

// Here is the specified name of the file
const char fileName[] = "/text.txt";

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 3 times bigger than original (5x7 px)

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card ok! Reading data...");
        display.partialUpdate();

        // Try to load text with a max length of 500 chars. You can change this by changing MAX_LENGTH define
        if (!file.open(fileName, O_RDONLY))
        {
            // If it fails to open, send error message to display and put sd card in sleep mode, otherwise read the
            // file.
            display.println("File open error");
            display.display();
            display.sdCardSleep();
        }
        else
        {
            display.clearDisplay();    // Clear everything that is stored in frame buffer of epaper
            display.setCursor(0, 0);   // Set print position at the begining of the screen
            char text[MAX_LENGTH + 1]; // An array where data from an SD card is stored (max 500 chars here plus one for
                                       // the null terminating char)
            int len = file.fileSize(); // Read how big is file that we are opening
            if (len > MAX_LENGTH)
                len = MAX_LENGTH;  // If it's more than 500 bytes (500 chars), limit to max 500 bytes
            file.read(text, len);  // Read 500 chars from the file and save it in the text array
            text[len] = 0;         // Put the null terminating char at the end of data
            display.print(text);   // Print data/text
            display.sdCardSleep(); // Put the SD card in sleep mode
            display.display();     // Do a full refresh of display
        }
    }
    else
    {
        // If card init was not successful, display error on screen and stop the program (using infinite loop)
        display.println("SD Card error!");
        display.partialUpdate();
        display.sdCardSleep();
    }
}

void loop()
{
    // Nothing...
}
