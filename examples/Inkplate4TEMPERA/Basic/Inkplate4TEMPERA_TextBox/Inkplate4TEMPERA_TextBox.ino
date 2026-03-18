/**
 **************************************************
 * @file        Inkplate4TEMPERA_TextBox.ino
 * @brief       Demonstrates rendering wrapped text inside bounded text boxes
 *              using the Inkplate TextBox helper.
 *
 * @details     This example shows how to use the drawTextBox() function on the
 *              Inkplate 4 TEMPERA in 1-bit (BW) mode. It renders a longer text
 *              string inside defined rectangular areas, automatically handling
 *              word wrapping and truncation.
 *
 *              Two text boxes are demonstrated:
 *              - A basic text box using default parameters and the built-in
 *                font.
 *              - An advanced text box using a custom font, adjusted vertical
 *                spacing, font size control, and optional border disabling.
 *
 *              If the text exceeds the vertical bounds of the box, it is
 *              truncated and ends with three dots (...) to indicate that not
 *              all content is visible.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 4 TEMPERA
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select the Inkplate 4 TEMPERA board and upload the sketch.
 * 2) After boot, two text boxes are rendered on the display.
 * 3) Compare default TextBox behavior with the custom-font example.
 *
 * Expected output:
 * - E-paper: Two rectangular text areas showing wrapped text. The second uses
 *   a larger custom font and adjusted spacing.
 *
 * Notes:
 * - Display mode is 1-bit (BW).
 * - TextBox automatically wraps words; it does not hyphenate.
 * - When using custom fonts, RAM usage increases significantly.
 * - Some fonts are drawn bottom-to-top; a vertical offset may be required to
 *   align text correctly (as shown in this example).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/

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
