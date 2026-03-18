/**
 **************************************************
 * @file        Inkplate2_Show_Pictures_From_Web.ino
 * @brief       Download JPG/BMP/PNG images over WiFi and display them on the
 *              Inkplate 2 e-paper display.
 *
 * @details     This example demonstrates how to fetch images from the Internet
 *              using WiFi and HTTP, then render them on Inkplate 2. It shows
 *              two workflows:
 *              - Direct URL drawing via display.image.draw() for JPG/PNG/BMP
 *              - Manual download using HTTPClient and rendering a BMP stream
 *                via drawBitmapFromWeb()
 *
 *              The display runs in 1-bit (black/white) mode. Optional dithering
 *              can be enabled when drawing to convert grayscale/color sources
 *              into a 1-bit dithered image. Image size must fit the Inkplate 2
 *              resolution (212x104 px) and format/encoding must be supported.
 *
 *              After showing several images with delays, WiFi is turned off and
 *              the ESP32 enters deep sleep. Deep sleep restarts the ESP32 on
 *              wake; this sketch does not configure a wake source.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - Serial Monitor: 115200 baud (optional, for connection logs)
 * - WiFi:           set ssid/password
 * - Image URLs:     update the URLs if you want to load your own images
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your WiFi SSID and password.
 * 2) Upload the sketch to Inkplate 2.
 * 3) Open Serial Monitor at 115200 baud to watch WiFi connection status.
 * 4) The sketch connects to WiFi, downloads and displays multiple images,
 *    then turns WiFi off and enters deep sleep.
 *
 * Expected output:
 * - Display: downloaded images rendered full-screen (212x104), each followed by
 *   a full refresh.
 * - Serial Monitor: WiFi connection progress and a "Going to sleep.." message.
 *
 * Notes:
 * - Display mode is 1-bit (BW). This example uses full refresh (display()).
 * - Image constraints:
 *   - Max size: 212x104 px (larger images may fail or render incorrectly).
 *   - JPG: Baseline DCT with Huffman coding (common "baseline" JPEG).
 *   - BMP: Windows BMP, 1/4/8/24-bit, uncompressed (no RLE compression).
 *   - PNG: generally supported; if an image fails, re-save it with an editor.
 * - Large 24-bit images can be slow to download and decode.
 * - Dithering improves appearance for grayscale/color sources but increases
 *   processing time.
 * - Deep sleep restarts the ESP32; no wakeup source is configured here, so a
 *   reset or power cycle is required to run again.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-28
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "HTTPClient.h" // Include library for HTTPClient
#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "WiFi.h"       // Include library for WiFi
Inkplate display;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = "Soldered Electronics";     // Your WiFi SSID
const char password[] = "dasduino"; // Your WiFi password

void setup()
{
    Serial.begin(115200);   // Init Serial communication.
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display

    // Set settings for error printing
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.setTextColor(INKPLATE2_BLACK);

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected!");

    // Draw the first image from web.
    // Make sure the link is complete and correct (contains https:// or http://).
    // The example image is a monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares.
    // Fourth parameter will dither the image, but this image is already dithered
    // so it is not needed to dither it again while drawing.
    display.clearDisplay();

    if (!display.image.draw("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                           "master/examples/Inkplate2/Advanced/WEB_WiFi/"
                           "Inkplate2_Show_Pictures_From_Web/cat_dithered.jpg",
                           0, 0, false, false))
    {
        // If is something failed (wrong url or unsupported format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
    }
    display.display(); // Refresh the display
    delay(8000);       // Wait a little bit

    // Draw the second image from web, this time using a HTTPClient to fetch the response manually.
    // Full color 24 bit images are large and take a long time to load, will take around 20 secs.
    HTTPClient http;
    // Set parameters to speed up the download process.
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    http.begin("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
               "master/examples/Inkplate2/Advanced/WEB_WiFi/"
               "Inkplate2_Show_Pictures_From_Web/car.bmp");

    // Check response code.
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        // Get the response length and make sure it is not 0.
        int32_t len = http.getSize();
        if (len > 0)
        {
            if (!display.image.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len, true, false))
            {
                // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
                // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
                // compression!
                display.println("Image open error");
            }
            display.display(); // Refresh the display.
        }
        else
        {
            // If something goes wrong, print out the error message and refresh the display.
            display.println("Invalid response length");
            display.display();
        }
    }
    else
    {
        // Print out the error message and refresh the display.
        display.println("HTTP error");
        display.display();
    }

    display.clearDisplay(); // Clear the frame buffer
    delay(8000);            // Wait a little bit

    // Try to load image and display it on e-paper at position X=0, Y=0
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. fourth parameter will dither the
    // image.
    if (!display.image.draw("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                           "master/examples/Inkplate2/Advanced/WEB_WiFi/"
                           "Inkplate2_Show_Pictures_From_Web/mountain.png",
                           0, 0, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.clearDisplay();
        display.println("Image open error");
    }
    display.display(); // Refresh the display
    http.end();        // Close HTTP connection.

    WiFi.mode(WIFI_OFF); // Turn off the WiFi

    // Go to deep sleep
    Serial.println("Going to sleep..");
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

void loop()
{
    // Nothing...
}
