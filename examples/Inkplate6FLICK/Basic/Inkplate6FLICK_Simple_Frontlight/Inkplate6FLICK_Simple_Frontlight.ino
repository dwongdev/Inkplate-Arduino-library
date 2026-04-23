/**
 **************************************************
 * @file        Inkplate6FLICK_Simple_Frontlight.ino
 * @brief       Frontlight control demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to enable and control the frontlight on the
 *              Inkplate 6FLICK. The example allows adjusting frontlight
 *              brightness through the Serial Monitor by sending characters
 *              that increase or decrease intensity. A small light animation
 *              ("lightshow") can also be triggered.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK with integrated frontlight, USB cable
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6FLICK.
 * 2) Open the Serial Monitor at 115200 baud.
 * 3) Send the following characters to control brightness:
 *      '+' → Increase frontlight intensity
 *      '-' → Decrease frontlight intensity
 *      's' → Run a simple frontlight animation ("lightshow")
 *
 * Expected output:
 * - Frontlight turns on during setup.
 * - Brightness level changes when '+' or '-' is sent via Serial Monitor.
 * - Current brightness value (0–63) is printed in the Serial Monitor.
 * - The 's' command runs a short brightness sweep animation.
 *
 * Notes:
 * - Frontlight brightness range is 0–63.
 * - display.frontlight.setState(true) enables the frontlight driver circuit.
 * - display.frontlight.setState(value) sets brightness level.
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
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h" //Include Inkplate library

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate class

int b = 31; // Variable that holds intensity of the frontlight

void setup()
{
    Serial.begin(115200);    // Set up a serial communication of 115200 baud
    display.begin();         // Init Inkplate library
    display.frontlight.setState(true); // Enable frontlight circuit
    display.frontlight.setState(b); // Set frontlight intensity
}

void loop()
{
    if (Serial.available()) // Change frontlight value by sending "+" sign into serial monitor to increase frontlight or
                            // "-" sign to decrese frontlight
                            // try to find hidden lightshow ;)
    {
        bool change = false;    // Variable that indicates that frontlight value has changed and intessity has to be updated
        char c = Serial.read(); // Read incomming serial data

        if (c == '+' && b < 63) // If is received +, increase frontlight
        {
            b++;
            change = true;
        }
        if (c == '-' && b > 0) // If is received -, decrease frontlight
        {
            b--;
            change = true;
        }

        if (c == 's')
        {
            for (int j = 0; j < 4; ++j)
            {
                for (int i = 0; i < 64; ++i)
                {
                    display.frontlight.setState(i);
                    delay(30);
                }

                for (int i = 63; i >= 0; --i)
                {
                    display.frontlight.setState(i);
                    delay(30);
                }
            }

            change = true;
        }

        if (change) // If frontlight valuse has changed, update the intensity and show current value of frontlight
        {
            display.frontlight.setState(b);
            Serial.print("Frontlight:");
            Serial.print(b, DEC);
            Serial.println("/63");
        }
    }
}
