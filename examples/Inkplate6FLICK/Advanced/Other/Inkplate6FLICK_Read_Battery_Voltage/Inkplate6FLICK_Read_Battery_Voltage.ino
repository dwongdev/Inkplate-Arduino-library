/**
 **************************************************
 * @file        Inkplate6FLICK_Read_Battery_Voltage.ino
 * @brief       Battery voltage reading demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to measure and display the connected Li-ion
 *              battery voltage using the built-in battery measurement circuit
 *              on Inkplate 6FLICK. The measured voltage is shown on the
 *              e-paper display together with a battery icon.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable,
 *               3.6–3.7V Li-ion/LiPo battery with 2-pin JST connector
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Connect a compatible Li-ion/LiPo battery to the JST connector.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) The screen will display the measured battery voltage.
 * 4) Voltage updates every 10 seconds.
 *
 * Expected output:
 * - Battery icon rendered on screen.
 * - Measured battery voltage displayed in volts (e.g., 3.92V).
 *
 * Notes:
 * - Uses display.readBattery() to read battery voltage.
 * - Example runs in 1-bit (black & white) mode.
 * - Ensure correct battery polarity before connecting.
 *
 * Docs:         https://docs.soldered.com/inkplate
 *
 * @author      Soldered Electronics
 * @date        2026-02-26
 * @license     GNU GPL V3
 **************************************************
 */

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "battSymbol.h" // Include .h file that contains byte array for battery symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
}

void loop()
{
    float voltage = display.readBattery();                   // Read battery voltage
    display.clearDisplay();                                  // Clear everything in frame buffer of e-paper display
    display.image.draw(battSymbol, 100, 100, 106, 45, BLACK); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
