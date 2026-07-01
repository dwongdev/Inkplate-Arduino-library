/**
 **************************************************
 * @file        Inkplate5V2_GIF_From_SD.ino
 * @brief       Play a GIF animation from the SD card on Soldered Inkplate 5V2.
 *
 * @details     Demonstrates loading a GIF file from a FAT-formatted SD card
 *              and playing it back on the Inkplate 5V2 e-paper display using
 *              partial updates. Each pixel is converted to black/white with
 *              a fixed threshold (no dithering) since dithering noise would
 *              flicker differently frame to frame.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5V2
 * - Hardware:   Inkplate 5V2, USB cable, microSD card
 * - Extra:      SD card containing a GIF file named "giftest.gif"
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5 V2
 * - SD card format: FAT / FAT32
 *
 * How to use:
 * 1) Copy a GIF file to the root of a FAT-formatted SD card and rename it
 *    to "giftest.gif".
 * 2) Insert the SD card into the Inkplate.
 * 3) Upload the sketch to Inkplate 5V2.
 * 4) The GIF plays back on the e-paper screen.
 *
 * Expected output:
 * - "giftest.gif" loops forever on the display, top left corner at (0, 0).
 *
 * Notes:
 * - Partial update (and therefore GIF playback) only works in INKPLATE_1BIT
 *   mode, hence the display object below is created with INKPLATE_1BIT.
 * - e-paper partial refresh takes far longer than a typical GIF frame delay
 *   (tens to hundreds of ms per refresh vs ~100ms/frame in the file), so
 *   actual playback speed is limited by the panel, not by the GIF itself.
 * - The driver forces a full refresh every 60 partial updates by default to clear
 *   partial-update ghosting; change the last argument of playGifFromSd() to
 *   tune that, or pass 0 to disable forced full refreshes entirely.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2026-07-01
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library, GIF playback requires 1-bit mode

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Push the cleared frame buffer to the panel with a full refresh

    // Init SD card. Display if SD card is init properly or not.
    if (display.sdCardInit())
    {
        // Play "giftest.gif" from the SD card root, top left corner at (0, 0).
        // loop = true: keep replaying the file forever (until reset/power-cycled).
        if (!display.gif.playGifFromSd("giftest.gif", 0, 0, false, true))
        {
            // If something failed (missing file, unsupported/corrupt GIF), write an error message on the screen.
            display.setTextColor(BLACK);
            display.setTextSize(3);
            display.println("GIF open error");
            display.display();
        }
    }
    else
    {
        // If SD card init was not successful, display error on screen.
        display.setTextColor(BLACK);
        display.setTextSize(3);
        display.println("SD Card error!");
        display.display();
    }

    // Turn off the MOSFET that powers the SD card
    display.sdCardSleep();
}

void loop()
{
    // Nothing...
}
