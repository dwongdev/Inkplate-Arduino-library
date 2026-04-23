/**
 **************************************************
 * @file        Inkplate6PLUS_Read_Battery_Voltage.ino
 * @brief       Battery voltage reading example for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to read the connected Li-ion/Li-Po battery
 *              voltage using Inkplate’s built-in battery measurement circuitry.
 *              The example shows how to obtain the battery voltage value in
 *              software and display or process it as needed.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable, 3.6–4.2 V Li-ion/Li-Po battery (JST connector)
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6PLUS
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Connect a supported Li-ion/Li-Po battery to the Inkplate battery connector.
 * 2) Upload the sketch to Inkplate 6PLUS.
 * 3) The battery voltage is read and can be displayed or logged by the sketch.
 *
 * Expected output:
 * - Measured battery voltage value reported by the program.
 *
 * Notes:
 * - Battery voltage reading is enabled through the onboard circuitry.
 * - Accuracy depends on battery condition and load.
 * - Battery reading typically requires enabling the battery measurement path
 *   in hardware (see Inkplate documentation).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 * Image tool:   https://tools.soldered.com/tools/image-converter/
 *
 * @author      Soldered
 * @date        2023-01-23
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "battSymbol.h" // Include .h file that contains byte array for battery symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
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
