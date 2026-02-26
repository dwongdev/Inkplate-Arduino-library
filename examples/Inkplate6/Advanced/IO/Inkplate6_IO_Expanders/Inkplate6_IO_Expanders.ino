/**
 **************************************************
 * @file        Inkplate6_IO_Expanders.ino
 * @brief       Internal and external IO expander control example for Soldered Inkplate 6.
 *
 * @details     Demonstrates how to control GPIO pins on both the internal and
 *              external IO expanders available on Inkplate 6. The example
 *              alternates blinking an LED connected to the external IO expander
 *              (IO Expander 2) and an LED connected to the internal IO expander
 *              (IO Expander 1), showing correct usage and addressing for each.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable, 2× LED, 2× 330 Ω resistors
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6/quick-start-guide/
 *
 * How to use:
 * 1) Connect an LED + 330 Ω resistor to P1-7 (GPB7) on IO Expander 2 (external).
 * 2) Connect another LED + 330 Ω resistor to P1-7 (GPB7) on IO Expander 1 (internal).
 * 3) Upload the sketch to Inkplate 6.
 * 4) Observe alternating blinking between external and internal LEDs.
 *
 * Expected output:
 * - External IO expander LED blinks for 5 seconds.
 * - Internal IO expander LED blinks for 5 seconds.
 * - Sequence repeats continuously.
 *
 * Notes:
 * - External IO expander pins are all free to use by default.
 * - Internal IO expander has restrictions:
 *   - DO NOT use GPA0–GPA7 or GPB0.
 *   - Use only pins 9–15 (P1-1 to P1-7).
 * - Using restricted pins may permanently damage the display.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-05
 * @license     GNU GPL V3
 **************************************************/

#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"

// We are going to use pin P1-7 (GPB7)
// GPA0 = 0 ... GPA7 = 7
// GPB0 = 8 ... GPB7 = 15
#define LED_PIN IO_PIN_B7

Inkplate display(INKPLATE_1BIT);

void setup()
{
    display.begin();

    // Configure LED pin on both IO expanders
    display.externalIO.pinMode(LED_PIN, OUTPUT); // IO Expander 2
    display.internalIO.pinMode(LED_PIN, OUTPUT); // IO Expander 1
}

void loop()
{
    // External IO Expander (IO Expander 2) 
    for (int i = 0; i < 5; i++)
    {
        display.externalIO.digitalWrite(LED_PIN, HIGH);
        delay(500);
        display.externalIO.digitalWrite(LED_PIN, LOW);
        delay(500);
    }

    delay(1000);

    // Internal IO Expander (IO Expander 1)
    for (int i = 0; i < 5; i++)
    {
        display.internalIO.digitalWrite(LED_PIN, HIGH, IO_INT_ADDR);
        delay(500);
        display.internalIO.digitalWrite(LED_PIN, LOW, IO_INT_ADDR);
        delay(500);
    }

    delay(2000);
}