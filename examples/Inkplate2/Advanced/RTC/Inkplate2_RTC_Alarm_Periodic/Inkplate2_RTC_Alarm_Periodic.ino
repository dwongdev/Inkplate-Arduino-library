/**
 **************************************************
 * @file        Inkplate2_RTC_Alarm_Periodic.ino
 * @brief       Periodic RTC alarm check: fetch time via NTP, determine time
 *              until a target date/time, and deep sleep between checks.
 *
 * @details     This example demonstrates an RTC-based alarm workflow implemented
 *              as a periodic check. On each boot, the sketch connects to WiFi,
 *              fetches the current time from an NTP server, and calculates the
 *              remaining time until a configured alarm moment (day/month and
 *              hour/minute/second).
 *
 *              If the alarm time has been reached (or is in the past), an
 *              "ALARM!" message is shown on the display and the sketch stops.
 *              Otherwise, the sketch displays a "Waiting for" screen and sets
 *              a wakeup timer to re-check later, then enters ESP32 deep sleep.
 *
 *              This is not a hardware RTC interrupt alarm. The alarm is reached
 *              only when the device wakes and performs the comparison, so the
 *              effective trigger resolution depends on the chosen wake period.
 *              Because deep sleep resets the ESP32, execution always starts
 *              from setup() on every wake.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access (NTP)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - Serial Monitor: (optional) 115200 baud
 * - WiFi:           set ssid/pass
 * - Timezone:       set timeZone (hours offset from UTC)
 * - Alarm time:     set alarmHour/alarmMins/alarmSecs and alarmDay/alarmMon
 * - Wake period:    set wakeHours/wakeMinutes (how often to re-check)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Enter your WiFi SSID/password and set the correct timeZone.
 * 2) Set the desired alarm date/time and the periodic wake interval.
 * 3) Upload the sketch to Inkplate 2.
 * 4) The device shows a waiting screen, then sleeps.
 * 5) It wakes periodically, fetches the current time again, and triggers the
 *    alarm screen once the target time has been reached.
 *
 * Expected output:
 * - Before alarm: display shows "Waiting for" with the configured alarm time
 *   and date, then the device enters deep sleep.
 * - At/after alarm: display shows "ALARM!" and the device remains awake in an
 *   infinite loop.
 *
 * Notes:
 * - Display mode is 1-bit (BW). This example uses full refresh (display()).
 * - Deep sleep restarts the ESP32 each wake. Keep the logic in setup() and
 *   leave loop() empty.
 * - Alarm resolution is limited by the wake interval: the alarm will trigger
 *   on the first wake that occurs after the target time.
 * - This example depends on WiFi/NTP at each wake. If WiFi is unavailable, the
 *   alarm check cannot be performed (add retries/error handling as needed).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-11-28
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library

#include "NetworkFunctions.h" // Our networking functions, declared in Network.cpp

#include "RTC.h" // Our RTC functions, declared in RTC.cpp

Inkplate display; // Initialize Inkplate object

NetworkFunctions network; // Create network object for WiFi and HTTP functions

RTC rtc; // Create RTC object for RTC functions

// Write your SSID and password (needed to get the correct time from the Internet)
char ssid[] = "Soldered Electronics";
char pass[] = "dasduino";

// Adjust your time zone, 1 means UTC+1
int timeZone = 1;

// Set the time and date to trigger the alarm function
// Here it's set to 25.12. 8:00:00 AM, Christmas morning of the current year
int alarmHour = 8;
int alarmMins = 0;
int alarmSecs = 0;
int alarmDay = 25;
int alarmMon = 12;

// Structure that contains time info
struct tm currentTime;

// Set the period to wake up - every 1 hour and 30 minutes wake up and check if it is time for the alarm
int wakeHours = 1;
int wakeMinutes = 30;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin(ssid, pass);

    // Get the current time from the NTP servers
    // Note: WiFi must be connected
    network.getTime(&currentTime, timeZone);

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).

    display.setTextColor(INKPLATE2_BLACK);

    int timeUntilAlarmInSeconds = rtc.secondsUntilAlarm(alarmHour, alarmMins, alarmSecs, alarmDay, alarmMon, currentTime);

    // The alarm time has been reached!
    if (timeUntilAlarmInSeconds <= 0)
    {
        // Display the screen when the alarm is reached
        alarmScreen();

        // Do whatever the alarm should do here
        while (1)
        {
            delay(100);
        }
    }
    else
    {
        // Display the screen for waiting for the alarm
        waitingScreen();

        rtc.setWakeUpTimer(wakeHours, wakeMinutes, currentTime);

        // Start sleep, this function never returns, program stops here
        esp_deep_sleep_start();
    }
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void waitingScreen()
{
    display.setTextSize(1);
    display.printf("\n           "); // For alignment
    display.printf("Waiting for: ");
    display.setTextSize(4);
    display.setCursor(0, 33);
    display.printf("  "); // For alignment
    display.printf("%2.1d:%02d", alarmHour, alarmMins);
    display.setCursor(0, 65);
    display.setTextSize(2);
    display.printf("    "); // For alignment
    display.printf("on %2.1d.%2.1d.", alarmDay, alarmMon);

    display.display(); // Show the data on the display
}

void alarmScreen()
{
    display.setTextSize(2);
    display.setCursor(9, 30);
    display.printf("ALARM!\n");
    display.setTextSize(1);

    display.display(); // Show the data on the display
}