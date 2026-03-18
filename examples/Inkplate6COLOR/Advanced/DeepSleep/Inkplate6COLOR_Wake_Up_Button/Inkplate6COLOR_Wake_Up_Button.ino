/**
 **************************************************
 * @file        Inkplate6COLOR_Wake_Up_Button.ino
 * @brief       Demonstrates deep sleep wake-up by button press or timer on
 *              Inkplate 6COLOR.
 *
 * @details     This example shows how to use ESP32 deep sleep on Inkplate
 *              6COLOR with two wake-up sources: the onboard wake-up button and
 *              a fallback timer. On each boot, the sketch initializes the
 *              display, increments a boot counter stored in RTC memory, checks
 *              the wake-up cause, prints that information to the display, and
 *              then returns to deep sleep.
 *
 *              GPIO36 is configured as an external deep sleep wake source for
 *              the wake-up button, while a timer wake source is also enabled so
 *              the board wakes automatically after 30 seconds if the button is
 *              not pressed. This example is useful for low-power interactive
 *              applications where manual wake-up is preferred, but a periodic
 *              automatic wake is also acceptable.
 *
 *              Because deep sleep resets the ESP32, program execution always
 *              starts again from setup() after wake-up. Only data explicitly
 *              stored in RTC memory survives between sleep cycles.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the sketch.
 * 2) After boot, the display shows the current boot count and wake-up reason.
 * 3) The sketch enables two wake sources: the wake-up button on GPIO36 and a
 *    timer set to 30 seconds.
 * 4) The board enters deep sleep.
 * 5) Press the wake-up button to wake the board immediately, or wait for the
 *    timer wake-up to occur automatically.
 * 6) After wake-up, the ESP32 restarts, updates the boot counter and reason on
 *    the display, then goes back to sleep.
 *
 * Expected output:
 * - Display: Boot count and wake-up reason:
 *   "Wakeup caused by WakeUp button", "Wakeup caused by timer", or an initial
 *   power-on/reset message.
 * - Power behavior: The board remains in deep sleep until the button is pressed
 *   or the timer expires.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode.
 * - Deep sleep restarts the ESP32, so all application logic must run from
 *   setup(); loop() should remain empty.
 * - RTC_DATA_ATTR is used to preserve the boot counter across deep sleep cycles.
 * - External wake-up and timer wake-up are configured independently and can be
 *   enabled at the same time.
 * - Color e-paper updates use full refreshes and are slower than monochrome
 *   partial-update workflows on supported boards.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-14
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include <Inkplate.h>

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000
// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 30

// Initiate Inkplate object
Inkplate display;

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    display.begin();
    display.setTextColor(INKPLATE_BLACK);

    ++bootCount;

    // Our function declared below
    displayInfo();

    // Go to sleep for TIME_TO_SLEEP seconds
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    // Enable wakeup from deep sleep on gpio 36 (wake button)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you use deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Function that will write number of boots and boot reason to screen
void displayInfo()
{
    // First, lets delete everything from frame buffer
    display.clearDisplay();

    // Set text cursor and size
    display.setCursor(10, 180);
    display.setTextSize(2);

    display.print(F("Boot count: "));
    display.println(bootCount, DEC); // Print the number

    // Set next line cursor position
    display.setCursor(10, 220);

    // Display wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        display.println("Wakeup caused by WakeUp button");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        display.println("Wakeup caused by timer");
        break;
    default:
        display.println("Wakeup was not caused by deep sleep");
        break;
    }

    display.display();
}
