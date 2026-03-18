/**
 **************************************************
 * @file        Inkplate10_Google_Calendar.ino
 * @brief       Google Calendar dashboard example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to use Inkplate 10 as a low-power Google
 *              Calendar viewer. The example connects to WiFi, synchronizes
 *              time using NTP, fetches events from a public Google Calendar
 *              via the Google Calendar REST API, and renders them on the
 *              Inkplate e-paper display using a simple GUI layer.
 *
 *              To reduce power consumption, the device enters deep sleep
 *              after updating the display and periodically wakes up to
 *              refresh calendar data.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      Stable WiFi Internet connection
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 * - Enter WiFi credentials (SSID & password)
 * - Set your local time zone (UTC offset)
 * - Provide a public Google Calendar ID and Google API key
 *
 * Before you start:
 * - Make sure your Google Calendar is set to *public*
 *   (Settings → Access permissions for events).
 * - Enable **Google Calendar API** in Google Cloud Console.
 * - Create an API key under *APIs & Services → Credentials*.
 *
 * How to use:
 * 1) Enter WiFi credentials in the sketch.
 * 2) Set your time zone (UTC offset).
 * 3) Insert your public Google Calendar ID and API key.
 * 4) Upload the sketch to Inkplate 10.
 * 5) The calendar is fetched, rendered on screen, then the device enters deep sleep.
 *
 * Expected output:
 * - Calendar events displayed in a readable layout on the Inkplate screen.
 * - Error messages shown on the display if WiFi or API requests fail.
 * - Automatic refresh every defined sleep interval (default: 10 minutes).
 *
 * Notes:
 * - Only **public** calendars can be accessed without OAuth.
 * - API limits apply according to Google Cloud project settings.
 * - Designed for low-power, always-on wall or desk calendar use.
 *
 * Docs:
 * - Inkplate:   https://soldered.com/documentation/inkplate
 * - Project:    https://soldered.com/documentation/inkplate/projects/GoogleCalendar
 * - Google API: https://developers.google.com/calendar
 *
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

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