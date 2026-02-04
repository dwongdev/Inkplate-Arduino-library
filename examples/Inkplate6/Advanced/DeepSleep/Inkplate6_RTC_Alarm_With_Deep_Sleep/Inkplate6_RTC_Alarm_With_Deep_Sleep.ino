/*
   Inkplate6_RTC_Alarm_With_Deep_Sleep example for Soldered Inkplate 6
   For this example you will need only USB cable and Inkplate 6
   Select "e-radionica Inkplate6" or "Soldered Inkplate6" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6" or "Soldered Inkplate6" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to use RTC alarm interrupt with deep sleep.
   Inkplate features RTC chip with interrupt for alarm connected to GPIO39
   Inkplate board will wake up every 10 seconds, refresh screen and go back to sleep.

   NOTE: External PCF85603 is only available on newer versions of Inkplate 6 boards.
   If your board doesn't have one, you can't run this example.
   If there is a battery holder on the back of Inkplate board, you have external RTC on Inkplate board and you can run
   this example.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   20 February 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    display.rtc.ClearAlarmFlag(); // Clear alarm flag from any previous alarm

    if (!display.rtc.IsSet()) // Check if RTC is already is set. If ts not, set time and date
    {
        //  display.setTime(hour, minute, sec);
        display.rtc.SetTime(6, 54, 00); // 24H mode, ex. 6:54:00
        //  display.setDate(weekday, day, month, yr);
        display.rtc.SetDate(6, 16, 5, 2020); // 0 for Sunday, ex. Saturday, 16.5.2020.

        // display.rtc.SetEpoch(1589610300); // Or use epoch for setting the time and date
    }

    printCurrentTime(); // Display current time and date
    display.display();

    display.rtc.SetAlarmEpoch(display.rtc.GetEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS); // Set RTC alarm 10 seconds from now

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

    display.rtc.GetRtcData();

    switch (display.rtc.GetWeekday())
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

    display.print(display.rtc.GetDay());
    display.print(".");
    display.print(display.rtc.GetMonth());
    display.print(".");
    display.print(display.rtc.GetYear());
    display.print(". ");
    print2Digits(display.rtc.GetHour());
    display.print(':');
    print2Digits(display.rtc.GetMinute());
    display.print(':');
    print2Digits(display.rtc.GetSecond());
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
