/*
   Inkplate6PLUS_Image_Frame_From_Web example for Soldered Inkplate 6Plus
   For this example you will need only USB cable and Inkplate 6PLUS.
   Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu.
   Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   28 July 2020 by Soldered
*/

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
