/**
 **************************************************
 * @file        Inkplate6FLICK_Scrolling_Text.ino
 * @brief       Scrolling text demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to create a simple horizontal scrolling text
 *              animation on the Inkplate 6FLICK e-paper display. The example
 *              repeatedly redraws text at a shifting X position and uses
 *              partial screen updates for smooth and fast refresh.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Libraries:  Inkplate library
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6FLICK.
 * 2) A line of text will scroll horizontally across the screen.
 * 3) When the text reaches the right edge, it restarts from the left side.
 *
 * Expected output:
 * - Text smoothly scrolling across the e-paper display.
 * - Continuous animation using fast partial updates.
 *
 * Notes:
 * - Partial updates only work in 1-bit (black & white) mode.
 * - Keeping the display power enabled during partial updates improves speed.
 * - The scrolling speed can be adjusted by changing the delay value.
 *
 * Docs:         https://docs.soldered.com/inkplate
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************
 */

#include "Inkplate.h"   // Include Inkplate library to the sketch

// Create Inkplate object and set display mode to 1-bit (black & white).
// 1-bit mode supports partial updates and is the fastest refresh mode.
Inkplate inkplate(INKPLATE_1BIT);

// Text that will scroll across the screen
String text = "This is a scrolling text example!";

// Starting X position (negative value means text starts off-screen on the left)
int x = 0;

// Vertical position of the text on the screen
int y = 300;

void setup()
{
    Serial.begin(115200);

    // Initialize Inkplate hardware and library.
    // This function should only be called once in setup().
    inkplate.begin();

    // Set text size (larger number = larger font)
    inkplate.setTextSize(3);

    // Set text color (BLACK for drawing on white background)
    inkplate.setTextColor(BLACK);
}

void loop()
{
    
    inkplate.clearDisplay();    // Clear the framebuffer
    inkplate.setCursor(x, y);   // Set cursor position where the text will be drawn

    inkplate.print(text);   // Draw the text into the framebuffer

    // Perform a partial refresh of the screen.
    // Second parameter (1) = keep display power ON for faster next updates
    // NOTE: Partial update only works in black and white mode
    inkplate.partialUpdate(0, 1);

    // Move text to the right by 10 pixels every loop iteration
    x += 10;

    // If text moves past the right edge of the screen
    // reset it to start again from the left side
    if (x > E_INK_WIDTH)
    {
        x = 0;
    }

    // Small delay controls scrolling speed
    delay(80);
}