/**
 **************************************************
 * @file        Inkplate2_Hello_World.ino
 * @brief       Minimal example: print "Hello World!" on the Inkplate 2
 *              e-paper display.
 *
 * @details     This is the simplest Inkplate 2 example demonstrating basic
 *              initialization and text rendering. The sketch initializes the
 *              display, clears the internal framebuffer, sets cursor position,
 *              text size, and text color, then prints "Hello World!" and
 *              performs a full refresh to update the e-paper panel.
 *
 *              Inkplate 2 operates in 1-bit mode (black/white with optional
 *              red plane). This example uses standard monochrome text drawing
 *              and a single full refresh.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - Serial settings: none
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Inkplate 2 in Tools -> Board.
 * 2) Upload the sketch.
 * 3) After reset, the display refreshes and shows "Hello World!".
 *
 * Expected output:
 * - Display: "Hello World!" printed near the top-left corner.
 *
 * Notes:
 * - Display mode is 1-bit (BW). This example uses a full refresh (display()).
 * - clearDisplay() clears only the framebuffer in RAM; display() is required
 *   to update the physical e-paper panel.
 * - No deep sleep is used; the message remains on screen after execution.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
Inkplate display;     // Create an Inkplate object in monochrome mode

void setup() {
    display.begin();           // Initialize the display hardware
    display.clearDisplay();    // Clear the software frame buffer (does NOT clear the physical screen)
    display.setCursor(10, 10); // Set the text position to (10, 10) pixels
    display.setTextSize(2);    // Set text size to 2 (default is 1)
    display.setTextColor(1);   // Set text color to black (1 = black, 0 = white)
    display.print("Hello World!"); // Print "Hello World!" at the set position
    display.display();         // Refresh the e-paper display to show changes
}

void loop() {
    // No code needed here for this example
}
