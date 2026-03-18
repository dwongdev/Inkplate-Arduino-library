/**
 **************************************************
 * @file        Inkplate2_Google_Calendar.ino
 * @brief       Display upcoming Google Calendar events on Inkplate 2 using
 *              Google Calendar API and periodic deep sleep updates.
 *
 * @details     This example connects Inkplate 2 to WiFi, synchronizes time via
 *              NTP, fetches events from a public Google Calendar using the
 *              Google Calendar REST API, and renders them on the e-paper
 *              display using a simple GUI layer.
 *
 *              The workflow:
 *              - Connect to WiFi (with timeout handling)
 *              - Configure system time via NTP (configTime)
 *              - Request calendar data using calendar ID and API key
 *              - Render events using a GUI helper
 *              - Enter deep sleep and wake periodically to refresh data
 *
 *              The ESP32 enters deep sleep after each update and wakes every
 *              TIME_TO_SLEEP seconds (default: 10 minutes). Because deep sleep
 *              resets the ESP32, execution always restarts from setup().
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection, Google Cloud API key, public Google Calendar
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - WiFi:           set ssid/password
 * - Calendar ID:    set calendarID (public calendar)
 * - API Key:        set apiKey (Google Cloud project)
 * - Timezone:       set timeZone (UTC offset)
 * - NTP Server:     set ntpServer if needed
 * - Refresh period: set TIME_TO_SLEEP (seconds)
 *
 * Before use:
 * - Enable "Google Calendar API" in Google Cloud Console.
 * - Make the selected calendar public.
 * - Generate and copy an API key into the sketch.
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your WiFi credentials.
 * 2) Set your public Google Calendar ID and API key.
 * 3) Adjust timeZone for your location.
 * 4) Upload the sketch to Inkplate 2.
 * 5) After boot, the device connects, fetches events, displays them,
 *    then enters deep sleep.
 * 6) The screen refreshes automatically every TIME_TO_SLEEP seconds.
 *
 * Expected output:
 * - Display: formatted list of upcoming calendar events.
 * - On WiFi error: dedicated WiFi error screen.
 * - On API error: error message shown via GUI.
 *
 * Notes:
 * - Display mode is 1-bit (BW). Full refresh is used when rendering the GUI.
 * - Deep sleep restarts the ESP32; all logic must remain in setup().
 * - Google API errors:
 *   - 403 Forbidden → API not enabled.
 *   - 404 Not Found → calendar not public or wrong ID.
 * - API keys should be protected in production environments.
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
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "Soldered-testingPurposes";
const char *password = "Testing443";

// --- User Info ---
String calendarID = "yourpublicgooglecalid@group.calendar.google.com";
String apiKey = "yourapikey";

int timeZone = 2; // timeZone is the number in (UTC + number) in your time zone | UTC + 2 for Osijek, UTC - 4 for New York City
const char  *ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate;
calendarData calendar;
NetworkFunctions network(calendarID, apiKey);
Gui gui(inkplate);

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000 // Convert microseconds to seconds
#define TIME_TO_SLEEP 600      // Sleep time: 600 seconds = 10 minutes

void setup()
{
  Serial.begin(115200);    // Initialize serial monitor for debugging
  inkplate.begin();        // Start the Inkplate display
  inkplate.clearDisplay(); // Clear the screen
  inkplate.setRotation(1); // Portrait mode | if it's upside down do setRotation(3);

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
    configTime(timeZone * 3600, 0, ntpServer);
    // Fetch and display calendar
    if (network.fetchCalendar(&calendar))
    {
      Serial.println("Calendar loaded.");
      gui.showCalendar(&calendar);
    }
    else
    {
      Serial.println("Failed to load calendar.");
      gui.showError("Failed to load calendar.");
    }
  }
  // Sleep to save power; wakes every 10 minutes
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer 
  esp_deep_sleep_start();                                        // Put ESP32 into deep sleep.
}

void loop()
{
  // Should remain empty, main logic is in the setup();
}