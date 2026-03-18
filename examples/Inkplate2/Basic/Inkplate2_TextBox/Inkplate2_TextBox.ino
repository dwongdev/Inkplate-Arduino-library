/**
 **************************************************
 * @file        Inkplate2_TextBox.ino
 * @brief       TextBox usage example for Soldered Inkplate 2.
 *
 * @details     Demonstrates how to use the drawTextBox() function to render
 *              multi-line text inside a defined rectangular area.
 *              The example shows:
 *              - A basic TextBox with default parameters.
 *              - A fully customized TextBox using a custom font,
 *                text scaling, spacing, and optional border.
 *
 *              If a word does not fit at the end of a row, it automatically
 *              wraps to the next line. If the text exceeds the lower boundary
 *              of the box, it ends with three dots (...) to indicate that
 *              not all text is displayed.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Custom font file (e.g. Roboto_Light_36.h)
 *
 * Configuration:
 * - Tools -> Board -> "Soldered Inkplate 2"
 *
 * Expected result:
 * - Two text boxes rendered on the screen:
 *   1) Default TextBox
 *   2) Custom styled TextBox using Roboto font
 *
 * Notes:
 * - This example runs in 1-bit (black & white) mode.
 * - Some custom fonts are drawn bottom-to-top and may require
 *   a vertical offset for correct positioning.
 * - Always call display.display() after drawing operations
 *   to update the physical e-paper screen.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display; // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

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
    display.drawTextBox(10,10,200,90,text);

    // Display both text boxes
    display.display();
}

void loop()
{
    // Nothing...
}