/**
 **************************************************
 * @file        Inkplate6FLICK_HTTPS_POST_Request.ino
 * @brief       HTTPS POST request demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to connect Inkplate 6FLICK to a Wi-Fi network
 *              and send an HTTPS POST request. The example uses the
 *              JSONPlaceholder test API to send JSON data and prints the HTTP
 *              status code and server response to the Serial Monitor.
 *              The Inkplate display shows basic instructions to open the
 *              Serial Monitor for logs.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 *
 * Configuration:
 * - Set ssid/pass to your Wi-Fi credentials
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Enter your Wi-Fi SSID and password in ssid/pass.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) The device sends an HTTPS POST request periodically and prints:
 *    - HTTP status code
 *    - Server response payload
 *
 * Expected output:
 * - Inkplate display shows “HTTPS POST Request example” and a Serial Monitor hint.
 * - Serial Monitor shows Wi-Fi connection info, HTTP status code, and JSON response.
 *
 * Notes:
 * - This example uses client.setInsecure(), which disables certificate validation.
 *   For production use, validate the server certificate or use certificate pinning.
 * - JSONPlaceholder is a fake API for testing; it returns a response but does not
 *   persist data like a real backend.
 * - Modify the JSON payload and headers to match your real API requirements.
 *
 * Docs:         https://docs.soldered.com/inkplate
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************
 */

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

// Include needed libraries
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>

// Enter your WiFi credentials
const char *ssid = "";
const char *pass = "";

// Specify the API URL to send a POST request
const char *apiUrl = "https://jsonplaceholder.typicode.com/posts";

// Specify the delay time between 2 POST requests in milliseconds
#define DELAY_BETWEEN_REQUESTS 10000

// Create objects from included libraries
WiFiClientSecure client;
HTTPClient http;
Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochrome mode

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init the display

    display.begin();

    // Clear the display and print message
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(8);
    display.printf("HTTPS POST \nRequest example\n");

    display.setTextSize(6);
    display.printf("\nOpen Serial Monitor at \n115200 baud rate to see \nwhat's happening");
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
