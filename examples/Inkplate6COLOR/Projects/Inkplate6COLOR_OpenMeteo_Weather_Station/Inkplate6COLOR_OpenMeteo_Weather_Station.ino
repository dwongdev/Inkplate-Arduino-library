/**
 **************************************************
 * @file        Inkplate6COLOR_OpenMeteo_Weather_Station.ino
 * @brief       Connects to Wi-Fi, fetches weather data from Open-Meteo, and
 *              displays it on Inkplate 6COLOR.
 *
 * @details     This example demonstrates a connected weather dashboard for
 *              Inkplate 6COLOR using the Open-Meteo API. The sketch connects
 *              to a Wi-Fi network, requests current weather data for a
 *              configured location, and renders the results on the e-paper
 *              display in a user-friendly weather station layout.
 *
 *              The displayed data is based on the configured latitude,
 *              longitude, and timezone. Optional user-facing values such as
 *              username and city name can also be shown on the screen for a
 *              more personalized layout, but they are not required by the API
 *              request itself.
 *
 *              The example supports both metric and imperial units. By
 *              default, metric units are used, but the sketch can be switched
 *              to imperial mode through a configuration flag.
 *
 *              This is a practical starting point for always-on home weather
 *              displays, desk dashboards, and low-power Wi-Fi information
 *              panels.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      WiFi
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Enter your Wi-Fi SSID and password in the sketch
 * - Set the correct timezone, latitude, and longitude for your location
 * - Optionally set myUsername and myCity for display personalization
 * - Set metricUnits to true or false depending on the preferred unit system
 * - Serial settings (if relevant)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your Wi-Fi credentials in the sketch.
 * 2) Set the correct timezone, latitude, and longitude for your location.
 * 3) Optionally customize the displayed username and city.
 * 4) Choose metric or imperial units in the sketch configuration.
 * 5) Upload the sketch to Inkplate 6COLOR.
 * 6) After connecting to Wi-Fi, the board requests weather data from the
 *    Open-Meteo API and renders it on the display.
 *
 * Expected output:
 * - Display: A weather station screen showing current weather information for
 *   the configured location.
 * - Serial Monitor: Optional debug, connection, or API status output if used
 *   in the sketch.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode with full refreshes.
 * - This example depends on internet connectivity and successful API access.
 * - Weather API responses and parsing may use significant RAM depending on the
 *   amount of requested data and the implementation.
 * - Timezone, latitude, and longitude must match the intended location or the
 *   displayed weather data may be misleading.
 * - Frequent refreshes increase power use; for battery-powered projects,
 *   combine this workflow with timed deep sleep where appropriate.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "Soldered-testingPurposes";
const char *password = "Testing443";

// --- User and Location Info ---
String myUsername = "Username"; // User's name to be displayed on screen
String myCity = "Osijek";       // City name for weather data
int timeZone =
    2; // timeZone is the number in (UTC + number) in your time zone UTC + 2 for Osijek, UTC - 4 for New York City
float latitude =  45.5550;  // Latitude of the city
float longitude = 18.6955; // Longitude of the city

bool metricUnits = true;  // set this to false if you wish to use Imperial units

const char* ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate; // Create Inkplate display object (3-bit mode for partial grayscale)
NetworkFunctions network;                  // Network utility for weather fetching
NetworkFunctions::UserInfo userInfo;       // Structure to hold user and device info (battery, last updated, etc.)
WeatherData weatherData;          // Structure to hold fetched weather data
Gui gui(inkplate);                // Drawing visuals and info

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000 // Convert microseconds to seconds
#define TIME_TO_SLEEP  1800    // Sleep time: 1800 seconds = 30 minutes

// --- Main Setup: Runs Once on Boot ---
void setup()
{
    Serial.begin(115200);    // Initialize serial monitor for debugging
    inkplate.begin();        // Start the Inkplate display
    inkplate.clearDisplay(); // Clear the screen

    // Attempt to connect to WiFi
    const unsigned long timeout = 30000;
    unsigned long startTime = millis();
    bool connected = false;

    while (!connected && (millis() - startTime) < timeout)
    {
        connected = inkplate.connectWiFi(ssid, password, 10, true);
    }

    // If WiFi failed, display error message
    if (!connected)
    {
        gui.wifiError();
    }
    else
    {
        configTime(timeZone * 3600, 0, ntpServer); // Set local time via NTP server
        // Gather battery and city info
        gui.voltage = inkplate.readBattery();
        userInfo.city = myCity;
        userInfo.username = myUsername;
        userInfo.useMetric = metricUnits;

        // Fetch weather data for specified coordinates
        network.fetchWeatherData(&weatherData, &userInfo, &latitude, &longitude);

        // Display if weather API call fails
        if (userInfo.apiError)
        {
            gui.apiError();
        }
        else
        {
            // Success: draw UI and data
            gui.drawBackground();
            gui.displayWeatherData(&weatherData, &userInfo);
        }
    }

    // Sleep to save power; wakes every 30 minutes
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *
                                  uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 30mins here
    esp_deep_sleep_start();                        // Put ESP32 into deep sleep.
}

void loop()
{
    // Nothing to do here - main logic runs once in setup()
}

