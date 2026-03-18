/**
 **************************************************
 * @file        Inkplate6PLUS_Image_Frame_From_Web.ino
 * @brief       Download a JPEG image from the web (following an HTTP redirect),
 *              display it in 3-bit grayscale, then deep-sleep between refreshes.
 *
 * @details     This example turns Inkplate 6PLUS into a simple “web image
 *              frame”. It connects to WiFi, resolves an HTTP redirect from a
 *              starting URL, then downloads and renders the final JPEG directly
 *              from the internet using drawJpegFromWeb().
 *
 *              The sketch uses manual redirect handling: it performs an HTTP GET
 *              to the start URL with redirects disabled, reads the Location
 *              header, normalizes relative / protocol-relative redirects, and
 *              then uses the resolved URL for image download.
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT). After
 *              drawing the image and performing a full refresh, the ESP32 enters
 *              deep sleep for 15 minutes. Deep sleep restarts the ESP32 on wake,
 *              so setup() reruns and a new image is fetched each cycle.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      WiFi access (internet connection required)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6PLUS
 * - Serial settings: 115200 baud (optional; used for debug logs)
 * - WiFi credentials: set ssid/password
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6PLUS/quick-start-guide/
 *
 * How to use:
 * 1) Enter your WiFi SSID/password in the sketch.
 * 2) Upload the sketch to Inkplate 6PLUS.
 * 3) After boot, the device connects to WiFi, resolves the redirect URL, then
 *    downloads and displays a JPEG image.
 * 4) The device deep-sleeps for 15 minutes and repeats after waking.
 *
 * Expected output:
 * - Display: A downloaded image rendered on the e-paper display (grayscale).
 * - Serial Monitor: WiFi join message, resolved URL, HTTP status code, and the
 *   return value of drawJpegFromWeb() (1 = success, 0 = failure).
 *
 * Notes:
 * - Display mode is 3-bit grayscale (INKPLATE_3BIT). Grayscale updates are
 *   slower and use more energy than 1-bit BW.
 * - Deep sleep restarts the ESP32 on wake, so all initialization repeats and a
 *   new image is fetched each cycle.
 * - This example uses plain HTTP for the redirect source. For HTTPS endpoints,
 *   certificate handling may be required depending on the server and library
 *   configuration.
 * - Web image decoding and buffering can be memory-intensive; very large images
 *   or uncommon JPEG encodings may fail due to RAM limits.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2024-03-15
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"

const char ssid[] = "Soldered Electronics";    // Your WiFi SSID
const char *password = "dasduino"; // Your WiFi password

Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.setTextColor(0);

    // Join wifi
    display.connectWiFi(ssid, password);
    Serial.println("joined wifi");
    char url[256];  // Buffer that will hold the final resolved image url

    // Get the final image URL (resolve HTTP redirect) 
    imageUrl(url);
    Serial.println(url);

    // Download and draw the JPEG image directly from the web
    // Function returns 1 on success and 0 on failure
    Serial.println(display.image.drawJpegFromWeb(url, 0, 0, true, false));
    display.display();

    Serial.println("Going to sleep");

    // Activate wakeup timer
    esp_sleep_enable_timer_wakeup(15ll * 60 * 1000 * 1000);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here, as deepsleep restarts esp32
}

void imageUrl(char *out)
{
    HTTPClient http;

    // Starting URL that always redirects to a random image
    const char *startUrl = "http://loremflickr.com/1024/768";

    // Tell HTTPClient to store the "Location" header sowe can read it later
    const char* keys[] = {"Location"};
    http.collectHeaders(keys, 1);

    // Disable automatic redirect following, because we want to manually read the Location header
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);

    // Start HTTP connection
    if (!http.begin(startUrl))
    {
        strcpy(out, startUrl);
        return;
    }

    // Perform HTTP GET request
    int code = http.GET();
    Serial.printf("HTTP code: %d\n", code);

    // Try header() first (works when collectHeaders is used)
    String loc = http.header("Location");

    // Fallback to getLocation() (sometimes works even if header() is empty)
    if (loc.length() == 0)
        loc = http.getLocation();

    Serial.print("Location: ");
    Serial.println(loc);

    if (loc.length() == 0)
    {
        strcpy(out, startUrl);
        http.end();
        return;
    }

    // If the path is relative, make it absolute
    if (loc.startsWith("/"))
        loc = String("http://loremflickr.com") + loc;

    // If the redirect is protocol-relative (redirect starts with "//"), add "http:"
    if (loc.startsWith("//"))
        loc = String("http:") + loc;

    strncpy(out, loc.c_str(), 255);
    out[255] = 0;

    http.end();
}
