/**
 **************************************************
 * @file        Inkplate6COLOR_Full_Screen_Colors.ino
 * @brief       Displays all supported Inkplate 6COLOR panel colors as vertical
 *              full-screen bars.
 *
 * @details     This example demonstrates the basic color capabilities of the
 *              Inkplate 6COLOR e-paper display. The sketch fills the screen
 *              with vertical color bars representing each color supported by
 *              the panel.
 *
 *              The display is divided into equal-width vertical regions and
 *              filled sequentially using the Inkplate color constants. This
 *              provides a quick visual reference for the available display
 *              palette and confirms that the panel is functioning correctly.
 *
 *              The example is useful for testing a newly assembled board,
 *              verifying panel color reproduction, or understanding the
 *              available color set when designing UI layouts.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate 6COLOR in Arduino IDE.
 * 2) Upload the sketch to the board.
 * 3) After initialization, the display is filled with vertical color bars.
 *
 * Expected output:
 * - Display: Full-screen vertical bars showing the Inkplate color palette:
 *   black, white, green, blue, red, yellow, and orange.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode.
 * - The display uses a full refresh to render the color bars.
 * - This example performs a single update and does not change afterward.
 * - Color e-paper refreshes are slower than monochrome displays and should
 *   not be refreshed frequently in typical applications.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2021-09-27
 * @license     GNU GPL V3
 **************************************************/

#include "Inkplate.h"

#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    // Draw a full screen of all colors
    display.fillRect(0, 0, 600 / 7 + 2, 448, INKPLATE_BLACK);
    display.fillRect(1 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_WHITE);
    display.fillRect(2 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_GREEN);
    display.fillRect(3 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_BLUE);
    display.fillRect(4 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_RED);
    display.fillRect(5 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_YELLOW);
    display.fillRect(6 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_ORANGE);

    // Show the Image on the screen
    display.display();
}

void loop()
{
    // Loop forever
}
