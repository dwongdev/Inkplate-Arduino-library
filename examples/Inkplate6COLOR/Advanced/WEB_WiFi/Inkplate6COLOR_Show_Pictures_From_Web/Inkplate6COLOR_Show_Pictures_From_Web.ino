/**
 **************************************************
 * @file        Inkplate6COLOR_Show_Pictures_From_Web.ino
 * @brief       Downloads and displays image files from the web on Inkplate
 *              6COLOR over Wi-Fi.
 *
 * @details     This example demonstrates how to connect Inkplate 6COLOR to a
 *              Wi-Fi network, download image files from remote URLs, and render
 *              them on the e-paper display. It shows multiple web image
 *              workflows using the Inkplate image loader, including 1-bit BMP,
 *              multi-bit BMP, and JPEG files fetched directly over HTTP/HTTPS.
 *
 *              The sketch first connects to Wi-Fi, then attempts to load and
 *              display several remote images in sequence. It demonstrates the
 *              use of optional image-processing parameters such as dithering and
 *              color inversion, which can be useful when converting source
 *              images for e-paper rendering.
 *
 *              Web image examples are convenient for connected dashboards,
 *              signage, and remote content display, but they depend on network
 *              reliability, server availability, supported image formats, and
 *              available RAM. BMP files are generally the most predictable
 *              option for embedded rendering, while JPEG decoding may require
 *              more processing and memory.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      WiFi
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Enter your Wi-Fi SSID and password in the sketch
 * - Serial settings: optional if you want to monitor connection progress
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your Wi-Fi credentials in the sketch.
 * 2) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the example.
 * 3) On startup, the board connects to the configured Wi-Fi network.
 * 4) The sketch downloads several remote images in sequence and displays each
 *    one on the e-paper panel.
 * 5) If an image cannot be opened or decoded, an error message is shown on the
 *    display.
 *
 * Expected output:
 * - Display: A sequence of downloaded images rendered full-screen or near
 *   full-screen, followed by any image-load error messages if a fetch or decode
 *   fails.
 * - Serial: Wi-Fi connection progress messages, if Serial is monitored.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode with full refreshes.
 * - Remote image rendering is limited by available RAM, decode complexity, and
 *   network stability.
 * - BMP/JPEG support depends on library capabilities. BMP is typically the
 *   safest format for predictable embedded workflows.
 * - For BMP files, supported bit depths and uncompressed formats are important.
 * - Image dimensions should fit the Inkplate 6COLOR panel resolution
 *   (600x448). Oversized images may fail or require preprocessing.
 * - The optional invert parameter can help when source bitmaps have reversed
 *   black/white polarity.
 * - The optional dither parameter can improve visual conversion for some images
 *   at the cost of additional processing.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-23
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "HTTPClient.h" //Include library for HTTPClient
#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "WiFi.h"       //Include library for WiFi
Inkplate display;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = ""; // Your WiFi SSID
const char *password = "";     // Your WiFi password

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    Serial.print("Connecting to WiFi...");

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi OK! Downloading...");

    // Draw the first image from web.
    // Monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares. Forth parameter will dither the image. Photo taken by: Roberto Fernandez
    if (!display.image.draw("https://varipass.org/neowise_mono.bmp", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
        display.display();
    }
    display.display();

    if (!display.image.draw("https://varipass.org/neowise.bmp", 0, 0, true, false))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
        display.display();
    }
    display.display();

    display.clearDisplay();
    delay(3000);

    // Try to load image and display it on e-paper at position X=0, Y=100
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. forth parameter will dither the
    // image.
    if (!display.image.draw("https://varipass.org/destination.jpg", 0, 25, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.println("Image open error");
        display.display();
    }
    display.display();

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}
