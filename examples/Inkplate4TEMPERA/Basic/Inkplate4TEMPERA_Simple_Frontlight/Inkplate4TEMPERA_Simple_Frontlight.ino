/**
 **************************************************
 * @file        Inkplate4TEMPERA_Simple_Frontlight.ino
 * @brief       Control the Inkplate 4 TEMPERA frontlight brightness via the
 *              Serial Monitor.
 *
 * @details     This example enables the Inkplate 4 TEMPERA frontlight circuit
 *              and lets you change brightness interactively over Serial. Send
 *              '+' to increase brightness and '-' to decrease it. The current
 *              level is printed back to the Serial Monitor as a value from
 *              0 to 63.
 *
 *              Sending 's' runs a simple "light show" that ramps the frontlight
 *              up and down several times to demonstrate the full brightness
 *              range.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 4 TEMPERA
 * - Serial Monitor: 115200 baud, Newline: "No line ending" recommended
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select the Inkplate 4 TEMPERA board and upload the sketch.
 * 2) Open the Serial Monitor at 115200 baud.
 * 3) Send '+' to increase brightness (max 63).
 * 4) Send '-' to decrease brightness (min 0).
 * 5) Send 's' to run the brightness ramp demo.
 *
 * Expected output:
 * - Serial: Prints the current brightness as "Frontlight:<value>/63" after a
 *   change or after the demo completes.
 * - Hardware: Frontlight intensity changes immediately.
 *
 * Notes:
 * - This sketch runs the display in 1-bit (BW) mode, but it does not draw
 *   anything on the e-paper panel; only the frontlight is controlled.
 * - Brightness is clamped to the valid range (0–63).
 * - The 's' demo uses delays; during the ramp animation Serial input is not
 *   processed.
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
