/**
 **************************************************
 * @file        Inkplate6FLICK_RTC_Alarm_With_Deep_Sleep.ino
 * @brief       RTC alarm wake-up with deep sleep demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to use the onboard RTC alarm interrupt to wake
 *              the ESP32 from deep sleep on Inkplate 6FLICK. The RTC alarm
 *              interrupt line is connected to GPIO39. The device wakes up at a
 *              fixed interval (e.g., every 10 seconds), refreshes the e-paper
 *              display, and returns to deep sleep.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Libraries:  Inkplate library
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6FLICK.
 * 2) On boot, the sketch configures an RTC alarm.
 * 3) Inkplate enters deep sleep.
 * 4) When the RTC alarm triggers (GPIO39), the ESP32 wakes up,
 *    updates the display, then goes back to sleep.
 *
 * Expected output:
 * - Screen refreshes periodically based on the RTC alarm interval.
 * - Very low power consumption between wake-ups.
 *
 * Notes:
 * - RTC alarm interrupt is wired to GPIO39 on Inkplate 6FLICK.
 * - Prefer deep sleep for low-power periodic wake-ups.
 * - Interval is configured in code (commonly 10 s in this example).
 *
 * Docs:         https://docs.soldered.com/inkplate
 *
 * @author      Soldered Electronics
 * @date        2026-02-26
 * @license     GNU GPL V3
 **************************************************
 */

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    display.rtc.clearAlarmFlag(); // Clear alarm flag from any previous alarm

    if (!display.rtc.isSet()) // Check if RTC is already is set. If ts not, set time and date
    {
        //  display.setTime(hour, minute, sec);
        //display.rtc.setTime(13, 30, 00); // 24H mode, ex. 13:30:00
        //  display.setDate(weekday, day, month, yr);
        //display.rtc.setDate(0, 18, 3, 2024); // 0 for Monday, ex. Monday, 18.3.2024.

        display.rtc.setEpoch(1772180976); // Or use epoch for setting the time and date
    }

    printCurrentTime(); // Display current time and date
    display.display();

    display.rtc.getRtcData(); // Get data from RTC into Inkplate object
    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS); // Set RTC alarm 10 seconds from now

    display.frontlight.setState(false); // Disable frontlight (to save power)

    // Enable wakup from deep sleep on gpio 39 where RTC interrupt is connected
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

    // Go to sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void printCurrentTime()
{
    display.setCursor(100, 300);
    display.setTextSize(3);

    display.rtc.getRtcData();

    switch (display.rtc.getWeekday())
    {
    case 0:
        display.print("Sunday , ");
        break;
    case 1:
        display.print("Monday , ");
        break;
    case 2:
        display.print("Tuesday , ");
        break;
    case 3:
        display.print("Wednesday , ");
        break;
    case 4:
        display.print("Thursday , ");
        break;
    case 5:
        display.print("Friday , ");
        break;
    case 6:
        display.print("Saturday , ");
        break;
    }

    display.print(display.rtc.getDay());
    display.print(".");
    display.print(display.rtc.getMonth());
    display.print(".");
    display.print(display.rtc.getYear());
    display.print(". ");
    print2Digits(display.rtc.getHour());
    display.print(':');
    print2Digits(display.rtc.getMinute());
    display.print(':');
    print2Digits(display.rtc.getSecond());
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
