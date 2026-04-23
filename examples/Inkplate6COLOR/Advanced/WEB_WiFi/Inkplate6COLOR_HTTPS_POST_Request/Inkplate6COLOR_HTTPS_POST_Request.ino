/**
 **************************************************
 * @file        Inkplate6COLOR_HTTPS_POST_Request.ino
 * @brief       Connects Inkplate 6COLOR to Wi-Fi and sends periodic HTTPS POST
 *              requests with a JSON payload.
 *
 * @details     This example demonstrates how to use Inkplate 6COLOR for secure
 *              web API communication over HTTPS. The sketch connects to a Wi-Fi
 *              network, builds a small JSON document with ArduinoJson, and
 *              sends that JSON payload to a remote HTTPS endpoint using
 *              HTTPClient with a WiFiClientSecure transport.
 *
 *              The example uses the JSONPlaceholder test API, which is useful
 *              for learning and debugging POST request workflows because it
 *              returns realistic API-style responses without storing real data.
 *              This makes it a safe demo target for testing request formatting,
 *              headers, and response handling.
 *
 *              In this version, the TLS client uses client.setInsecure(), which
 *              disables certificate validation. That is convenient for demos,
 *              but it should not be used in production or security-sensitive
 *              deployments. For real services, use proper certificate validation
 *              or certificate pinning that matches the target host.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      Wi-Fi connection
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial Monitor: 115200 baud
 * - Enter your Wi-Fi SSID and password in the sketch
 * - Update the HTTPS API URL if you want to test against a different endpoint
 * - Add API keys, authentication headers, or extra JSON fields if required by
 *   your real service
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your Wi-Fi credentials in the sketch.
 * 2) Upload the example to Inkplate 6COLOR.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) The board connects to Wi-Fi and initializes a secure HTTP client.
 * 5) In loop(), the sketch builds a JSON payload and sends an HTTPS POST
 *    request to the configured API endpoint.
 * 6) The HTTP status code and returned response body are printed to Serial.
 * 7) The process repeats after the configured delay.
 *
 * Expected output:
 * - Serial: Wi-Fi connection progress, assigned IP address, HTTP status code,
 *   and the JSON response returned by the test API.
 * - Display: Not used in this example.
 *
 * Notes:
 * - Display mode: not used in this example (no screen updates occur).
 * - HTTPS transport is used, but client.setInsecure() disables certificate
 *   validation. Demo only; not recommended for production.
 * - For production APIs, use proper CA certificates or certificate pinning,
 *   and ensure the certificate matches the target host.
 * - JSON payload size affects RAM usage, so larger requests may require more
 *   careful buffer sizing on embedded targets.
 * - Repeated POST intervals should respect the target API's rate limits and
 *   service policies.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-02
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Include needed libraries
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>

// Enter your WiFi credentials
const char *ssid = "Soldered Electronics";
const char *pass = "dasduino";

// Specify the API URL to send a POST request
const char *apiUrl = "https://jsonplaceholder.typicode.com/posts";

// Specify the delay time between 2 POST requests in milliseconds
#define DELAY_BETWEEN_REQUESTS 10000

// Create objects from included libraries
WiFiClientSecure client;
HTTPClient http;
Inkplate display; 

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init the display
    display.begin();
    
    // Connect to WiFi
    WiFi.begin(ssid, pass);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected to WiFi with IP address ");
    Serial.println(WiFi.localIP());

    // Use https but don't use a certificate
    client.setInsecure();
}

void loop()
{
    // Init HTTP client with secure wifi client and the API URL
    http.begin(client, apiUrl);

    // Add header. In your case maybe you must add more depending on your API
    http.addHeader("Content-Type", "application/json");

    // Create a JSON document for serializing data
    const size_t CAPACITY = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<CAPACITY> doc;

    // Convert the JSON document to a JSON object in order to add data
    JsonObject object = doc.to<JsonObject>();

    // Add data in the JSON object
    object["title"] = "Hello Inkplate";

    // Serialize the JSON object with your data to a JSON document for sending
    char jsonOutput[128];
    serializeJson(doc, jsonOutput);

    // Send the POST request and save returned code
    int httpCode = http.POST(String(jsonOutput));

    // If the code is smaller than 0, something went wrong
    if (httpCode > 0)
    {
        // Get the string returned by the HTTPS request and print the response
        String payload = http.getString();
        Serial.println("Status code: " + String(httpCode));
        Serial.println(payload);

        // End connection
        http.end();
    }
    else
    {
        Serial.println("Error on HTTPS request");
    }

    // Wait for sending next request
    delay(DELAY_BETWEEN_REQUESTS);
}
