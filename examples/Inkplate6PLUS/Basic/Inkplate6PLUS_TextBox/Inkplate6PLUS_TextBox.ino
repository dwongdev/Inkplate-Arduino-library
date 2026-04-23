/**
 **************************************************
 * @file        Inkplate6PLUS_TextBox.ino
 * @brief       TextBox rendering demo for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to use the drawTextBox() function on
 *              Inkplate 6PLUS to render multiline text inside a defined
 *              rectangular area. The example shows both basic usage with
 *              default parameters and advanced usage with custom font,
 *              scaling, spacing, and optional border control.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Libraries:  Inkplate library
 * - Extra:      Custom font header (e.g., Roboto_Light_36.h)
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6PLUS.
 * 2) The display will show two text boxes:
 *    - One using default parameters.
 *    - One using a custom font and additional configuration options.
 *
 * Expected output:
 * - Text automatically wrapped inside defined rectangular areas.
 * - Words move to the next line if they do not fit.
 *
 * Notes:
 * - Custom fonts may require vertical offset adjustments.
 * - This example runs in 1-bit (black & white) mode.
 * - drawTextBox() supports font scaling, spacing, and border options.
 *
 * Docs:         https://docs.soldered.com/inkplate
 *
 * @author      Soldered Electronics
 * @date        2026-02-26
 * @license     GNU GPL V3
 **************************************************
 */

#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "Roboto_Light_36.h"
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

// Define the text you will show in the text box
const char* text="This is an example of a text written in a textbox. When a word doesn't fit into the current row, it goes to the next one."\
" If the text reaches the lower bound, it ends with three dots (...) to mark that the text isnt displayed fully";

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    // Create a text box without any optional parameters
    // x0- x coordinate of upper left corner
    // y0- y coordinate of upper left corner
    // x1- x coordinate of bottom right corner
    // y1- y coordinate of bottom right corner
    // text - text we want to display
    display.drawTextBox(100,100,300,300,text);

    // Create a text box with all parameters
    // x0- x coordinate of upper left corner
    // y0- y coordinate of upper left corner
    // x1- x coordinate of bottom right corner
    // y1- y coordinate of bottom right corner
    // text - text we want to display
    // textSizeMultiplier - by what factor we want to enlarge the size of a font
    // font - address of selected custom font
    // verticalSpacing - how many pixels between each row of text
    // showBorder - Create a visible rectangle around the box
    // fontSize - size of the used font in pt
    int offset=32; // Note - some custom fonts are drawn from bottom-to-top which requires an offset, use an offset that best suits the font you use 
    display.drawTextBox(400,100+offset,600,300,text,1,&Roboto_Light_36,27,false,36);

    // Display both text boxes
    display.display();
}

void loop()
{
    // Nothing...
}
