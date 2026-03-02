/**
 **************************************************
 * @file        Inkplate6PLUS_Read_Temperature.ino
 * @brief       On-board temperature sensor reading example for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to read temperature data from the on-board
 *              temperature sensor integrated inside the TPS65186 e-paper PMIC.
 *              This sensor is intended primarily for internal compensation
 *              and basic monitoring. It is a simple (basic) temperature sensor
 *              and should not be considered highly accurate or suitable for
 *              precise temperature measurements.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6PLUS
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6PLUS.
 * 2) The program reads the temperature from the onboard PMIC sensor.
 * 3) The measured value can be displayed or printed to Serial.
 *
 * Expected output:
 * - Approximate temperature reading reported by the TPS65186 sensor.
 *
 * Notes:
 * - The TPS65186 PMIC includes a basic internal temperature sensor.
 * - This sensor is not ultra-precise and is not a replacement for a
 *   dedicated external temperature sensor.
 * - Intended use is system monitoring and waveform compensation.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
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
#include "tempSymbol.h" // Include .h file that contains byte array for temperature symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on tempSymbol.h tab in Arduino IDE
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
    int temperature = display.readTemperature();            // Read temperature from on-board temperature sensor
    display.clearDisplay();                                 // Clear everything in frame buffer of e-paper display
    display.image.draw(tempSymbol, 100, 100, 38, 79, BLACK); // Draw temperature symbol at position X=100, Y=100
    display.setCursor(150, 120);
    display.print(temperature, DEC); // Print temperature
    display.print('C');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
