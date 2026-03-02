/**
 **************************************************
 * @file        Inkplate6FLICK_Touch_Registers.ino
 * @brief       Touchscreen register debug example for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to read raw touchscreen controller register
 *              data from the Inkplate 6FLICK. When a touch event occurs, the
 *              sketch reads the raw register values from the touchscreen
 *              controller and prints them to the Serial Monitor for debugging
 *              or development purposes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6FLICK.
 * 2) Open the Serial Monitor at 115200 baud.
 * 3) Touch the screen to generate touchscreen events.
 * 4) Raw touchscreen register values will be printed in the Serial Monitor.
 *
 * Expected output:
 * - Serial Monitor displays raw touchscreen register values when the screen
 *   is touched.
 * - A small marker and label indicating the (0,0) screen position appear
 *   on the display.
 *
 * Notes:
 * - touchscreen.available() checks if a touch event occurred.
 * - touchscreen.getRawData() reads raw controller register data.
 * - Touchscreen coordinates are automatically adjusted when the display
 *   rotation changes.
 * - This example is mainly intended for debugging and low-level touchscreen
 *   development.
 *
 * Docs:         https://docs.soldered.com/inkplate
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************
 */

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"
Inkplate display(INKPLATE_1BIT);

uint8_t touchRegs[8];

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    display.begin();
    display.clearDisplay();
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

    // NOTE!!!
    // Touchscreen cooridinates are automatically swapped and adjusted when screen is rotated
    display.fillTriangle(10, 10, 20, 40, 40, 20, BLACK);
    display.setTextSize(3);
    display.setCursor(60, 60);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Check if there is any touch detected
    if (display.touchscreen.available())
    {
        display.touchscreen.getRawData(touchRegs);
        for(int i = 0; i < 16; ++i)
        {
            Serial.print("Reg ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(touchRegs[i], BIN);
        }

        Serial.println("---------------------------");
        Serial.println();
    }
    delay(50);
}
