/**
 **************************************************
 * @file        Inkplate5v2_Burn_In_Clean.ino
 * @brief       Run a burn-in cleaning cycle to reduce ghosting/burn-in on the
 *              Inkplate 5v2 e-paper panel.
 *
 * @details     This example calls Inkplate's burnInClean() routine to perform a
 *              repeated full-refresh cleaning sequence intended to reduce heavy
 *              ghosting (burn-in) on the e-paper panel. The cleaning process
 *              runs for a configurable number of cycles, with a fixed delay
 *              between cycles to respect e-paper refresh limitations.
 *
 *              The display is used in 1-bit (BW) mode and the cleaning routine
 *              performs multiple full updates, which may take several minutes
 *              depending on CLEAR_CYCLES and CYCLES_DELAY. When the sequence
 *              finishes, a confirmation message is rendered on the screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5v2
 * - Serial settings (if relevant): none
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 *
 * How to use:
 * 1) Set CLEAR_CYCLES to the number of cleaning refresh cycles you want.
 * 2) Set CYCLES_DELAY (ms) between cycles (keep it >= 5000 ms).
 * 3) Upload the sketch and keep the device powered for the entire process.
 * 4) Wait until the screen shows "Clearing done."
 *
 * Expected output:
 * - E-paper: The panel will repeatedly refresh during the cleaning routine.
 *   After completion, the message "Clearing done." is displayed.
 *
 * Notes:
 * - Display mode is 1-bit (BW).
 * - This routine performs many full refreshes; do not interrupt power during
 *   the cleaning sequence.
 * - Use CYCLES_DELAY >= 5 seconds to avoid overstressing the panel and to allow
 *   refresh waveforms to complete properly.
 * - Burn-in/ghosting reduction effectiveness depends on the panel condition and
 *   the content that caused the artifact; multiple runs may be required for
 *   severe cases.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2026-02-19
 * @license     GNU GPL V3
 **************************************************/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate inkplate(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochorme mode

// Nubmer of clear cycles.
#define CLEAR_CYCLES 20

// Delay between clear cycles (in milliseconds)
// NOTE: cycles delay should not be smaller than 5 seconds
#define CYCLES_DELAY 5000

void setup()
{
  inkplate.begin();        // Init library (you should call this function ONLY ONCE)
  inkplate.clearDisplay(); // Clear any data that may have been in (software) frame buffer.

  int cycles = CLEAR_CYCLES;

  // Clean the screen by running the burn in function which starts the cleaning sequence
  inkplate.burnInClean(cycles, CYCLES_DELAY);

  // Print text when clearing is done.
  inkplate.setTextSize(4);
  inkplate.setCursor(100, 100);
  inkplate.print("Clearing done.");
  inkplate.display();
}

void loop()
{
  // Empty...
}