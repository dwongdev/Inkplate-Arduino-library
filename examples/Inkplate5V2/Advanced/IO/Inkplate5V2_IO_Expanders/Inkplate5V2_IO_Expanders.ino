/**
 **************************************************
 * @file        Inkplate5v2_IO_Expanders.ino
 * @brief       Internal expander control example for Soldered Inkplate 5v2.
 *
 * @details     Demonstrates how to control GPIO pins on both the internal IO
 *              expander available on Inkplate 5v2. The example
 *              alternates blinking an LED connected to the internal IO expander
 *              , showing correct usage and addressing.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable, 2× LED, 2× 330 Ω resistors
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5v2
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 *
 * How to use:
 * 1) Connect an LED + 330 Ω resistor to P1-7 (GPB7) on IO Expander 2 (external).
 * 2) Connect another LED + 330 Ω resistor to P1-7 (GPB7) on IO Expander 1 (internal).
 * 3) Upload the sketch to Inkplate 5v2.
 * 4) Observe alternating blinking between external and internal LEDs.
 *
 * Expected output:
 * - Internal IO expander LED blinks for 5 seconds.
 * - Sequence repeats continuously.
 *
 * Notes:
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

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

// We are going to use pin P1-7.
// Remember! P0-0 = 0, P0-1 = 1, ..., P0-7 = 7, P1-0 = 8, P1-1 = 9, ..., P1-7 = 15.
#define LED_PIN IO_PIN_B7

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    display.internalIO.pinMode(LED_PIN, OUTPUT); // Set P1-7 to output. On that pin, we sholud connect LED with current
                                    // limiting resistor and specify that we want use internal IO expander
}

void loop()
{
    display.internalIO.digitalWrite(LED_PIN, LOW, IO_INT_ADDR);  // Set output to low (LED does not light up)
    delay(1000);                                        // Wait for one second
    display.internalIO.digitalWrite(LED_PIN, HIGH, IO_INT_ADDR); // Set output to high (LED lights up)
    delay(1000);                                       // Wait for one second
}
