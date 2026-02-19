/*
  Inkplate6FLICK_Burn_In_Clean example for Soldered Inkplate 6FLICK
  For this example you will need only USB cable and Inkplate 6FLICK.
  Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
  Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
  https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

  This example will try to remove heavy burn-in visible on the panel.
  Set number of refresh / clear cycles and upload the program.

  Want to learn more about Inkplate? Visit www.inkplate.io
  Looking to get support? Write on our forums: https://forum.soldered.com/
  19 February 2026 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
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