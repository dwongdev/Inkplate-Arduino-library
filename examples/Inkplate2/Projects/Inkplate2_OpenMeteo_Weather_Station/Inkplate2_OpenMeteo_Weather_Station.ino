/**
 **************************************************
 * @file        Inkplate2_OpenMeteo_Weather_Station.ino
 * @brief       Open-Meteo weather station: fetch current conditions/forecast
 *              over WiFi, render a dashboard, then deep sleep and refresh
 *              periodically.
 *
 * @details     This example demonstrates a simple "weather station" dashboard
 *              on Inkplate 2 using the Open-Meteo API. On each boot, the sketch
 *              connects to WiFi, sets local time using NTP (configTime), then
 *              requests weather data for the configured latitude/longitude via
 *              a NetworkFunctions helper. The resulting data is rendered by a
 *              GUI layer.
 *
 *              To add variety, the sketch alternates between two different UI
 *              layouts on each wake cycle using an RTC-persisted counter
 *              (bootCount). After drawing the screen, the ESP32 enters deep
 *              sleep and wakes every TIME_TO_SLEEP seconds (default: 10 minutes)
 *              to refresh the data.
 *
 *              Deep sleep resets the ESP32 each time it wakes up, so execution
 *              always starts from setup(). The display retains the last image
 *              while the device is asleep.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access (Open-Meteo + NTP)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - WiFi:           set ssid/password
 * - Timezone:       set timeZone (UTC offset)
 * - Location:       set latitude/longitude (API), myCity/myUsername (display)
 * - Units:          set metricUnits (true = metric, false = imperial)
 * - NTP server:     set ntpServer if needed
 * - Refresh period: set TIME_TO_SLEEP (seconds)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your WiFi credentials.
 * 2) Set timeZone and your location coordinates (latitude/longitude).
 * 3) Optionally set myUsername and myCity for display labels.
 * 4) Choose units with metricUnits (true/false).
 * 5) Upload the sketch to Inkplate 2.
 * 6) The device fetches weather, displays the dashboard, then sleeps and
 *    refreshes automatically every TIME_TO_SLEEP seconds.
 *
 * Expected output:
 * - Display: a weather dashboard (layout alternates between two designs on each
 *   wake due to bootCount).
 * - On WiFi failure: a WiFi error screen.
 * - On API failure: an API error screen.
 *
 * Notes:
 * - Display mode is 1-bit (BW). Full refresh is used for dashboard rendering.
 * - Deep sleep restarts the ESP32; keep all logic in setup() and leave loop()
 *   empty.
 * - RTC_DATA_ATTR bootCount persists across deep sleep but resets on power loss
 *   or full reset/flash.
 * - Weather accuracy and availability depend on Open-Meteo and network access.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "Soldered Electronics";
const char *password = "dasduino";

// --- User and Location Info ---
String myUsername = "Username"; // User's name to be displayed on screen
String myCity = "Osijek";       // City name for weather data
int timeZone =
    2; // timeZone is the number in (UTC + number) in your time zone UTC + 2 for Osijek, UTC - 4 for New York City
float latitude = 45.5550;  // Latitude of the city
float longitude = 18.6955; // Longitude of the city

bool metricUnits = true;  // set this to false if you wish to use Imperial units

const char* ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate;
NetworkFunctions network;                  // Network utility for weather fetching
NetworkFunctions::UserInfo userInfo;       // Structure to hold user info
WeatherData weatherData;          // Structure to hold fetched weather data
Gui gui(inkplate);                // Drawing visuals and info

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000 // Convert microseconds to seconds
#define TIME_TO_SLEEP  600   // Sleep time: 600 seconds = 10 minutes
RTC_DATA_ATTR int bootCount = 0; // This variable persists across deep sleep resets

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
        // Gather user info
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
            if (bootCount % 2 == 1)
            {
                gui.displayWeatherData(&weatherData, &userInfo); // Odd boots
                bootCount++;
            }
            else
            {
                gui.displayWeatherData2(&weatherData, &userInfo);// Even boots
                bootCount++;
            }
        }
    }

    // Sleep to save power; wakes every 10 minutes
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *
                                  uS_TO_S_FACTOR); // Activate wake-up timer
    esp_deep_sleep_start();                        // Put ESP32 into deep sleep.
}

void loop()
{
    // Nothing to do here - main logic runs once in setup()
}

