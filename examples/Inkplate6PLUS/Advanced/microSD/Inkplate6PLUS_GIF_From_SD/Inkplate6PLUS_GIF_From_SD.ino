/**
 **************************************************
 * @file        Inkplate6PLUS_GIF_From_SD.ino
 * @brief       Play a GIF animation from the SD card on Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates loading a GIF file from a FAT-formatted SD card
 *              and playing it back on the Inkplate 6PLUS e-paper display using
 *              partial updates. Each pixel is converted to black/white with
 *              a fixed threshold (no dithering) since dithering noise would
 *              flicker differently frame to frame.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable, microSD card
 * - Extra:      SD card containing a GIF file named "cat_gif.gif"
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6PLUS
 * - SD card format: FAT / FAT32
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Copy a GIF file to the root of a FAT-formatted SD card and rename it
 *    to "cat_gif.gif".
 * 2) Insert the SD card into the Inkplate.
 * 3) Upload the sketch to Inkplate 6PLUS.
 * 4) The GIF plays back on the e-paper screen.
 *
 * Expected output:
 * - "cat_gif.gif" loops forever on the display, centered on the screen.
 *
 * Notes:
 * - Partial update (and therefore GIF playback) only works in INKPLATE_1BIT
 *   mode, hence the display object below is created with INKPLATE_1BIT.
 * - e-paper partial refresh takes far longer than a typical GIF frame delay
 *   (tens to hundreds of ms per refresh vs ~100ms/frame in the file), so
 *   actual playback speed is limited by the panel, not by the GIF itself.
 * - The driver forces a full refresh every N partial updates (fullRefreshEveryFrames
 *   argument, defaults to 20 here) to clear partial-update ghosting; pass 0 to disable
 *   forced full refreshes entirely.
 * - leaveOn (last argument, defaults to true) keeps the panel powered between frames
 *   instead of power-cycling it on every partialUpdate() call; pass false to power the
 *   panel down after each frame instead.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2026-07-01
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library, GIF playback requires 1-bit mode

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    // display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Push the cleared frame buffer to the panel with a full refresh

    // Init SD card. Display if SD card is init properly or not.
    if (display.sdCardInit())
    {
        // Play "cat_gif.gif" from the SD card root, centered on the screen.
        // loop = true: keep replaying the file forever (until reset/power-cycled).
        if (!display.gif.playGifFromSd("cat_gif.gif", E_INK_WIDTH / 2 - 125, E_INK_HEIGHT / 2 - 125, false, true, 20, true))
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
