/**
 **************************************************
 * @file        Inkplate6COLOR_RTC_Alarm.ino
 * @brief       Demonstrates RTC clock setup, alarm configuration, and alarm
 *              flag handling on Inkplate 6COLOR.
 *
 * @details     This example shows how to use the onboard PCF85063(A) real-time
 *              clock on Inkplate 6COLOR for basic timekeeping and alarm
 *              generation. The sketch resets the RTC, sets an initial time and
 *              date, configures an alarm, then repeatedly reads the current RTC
 *              values and prints them on the e-paper display.
 *
 *              The example also demonstrates how to check whether the RTC alarm
 *              has triggered by reading the alarm flag, and how to clear that
 *              flag after the event occurs. In this sketch, the alarm is set to
 *              occur about 60 seconds after startup, and the display is updated
 *              once per minute.
 *
 *              This is a polling-based RTC example: it checks the alarm status
 *              in software during normal execution. It does not use the RTC
 *              interrupt pin or deep sleep wake-up. For interrupt-based wake-up
 *              behavior, use the dedicated RTC alarm + deep sleep examples.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Edit the initial time/date and alarm values in the sketch before upload
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the sketch.
 * 2) Adjust the initial RTC time/date and alarm values in the sketch if needed.
 * 3) On startup, the sketch resets the RTC and writes the configured time/date.
 * 4) The sketch sets an RTC alarm using hour, minute, second, day, and weekday
 *    parameters.
 * 5) The current time and date are read from the RTC and shown on the display.
 * 6) When the alarm flag becomes active, "ALARM!" is printed on the screen and
 *    the flag is cleared.
 *
 * Expected output:
 * - Display: Current time, weekday, and date.
 * - Display: "ALARM!" appears after the configured RTC alarm condition occurs.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode.
 * - This example uses full display refreshes with a 60-second delay between
 *   updates.
 * - RTC alarm, RTC timer, and RTC interrupt are related but different features:
 *   this sketch demonstrates alarm configuration and software flag polling only.
 * - display.rtc.reset() clears previous RTC state, so the configured time/date
 *   is reapplied on every reset or power cycle.
 * - The PCF85063(A) RTC is suitable for general timekeeping, but like typical
 *   RTCs it depends on proper setup and backup power behavior for persistence.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-20
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library

// Set clock
uint8_t hour = 12;
uint8_t minutes = 51;
uint8_t seconds = 0;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 1;
uint8_t day = 20;
uint8_t month = 2;
uint8_t year = 23;

// Set alarm time and date (alarm will be generated 60 seconds after board power up)
uint8_t alarmHour = 12;
uint8_t alarmMinutes = 52;
uint8_t alarmSeconds = 0;
uint8_t alarmWeekday = 1;
uint8_t alarmDay = 20;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtc.reset();     // reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(3); // Set text to be 3 times bigger than classic 5x7 px text

    display.rtc.setTime(hour, minutes, seconds);    // Send time to RTC
    display.rtc.setDate(weekday, day, month, year); // Send date to RTC
    display.rtc.setAlarm(alarmSeconds, alarmMinutes, alarmHour, alarmDay, alarmWeekday); // Set alarm
}

void loop()
{
    display.rtc.getRtcData();           // Get the time and date from RTC
    seconds = display.rtc.getSecond();  // Store senconds in a variable
    minutes = display.rtc.getMinute();  // Store minutes in a variable
    hour = display.rtc.getHour();       // Store hours in a variable
    day = display.rtc.getDay();         // Store day of month in a variable
    weekday = display.rtc.getWeekday(); // Store day of week in a variable
    month = display.rtc.getMonth();     // Store month in a variable
    year = display.rtc.getYear();       // Store year in a variable

    display.clearDisplay();                                       // Clear content in frame buffer
    display.setCursor(100, 300);                                  // Set position of the text
    display.setTextColor(INKPLATE_GREEN, INKPLATE_WHITE);         // Set text color and background
    printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

    if (display.rtc.checkAlarmFlag())  // Check if alarm has occurred
    {
      display.rtc.clearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
      display.setCursor(400, 400);    // Set new position for cursor
      display.print("ALARM!");
    }

    display.display(); // Do a full refresh

    delay(60000); // Delay between refreshes one minute
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint16_t _month,
               uint8_t _year)
{
    // Write time and date info on screen
    char *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    print2Digits(_hour);
    display.print(':');
    print2Digits(_minutes);
    display.print(':');
    print2Digits(_seconds);

    display.print(' ');

    display.print(wday[_weekday]);
    display.print(", ");
    print2Digits(_day);
    display.print('/');
    print2Digits(_month);
    display.print('/');
    display.print(_year, DEC);
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
