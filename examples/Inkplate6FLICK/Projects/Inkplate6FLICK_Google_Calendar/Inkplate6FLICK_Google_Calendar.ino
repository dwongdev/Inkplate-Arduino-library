/**
 **************************************************
 * @file        Inkplate6FLICK_Google_Calendar.ino
 * @brief       Fetch and display a public Google Calendar on Inkplate 6FLICK
 *              over WiFi, then deep-sleep between updates.
 *
 * @details     This example connects Inkplate 6FLICK to WiFi, synchronizes time
 *              using NTP, downloads events from a *public* Google Calendar using
 *              the Google Calendar API (API key + public calendar ID), and
 *              renders the agenda using a simple GUI.
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT), which
 *              is suitable for UI layouts with icons/boxes/shading. After
 *              updating the screen, the ESP32 enters deep sleep for a fixed
 *              interval to save power. Note that deep sleep restarts the ESP32
 *              on wake, so setup() runs again each cycle.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      WiFi access, Google API key, public Google Calendar ID
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6FLICK
 * - Serial settings: 115200 baud (optional; used for debugging)
 * - WiFi credentials: set ssid/password
 * - Timezone: set timeZone (UTC offset hours) and optionally ntpServer
 * - Google Calendar:
 *   - calendarID must point to a *public* calendar (e.g. ...@group.calendar.google.com)
 *   - apiKey must be a valid Google Cloud API key with Google Calendar API enabled
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Make your Google Calendar public:
 *    - Google Calendar settings -> your calendar -> "Access permissions for events"
 *      -> enable public access.
 * 2) Get the Calendar ID:
 *    - Google Calendar settings -> your calendar -> "Integrate calendar"
 *      -> copy "Calendar ID".
 * 3) Create a Google Cloud API key and enable Google Calendar API for the project.
 * 4) Enter ssid/password, calendarID, apiKey, and timeZone in the sketch.
 * 5) Upload the sketch. The device will connect, sync time, fetch events, draw
 *    the calendar view, then deep-sleep and refresh periodically.
 *
 * Expected output:
 * - Display: Calendar/agenda view with events for the configured public calendar,
 *   or an on-screen error message if WiFi/API fetch fails.
 * - Serial Monitor: Status messages (WiFi/calendar loaded/failed).
 *
 * Notes:
 * - Display mode is 3-bit grayscale (INKPLATE_3BIT). Grayscale refreshes are
 *   slower than 1-bit BW and consume more energy per update.
 * - Deep sleep restarts the ESP32: all variables are reinitialized and WiFi/NTP
 *   sync + calendar fetch are repeated after each wake.
 * - Common API errors:
 *   - 403 Forbidden: Google Calendar API not enabled for the API key/project.
 *   - 404 Not Found: calendar is not public or the Calendar ID is incorrect.
 * - Keep API keys private in real projects; avoid committing them to public repos.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
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
Inkplate inkplate(INKPLATE_3BIT);
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
  // inkplate.setRotation(1); // Portrait mode | if it's upside down do setRotation(3);

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