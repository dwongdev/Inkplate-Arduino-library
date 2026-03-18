/**
 **************************************************
 * @file        Inkplate5_HTTPS_POST_Request.ino
 * @brief       HTTPS POST request example for Soldered Inkplate 5.
 *
 * @details     Demonstrates how to connect Inkplate 5 to a WiFi network and
 *              send an HTTPS POST request with a JSON payload. The example uses
 *              the JSONPlaceholder fake REST API to test request/response flow
 *              and prints the HTTP status code and response body to the Serial
 *              Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable
 * - Extra:      Stable WiFi Internet connection
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5
 * - Serial settings: 115200 baud
 * - Enter your WiFi credentials (ssid, pass) in the code
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/5/quick-start-guide/
 *
 * How to use:
 * 1) Enter your WiFi SSID and password in the sketch.
 * 2) Upload the sketch to Inkplate 5.
 * 3) Open Serial Monitor (115200 baud).
 * 4) The board connects to WiFi and periodically sends an HTTPS POST request.
 * 5) Observe the returned status code and response payload in Serial Monitor.
 *
 * Expected output:
 * - Inkplate display shows a short message prompting to open Serial Monitor.
 * - Serial Monitor shows WiFi connection status, HTTP status code, and response body.
 *
 * Notes:
 * - This example uses HTTPS without certificate validation via client.setInsecure().
 *   This is convenient for demos/testing but is not recommended for production use.
 * - JSONPlaceholder is a fake API: the response looks real, but data is not persisted.
 * - For real APIs you may need authentication headers (API keys, tokens) and additional fields.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-02
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include needed libraries
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"

// Enter your WiFi credentials
const char *ssid = "";
const char *pass = "";

// Specify the API URL to send a POST request
const char *apiUrl = "https://jsonplaceholder.typicode.com/posts";

// Specify the delay time between 2 POST requests in milliseconds
#define DELAY_BETWEEN_REQUESTS 10000 // Here is 10 seconds

// Create objects from included libraries
WiFiClientSecure client;
HTTPClient http;
Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init library (you should call this function ONLY ONCE)
    display.begin();

    // Clear the display and print message
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(6);
    display.println("HTTPS POST Request example");
    display.println();

    display.setTextSize(4);
    display.println("Open Serial Monitor at 115200 baud rate to see what's happening");
    display.display();

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
    const size_t CAPACITY = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<CAPACITY> doc;

    // Convert the JSON document to a JSON object in order to add data
    JsonObject object = doc.to<JsonObject>();

    // Add data in the JSON object
    // Add some title
    object["title"] = "Hello Inkplate";

    // Add some random number from 0 to 100
    // In the same way, you can add some sensor readings or something similar
    object["sensor_reading"] = random(100);

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
