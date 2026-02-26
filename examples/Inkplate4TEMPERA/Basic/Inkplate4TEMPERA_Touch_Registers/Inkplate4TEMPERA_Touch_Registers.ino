/**
 **************************************************
 * @file        Inkplate4TEMPERA_Touch_Registers.ino
 * @brief       Reads and prints raw touchscreen controller register bytes over
 *              Serial for advanced debugging on Inkplate 4 TEMPERA.
 *
 * @details     This example demonstrates low-level / advanced touchscreen
 *              diagnostics by reading raw register data from the Inkplate 4
 *              TEMPERA touchscreen controller. After initializing the display
 *              and touchscreen, the sketch periodically checks whether the
 *              touchscreen is responsive (available()). When available, it
 *              reads a fixed register snapshot (8 bytes) into a buffer and
 *              prints each register value to the Serial Monitor in binary
 *              format.
 *
 *              To help correlate coordinates with the screen origin, the sketch
 *              draws a small triangle marker and a "(0,0) position" label on
 *              the display. The e-paper output is static; all ongoing debug
 *              information is provided through Serial to avoid slow screen
 *              refresh overhead.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 4 TEMPERA
 * - Serial Monitor: 115200 baud
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 4 TEMPERA.
 * 2) Open Serial Monitor at 115200 baud.
 * 3) Every second, observe the printed register snapshot (8 bytes).
 * 4) Touch the screen and watch how register values change with activity.
 *
 * Expected output:
 * - Display: a triangle marker near the top-left corner and the text
 *   "(0,0) position".
 * - Serial Monitor: repeated blocks such as:
 *   - Reg [0] .. Reg [7] printed in binary
 *   - A separator line between snapshots
 *
 * Notes:
 * - Display mode: 1-bit BW (INKPLATE_1BIT). The display is used only for the
 *   static origin marker and label.
 * - This is a raw-register debug tool. The meaning of individual bits depends
 *   on the touchscreen controller and firmware; use this when developing or
 *   troubleshooting low-level touch handling.
 * - If touchscreen initialization fails, the sketch halts to avoid producing
 *   misleading data.
 * - For higher-level coordinate reporting, see the touchscreen serial or
 *   drawing examples instead.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-12
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

// Include Inkplate library
#include "Inkplate.h"

// Create Inkplate object in monochrome mode
Inkplate display(INKPLATE_1BIT);

// The array where to read registers of the touchscreen, used to print them
uint8_t touchRegs[8];

void setup()
{
    // Init Serial so we can see contents of the touchscreen registers
    Serial.begin(115200);

    // Init display (you should call this only once)
    display.begin();

    // Clear the frame buffer
    display.clearDisplay();

    // Display the results
    display.display();

    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.touchscreen.init(true))
    {
        Serial.println("Touchscreen init ok");
    }
    else
    {
        Serial.println("Touchscreen init fail");
        while (true)
            ;
    }

    // Show where the touchscreen 0, 0 position is to the user
    display.fillTriangle(13, 13, 23, 43, 43, 23, BLACK);
    display.setTextSize(3);
    display.setCursor(65, 65);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Periodically check if we can communicate to the touch screen
    if (display.touchscreen.available())
    {
        // Read the raw data of the touch screen registers
        display.touchscreen.getRawData(touchRegs);
        for(int i = 0; i < 8; ++i)
        {
            Serial.print("Reg [");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(touchRegs[i], BIN);
        }

        Serial.println("---------------------------");
        Serial.println();
    }

    delay(1000); // Wait a bit before reading again
}
