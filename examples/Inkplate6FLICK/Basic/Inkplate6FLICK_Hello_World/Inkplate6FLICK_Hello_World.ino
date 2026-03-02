/**
 **************************************************
 * @file        Inkplate6FLICK_Hello_World.ino
 * @brief       Basic "Hello World" example for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates the simplest way to use the Inkplate library by
 *              displaying the text "Hello World!" on the e-paper screen.
 *              The example initializes the display, writes text to the
 *              framebuffer using Adafruit GFX-compatible functions, and then
 *              refreshes the screen to show the content.
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
 * 1) Select "Soldered Inkplate 6FLICK" in Tools -> Board.
 * 2) Upload the sketch to your Inkplate device.
 * 3) After initialization, the text "Hello World!" appears on the display.
 *
 * Expected output:
 * - The e-paper display shows the text "Hello World!" near the top-left corner.
 *
 * Notes:
 * - clearDisplay() clears only the internal framebuffer.
 * - display.display() must be called to refresh the physical e-paper panel.
 * - Inkplate text functions are fully compatible with Adafruit GFX.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * GFX:          https://learn.adafruit.com/adafruit-gfx-graphics-library
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

#include "Inkplate.h" // Include the Inkplate library
Inkplate display(INKPLATE_1BIT); // Create an Inkplate object for Inkplate6 FLICK

void setup() {
    display.begin();             // Initialize the display hardware
    display.clearDisplay();      // Clear the frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10);   // Set the text position to (10, 10) pixels
    display.setTextSize(4);      // Set text size to 4 (default is 1)
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();           // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
