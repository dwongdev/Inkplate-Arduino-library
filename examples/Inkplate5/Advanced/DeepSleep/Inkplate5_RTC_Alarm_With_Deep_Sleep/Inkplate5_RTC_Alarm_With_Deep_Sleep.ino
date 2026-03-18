/**
 **************************************************
 * @file        Inkplate5_RTC_Alarm_With_Deep_Sleep.ino
 * @brief       RTC alarm wake-up with deep sleep for Soldered Inkplate 5.
 *
 * @details     Demonstrates how to use the onboard RTC alarm interrupt to wake
 *              the Inkplate 5 from ESP32 deep sleep. The RTC alarm is configured
 *              to trigger periodically, waking the board, refreshing the
 *              e-paper display with the current date and time, and then
 *              returning the system back to low-power deep sleep mode.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5
 * - Serial settings: 115200 baud (optional, not required for output)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/5/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 5.
 * 2) On first boot, the RTC time and date are initialized if not already set.
 * 3) The current time and date are shown on the display.
 * 4) The board enters deep sleep and wakes up every 10 seconds using the RTC alarm.
 * 5) After each wake-up, the display is refreshed and the board goes back to sleep.
 *
 * Expected output:
 * - Inkplate display shows the current weekday, date, and time.
 * - Display refreshes automatically on every RTC alarm wake-up.
 *
 * Notes:
 * - RTC alarm interrupt is connected to GPIO39 on Inkplate 5.
 * - When using deep sleep, all application logic must be placed in setup().
 * - loop() must remain empty when deep sleep is used.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-20
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h" 

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT); 

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin(); 

    // Clear alarm flag from any previous alarm
    display.rtc.ClearAlarmFlag(); 

    // Check if RTC is already set. If it's not, set a time and date
    if (!display.rtc.IsSet()) 
    {
        // display.setTime(hour, minute, sec);
        display.rtc.SetTime(6, 54, 00); // 24H mode, ex. 6:54:00
        // display.setDate(weekday, day, month, yr);
        display.rtc.SetDate(2, 21, 3, 2023); // 0 for Sunday, 1 for Monday, ... for example Tuesday, 21.3.2023.

        // display.rtc.SetEpoch(1679386177); // Or use epoch for setting the time and date
    }

    // Display current time and date
    printCurrentTime();
    display.display();

    // Set RTC alarm 10 seconds from now
    display.rtc.SetAlarmEpoch(display.rtc.GetEpoch() + 10, RTC_ALARM_MATCH_DHHMMSS);

    // Enable wakup from deep sleep on gpio 39 where RTC interrupt is connected
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

    // Put ESP32 into deep sleep. Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Print the current time on the screen
void printCurrentTime()
{
    // Set cursor and font size
    display.setCursor(100, 260);
    display.setTextSize(3);

    // Get data from the RTC
    display.rtc.GetRtcData();

    // Find a weekday and print its full name
    switch (display.rtc.GetWeekday())
    {
    case 0:
        display.print("Sunday, ");
        break;
    case 1:
        display.print("Monday, ");
        break;
    case 2:
        display.print("Tuesday, ");
        break;
    case 3:
        display.print("Wednesday, ");
        break;
    case 4:
        display.print("Thursday, ");
        break;
    case 5:
        display.print("Friday, ");
        break;
    case 6:
        display.print("Saturday, ");
        break;
    }

    // Print date and time
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

// A function that prints 2 digits
// It adds 0 before the number if it's only one digit
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
