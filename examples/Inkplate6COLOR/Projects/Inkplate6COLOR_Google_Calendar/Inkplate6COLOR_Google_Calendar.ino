/**
 **************************************************
 * @file        Inkplate6COLOR_Google_Calendar.ino
 * @brief       Connects to Wi-Fi, fetches events from a public Google Calendar,
 *              and displays them on Inkplate 6COLOR.
 *
 * @details     This example demonstrates a complete connected calendar workflow
 *              on Inkplate 6COLOR. The sketch connects to a Wi-Fi network,
 *              synchronizes time using NTP, requests upcoming events from a
 *              public Google Calendar through the Google Calendar API, and
 *              renders the results using a custom GUI on the color e-paper
 *              display.
 *
 *              The currently ongoing event can be highlighted using a selected
 *              Inkplate color. After updating the screen, the ESP32 enters deep
 *              sleep to reduce power consumption and wakes periodically to
 *              refresh the calendar data again.
 *
 *              This example requires a public Google Calendar and a valid
 *              Google API key. The calendar must be publicly accessible, and
 *              the Google Calendar API must be enabled in the Google Cloud
 *              project associated with the API key. If Wi-Fi fails or calendar
 *              loading fails, an error screen is shown instead.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      WiFi
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial Monitor: 115200 baud (optional for debugging)
 * - Enter your Wi-Fi SSID and password in the sketch
 * - Set the correct timeZone offset for your location
 * - Provide a public Google Calendar ID
 * - Provide a valid Google API key with Google Calendar API enabled
 * - Ensure the target calendar is public, otherwise requests may fail with
 *   permission errors
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Create or choose a public Google Calendar and copy its Calendar ID from
 *    the Google Calendar settings page.
 * 2) Create a Google Cloud project, enable Google Calendar API, and generate
 *    an API key.
 * 3) Enter the Wi-Fi credentials, calendar ID, API key, and time zone offset
 *    in the sketch.
 * 4) Upload the sketch to Inkplate 6COLOR.
 * 5) On boot, the board connects to Wi-Fi, synchronizes time using NTP, and
 *    requests calendar data from Google.
 * 6) The GUI renders the events on the display and highlights the currently
 *    active event, if one exists.
 * 7) The ESP32 enters deep sleep and wakes again after 10 minutes to refresh
 *    the data.
 *
 * Expected output:
 * - Display: A calendar/event overview screen rendered from the fetched Google
 *   Calendar data.
 * - Display: Wi-Fi or calendar-loading error screen if the request fails.
 * - Power behavior: The board wakes every 10 minutes, refreshes calendar data,
 *   then returns to deep sleep.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode with full refreshes.
 * - Deep sleep restarts the ESP32, so all application logic must run from
 *   setup(); loop() should remain empty.
 * - The Google Calendar must be public for this example. Private calendars
 *   require a different authenticated workflow.
 * - A wrong API key, disabled Google Calendar API, or incorrect calendar ID
 *   may cause request failures such as 403 or 404 errors.
 * - Network access, API latency, and response size can affect refresh time and
 *   RAM usage.
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

// --- User Info ---
String calendarID = "yourpublicgooglecalid@group.calendar.google.com";
String apiKey = "yourapikey";

int highlightColor = 6;  // this is used to highlight the currently ongoing event | change the integer to change the color (see below)

// 0 -> black
// 1 -> white
// 2 -> green
// 3 -> blue
// 4 -> red
// 5 -> yellow
// 6 -> orange

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
      gui.setHighlightColor(highlightColor);
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