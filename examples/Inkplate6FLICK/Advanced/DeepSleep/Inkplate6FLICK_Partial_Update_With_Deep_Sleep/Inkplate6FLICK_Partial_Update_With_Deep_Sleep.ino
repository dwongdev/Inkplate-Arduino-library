/**
 **************************************************
 * @file        Inkplate6FLICK_Partial_Update_With_Deep_Sleep.ino
 * @brief       Partial e-paper update with deep sleep demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to combine partial e-paper screen updates with
 *              ESP32 deep sleep on Inkplate 6FLICK. Because partial updates rely
 *              on RAM-stored screen state, the example shows how to reconstruct
 *              the previous screen content after waking from deep sleep before
 *              applying a new partial refresh.
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
 * 2) Device displays variables on screen.
 * 3) ESP32 enters deep sleep for a defined interval (10 s by default).
 * 4) After wake-up, screen is reconstructed and partially refreshed
 *    with updated values.
 *
 * Expected output:
 * - Two variables displayed on the e-paper screen.
 * - Values update after each deep sleep cycle.
 * - Reduced power consumption thanks to ESP32 deep sleep.
 *
 * Notes:
 * - Partial update works only in 1-bit (black & white) mode.
 * - After deep sleep, previous screen content must be recreated
 *   before calling partialUpdate().
 * - Periodic full refreshes are recommended to maintain image quality.
 * - Avoid partial update immediately after power-on.
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
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  10      // Time how long ESP32 will be in deep sleep (in seconds). In this case 10 seconds.

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Counter variable (stored in RTC RAM that stores variable even if deep sleep is used)
// Variables that are changed after each partial update has to be stored in RTC RAM in order to recreate screen before
// deep sleep
RTC_DATA_ATTR int counter = 0;
RTC_DATA_ATTR float decimal = PI;

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    createScreen();  // Function that contains everything that has to be written on screen

    if (rtc_get_reset_reason(0) == DEEPSLEEP_RESET) // Check if ESP32 is reseted by deep sleep or power up / user manual
                                                    // reset (or some other reason)
    {
        display.preloadScreen(); // If is woken up by deep sleep, recreate whole screen to be same as was before deep sleep
        counter++;            // Update variable / variables
        decimal *= 1.23;
        display.clearDisplay();      // Clear everything in buffer
        createScreen();              // Create new screen with new variables
        display.partialUpdate(true); // Partial update of screen. (Use this only in this
                                     // scenario, otherwise YOU CAN DAMAGE YOUR SCRREN)
    }
    else // If is not deep sleep reset, that must be some thing else, so use normal update procerude (full screen
         // update)
    {
        display.display();
    }
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *
                                  uS_TO_S_FACTOR); // Set EPS32 to be woken up in 10 seconds (in this case)

    // Uncomment this line if your Inkplate is older than Aug 2021 as older Inkplates have ESP32 wrover-e chips
    // rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)

    esp_deep_sleep_start(); // Put ESP32 into deep sleep (low power mode)
}

void loop()
{
    // Nothing should be here while using deep sleep!
}

void createScreen()
{
    display.setCursor(200, 300);      // Set text cursor @ X = 200, Y = 300
    display.setTextSize(3);           // Set font to be scaled up three times
    display.print("First variable:"); // Write first variable to buffer
    display.print(counter, DEC);
    display.setCursor(200, 340);       // Set text cursor @ X = 200, Y = 340
    display.print("Second variable:"); // Write second variable to buffer (use two decimals places)
    display.print(decimal, 2);
}
