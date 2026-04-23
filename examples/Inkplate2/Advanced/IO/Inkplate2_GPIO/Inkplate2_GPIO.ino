/**
 **************************************************
 * @file        Inkplate2_GPIO.ino
 * @brief       Basic GPIO control example: blink an external LED using
 *              ESP32 GPIO on Inkplate 2.
 *
 * @details     This example demonstrates how to use general-purpose I/O (GPIO)
 *              pins available on the Inkplate 2 header. An external LED is
 *              connected to ESP32 GPIO14 through a current-limiting resistor.
 *              The sketch configures the selected pin as an OUTPUT and toggles
 *              it every second, creating a visible blink.
 *
 *              The e-paper display is used only to show instructions and runs
 *              in 1-bit (black/white) mode with a single full refresh during
 *              setup.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      LED, ~330 Ω resistor, jumper wires, breadboard
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - Connect LED anode to GPIO14 through a 330 Ω resistor
 * - Connect LED cathode to GND
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Wire the LED to GPIO14 with a current-limiting resistor.
 * 2) Select Inkplate 2 in Tools and upload the sketch.
 * 3) After reset, read instructions on the display.
 * 4) Observe the LED blinking once per second (1 s ON, 1 s OFF).
 *
 * Expected output:
 * - Display: "Blink example" and wiring instructions.
 * - Hardware: LED connected to GPIO14 blinks continuously.
 *
 * Notes:
 * - Display mode is 1-bit (BW). Only a full refresh (display()) is used.
 * - Use only GPIO pins that are exposed on the Inkplate 2 header and not
 *   reserved for internal hardware.
 * - Ensure correct polarity of the LED and always use a resistor to avoid
 *   damaging the GPIO pin.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-05-10
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"

// LED is connected to ESP32 GPIO 14
#define PIN_LED 14

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.setTextSize(1);    // Set text size
    display.setCursor(10, 20); // Set cursor position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.println("Blink example");
    display.setCursor(10, 35); // Set cursor position
    display.println("Connect LED to ESP32 GPIO14 and LED will blink once every two seconds.");

    // Display to screen
    display.display();

    // Set LED GPIO to be output pin
    pinMode(PIN_LED, OUTPUT);
}

void loop()
{
    digitalWrite(PIN_LED, HIGH); // Turn on LED
    delay(1000);                 // Wait a bit
    digitalWrite(PIN_LED, LOW);  // Turn off LED
    delay(1000);                 // Wait a bit
}
