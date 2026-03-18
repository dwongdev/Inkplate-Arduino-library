/**
 **************************************************
 * @file        Inkplate4TEMPERA_Hello_World.ino
 * @brief       Basic “Hello World” example demonstrating text rendering on
 *              the Inkplate 4TEMPERA e-paper display.
 *
 * @details     This is the simplest possible Inkplate 4TEMPERA example. It initializes
 *              the display, clears the internal framebuffer, sets cursor
 *              position, text size, and text color, then prints the string
 *              "Hello World!" to the screen.
 *
 *              The example demonstrates the standard workflow:
 *              1) display.begin() – initialize hardware
 *              2) clearDisplay() – clear framebuffer
 *              3) draw text using print()
 *              4) display() – perform a full screen refresh
 *
 *              Note that clearDisplay() only clears the RAM framebuffer. The
 *              physical e-paper panel is updated only after calling display().
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4TEMPERA
 * - Hardware:   Inkplate 4TEMPERA, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate4TEMPERA
 * - Serial settings (if relevant): none
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select "Soldered Inkplate4TEMPERA" in Tools -> Board.
 * 2) Upload the sketch to the device.
 * 3) After reset, the text "Hello World!" appears in the top-left corner.
 *
 * Expected output:
 * - The message "Hello World!" rendered in black text on the e-paper display.
 *
 * Notes:
 * - Display mode: 3-bit grayscale (INKPLATE_3BIT).
 * - This example performs a single full refresh using display().
 * - No deep sleep is used; loop() remains empty.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display(INKPLATE_3BIT); // Create an Inkplate object for Inkplate6 FLICK

void setup() {
    display.begin();             // Initialize the display hardware
    display.clearDisplay();      // Clear the frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10);   // Set the text position to (10, 10) pixels
    display.setTextSize(3);      // Set text size to 3 (default is 1)
    display.setTextColor(BLACK); // Set text color to black (default is white)
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();           // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
