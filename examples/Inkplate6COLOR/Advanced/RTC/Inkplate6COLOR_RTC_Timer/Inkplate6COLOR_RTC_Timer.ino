/**
 **************************************************
 * @file        Inkplate6COLOR_RTC_Simple.ino
 * @brief       Demonstrates basic RTC time/date setup and display on
 *              Inkplate 6COLOR.
 *
 * @details     This example shows how to use the onboard PCF85063(A) real-time
 *              clock on Inkplate 6COLOR for basic clock functionality. The
 *              sketch resets the RTC, sets an initial time and date, reads the
 *              current RTC values, and prints the formatted time and date on
 *              the e-paper display.
 *
 *              After initialization, the example keeps the ESP32 awake and
 *              refreshes the display once per minute using a millis()-based
 *              timing interval. This demonstrates a simple polling workflow for
 *              RTC-based time display without using alarms, interrupts, or deep
 *              sleep.
 *
 *              This example is useful as a starting point for clocks, wall
 *              displays, dashboards, and other projects that need basic RTC
 *              timekeeping and periodic screen updates.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Edit the initial RTC time/date values in the sketch before upload
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the sketch.
 * 2) Adjust the initial time/date values in the sketch if needed.
 * 3) On startup, the sketch resets the RTC and writes the configured time/date.
 * 4) The current time and date are read from the RTC and rendered on the
 *    display.
 * 5) The screen is refreshed automatically once per minute.
 *
 * Expected output:
 * - Display: Current time in HH:MM:SS format followed by weekday and date.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode.
 * - This example uses full display refreshes every 60 seconds.
 * - display.rtc.Reset() clears previous RTC state, so the configured time/date
 *   is reapplied on every reset or power cycle.
 * - This is a simple RTC polling example only. It does not demonstrate alarm,
 *   interrupt, timer, or deep sleep behavior.
 * - The PCF85063(A) RTC is suitable for general timekeeping, but persistence
 *   depends on correct RTC setup and backup power conditions.
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

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display; // Create an object on Inkplate library

// Set clock
uint8_t hour = 12;
uint8_t minutes = 50;
uint8_t seconds = 30;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 means Monday, ...)
uint8_t weekday = 4;
uint8_t day = 11;
uint8_t month = 11;
uint8_t year = 21;

// Set up a 15 seconds timer
int countdown_time = 60;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtc.Reset();           //  Reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text
    display.setTextColor(INKPLATE_RED, INKPLATE_WHITE);         //Set text color and background

    pinMode(39, INPUT_PULLUP); // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.rtc.SetTime(hour, minutes, seconds);    // Send time to RTC
    display.rtc.SetDate(weekday, day, month, year); // Send date to RTC

    // Set up a timer
    /*   source_clock
     *       Inkplate::TIMER_CLOCK_4096HZ     -> clk = 4096Hz -> min timer = 244uS -> MAX timer = 62.256mS
     *       Inkplate::TIMER_CLOCK_64HZ       -> clk = 64Hz   -> min timer = 15.625mS -> MAX timer = 3.984s
     *       RTC::TIMER_CLOCK_1HZ        -> clk = 1Hz    -> min timer = 1s -> MAX timer = 255s
     *       Inkplate::TIMER_CLOCK_1PER60HZ   -> clk = 1/60Hz -> min timer = 60s -> MAX timer = 4h15min
     *   value
     *       coundowntime in seconds
     *   int_enable
     *       true = enable interrupt; false = disable interrupt
     *   int_pulse
     *       true = interrupt generate a pulse; false = interrupt follows timer flag
     */
    display.rtc.TimerSet(RTC::TIMER_CLOCK_1HZ, countdown_time, true, false);
}

void loop()
{
    display.rtc.GetRtcData();             // Get the time and date from RTC
    seconds = display.rtc.GetSecond();  // Store senconds in a variable
    minutes = display.rtc.GetMinute();  // Store minutes in a variable
    hour = display.rtc.GetHour();       // Store hours in a variable
    day = display.rtc.GetDay();         // Store day of month in a variable
    weekday = display.rtc.GetWeekday(); // Store day of week in a variable
    month = display.rtc.GetMonth();     // Store month in a variable
    year = display.rtc.GetYear();       // Store year in a variable

    display.clearDisplay();                                       // Clear content in frame buffer
    display.setCursor(60, 300);                                  // Set position of the text
    display.setTextColor(INKPLATE_YELLOW, INKPLATE_WHITE); //Set text color and background
    printTime(hour, minutes, seconds, day, weekday, month, year); // Print the time on screen

    display.display(); // Do a full refresh
    
    delay(60000);                             // Delay between refreshes.
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint8_t _month,
               uint16_t _year)
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

    if (display.rtc.CheckTimerFlag())  // Check if timer event has occurred
    {
      display.rtc.ClearTimerFlag();  // It's recommended to clear timer flag after timer has occurred
      display.rtc.DisableTimer();    // Disable timer if you want to make it one time only. Is you want to be repeatable, comment this line
      display.setCursor(400, 400);  // Set new position for cursor
      display.print("Timer!");
    }
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
