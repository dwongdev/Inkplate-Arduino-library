/**
 **************************************************
 * @file        Inkplate10_Read_Touchpads.ino
 * @brief       Reads the three built-in capacitive touchpads on Inkplate 10
 *              and updates a counter displayed on the screen.
 *
 * @details     This example demonstrates how to use the capacitive touchpads
 *              integrated on the Inkplate 10 PCB. The board includes three
 *              touch-sensitive pads labeled 1, 2, and 3 that can be used as
 *              simple input controls for user interaction.
 *
 *              The sketch continuously reads the state of the touchpads using
 *              display.touchpad.read(). Each pad performs a different action
 *              on a displayed counter value:
 *
 *              - Pad 1 decreases the displayed number
 *              - Pad 2 resets the number to zero
 *              - Pad 3 increases the number
 *
 *              To improve refresh speed and reduce panel wear, the example uses
 *              partial display updates in 1-bit mode. After several partial
 *              updates, a full refresh is automatically performed to maintain
 *              display quality.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate10 in Arduino IDE and upload the sketch.
 * 2) After initialization, a number appears in the center of the display.
 * 3) Touch the pads on the bottom of the PCB:
 *    - Touch pad 1 to decrease the number.
 *    - Touch pad 2 to reset the number to zero.
 *    - Touch pad 3 to increase the number.
 * 4) The display updates each time a pad is touched.
 *
 * Expected output:
 * - Display: A large number that changes according to touchpad input.
 * - Display: Symbols "-", "0", and "+" printed above the touchpads as
 *   visual indicators of their function.
 *
 * Notes:
 * - Display mode: 1-bit black-and-white (INKPLATE_1BIT).
 * - Partial updates are used for faster refresh. After ~20 partial updates,
 *   a full refresh is performed automatically.
 * - Capacitive touchpads are sensitive to environment and grounding and may
 *   not work reliably through thick enclosures or insulating materials.
 * - The function touchpad.read() returns 1 when the pad is touched and 0
 *   otherwise.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if defined(ARDUINO_INKPLATE10)

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

int number = 0; // Variable that stores our number
int n = 0;      // Variable that keeps track on how many times display is partially updated

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextSize(5);             // Set text scaling to five (text will be five times bigger)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
    displayNumber();                    // Call our function to display nubmer on screen
}

void loop()
{
    if (display.touchpad.read(1))
    { // Check if first pad has been touched. If it is, decrement the number and refresh the screen.
        number--;
        displayNumber();
    }

    if (display.touchpad.read(2))
    { // If you touched second touchpad, set number to zero and refresh screen by calling our displayNumber() function
        number = 0;
        displayNumber();
    }

    if (display.touchpad.read(3))
    { // If you touched third touchpad, incerement the number and refresh the screen.
        number++;
        displayNumber();
    }
    delay(100); // Wait a little bit between readings.
}

// Function that will write you number to screen
void displayNumber()
{
    display.clearDisplay();      // First, lets delete everything from frame buffer
    display.setCursor(580, 392); // Set print cursor at X=580, Y=392 (roughly in the middle of the screen)
    display.print(number, DEC);  // Print the number
    display.setCursor(480, 790); // Set new print position (right above first touchpad)
    display.print('-');          // Print minus sign
    display.setCursor(580, 790); // Set new print position (right above second touchpad)
    display.print('0');          // Print zero
    display.setCursor(680, 790); // Set new print position (right above third touchpad)
    display.print('+');          // Print plus sign
    if (n > 20)
    { // Chech if screen has been partially refreshed more than 20 times. If it is, do a full refresh. If is not, do a
      // partial refresh
        display.display();
        n = 0;
    }
    else
    {
        display.partialUpdate();
        n++;
    }
}
#else
void setup()
{

}
void loop()
{

}
#endif