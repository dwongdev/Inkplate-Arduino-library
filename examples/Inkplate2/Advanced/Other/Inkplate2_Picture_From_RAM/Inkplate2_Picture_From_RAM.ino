/**
 **************************************************
 * @file        Inkplate2_Picture_From_RAM.ino
 * @brief       Display multiple images stored in RAM, then enter deep sleep.
 *
 * @details     This example demonstrates how to render monochrome bitmap
 *              images stored in program memory (included as header files) and
 *              copied into RAM at runtime. Three pre-converted 1-bit images
 *              (212x104 pixels) are displayed sequentially, each shown for
 *              10 seconds using a full e-paper refresh.
 *
 *              After displaying the third image, the ESP32 enters deep sleep.
 *              Since deep sleep resets the microcontroller, execution always
 *              starts from setup() after a reset or power cycle.
 *
 *              Images must be converted to Inkplate-compatible 1-bit format
 *              using the Soldered Image Converter tool and included as
 *              header files.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Three 1-bit bitmap images converted with Soldered Image Converter
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - Image size: 212x104 pixels (full Inkplate 2 resolution)
 * - Include picture1.h, picture2.h, picture3.h in the sketch folder
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Convert three monochrome images (212x104 px) using the Soldered Image Converter.
 * 2) Place the generated header files in the sketch folder and include them.
 * 3) Upload the sketch to Inkplate 2.
 * 4) The device displays each image for 10 seconds, then enters deep sleep.
 *
 * Expected output:
 * - Display: picture1 → picture2 → picture3 (each with full refresh).
 * - After the third image, the device enters deep sleep and stops execution.
 *
 * Notes:
 * - Display mode is 1-bit (BW). Only full refresh (display()) is used.
 * - Large images consume RAM; ensure images match the native resolution
 *   (212x104) and are properly converted.
 * - Deep sleep restarts the ESP32 on wake; this example does not configure
 *   a wakeup source, so reset or power cycle is required to run again.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-11-24
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

#include "picture1.h" // Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter and have resolution of 212x104 picels
#include "picture2.h"
#include "picture3.h" // This are headers in which are saved pictures that needs to be stored in RAM for showing.
                      /* 
                        In order to convert your images into a format compatible with Inkplate
                        use the Soldered Image Converter available at:
                        http://soldered.com/image-converter
                      */


Inkplate display; // Create display object

void setup()
{
    Serial.begin(115200);
    Serial.print("Sketch begun!");

    
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.image.draw(picture1, 0, 0, 212,
                      104); // Display picture from RAM  at location X=0, Y=0. It is also needed to specify width and
                            // height of picture (212x104 in this case).
    display.display();      // Refresh the screen with new picture
    delay(10000);           // Wait a bit (in this case 10 seconds)

    display.clearDisplay();                      // Clear frame buffer of display
    display.image.draw(picture2, 0, 0, 212, 104); // Display selected picture at location X=0, Y=0.
    display.display();                           // Refresh the screen with new picture
    delay(10000);                                // Wait a bit (in this case 10 seconds)

    display.clearDisplay();                      // Clear frame buffer of display
    display.image.draw(picture3, 0, 0, 212, 104); // Display selected picture at location X=0, Y=0.
    display.display();                           // Refresh the screen with new picture

    // Put ESP32 into deep sleep. Program stops here
    esp_deep_sleep_start();
}


void loop()
{
    // Nothing! 
}
