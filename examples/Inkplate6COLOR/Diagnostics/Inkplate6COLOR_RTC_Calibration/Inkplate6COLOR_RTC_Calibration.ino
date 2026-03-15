/**
 **************************************************
 * @file        Inkplate6_Read_Touchpads.ino
 * @brief       Uses the built-in capacitive touchpads on Inkplate 6 to control
 *              a counter displayed on the screen.
 *
 * @details     This example demonstrates how to use the three capacitive
 *              touchpads integrated on the Inkplate 6 PCB. These pads are
 *              labeled 1, 2, and 3 and function as simple touch-sensitive
 *              switches that can be used for user input.
 *
 *              The sketch continuously reads the state of the touchpads using
 *              display.touchpad.read(). Each pad performs a specific action on
 *              a counter displayed on the screen:
 *
 *              - Pad 1 decreases the number
 *              - Pad 2 resets the number to zero
 *              - Pad 3 increases the number
 *
 *              The example uses 1-bit display mode and partial updates to keep
 *              screen refreshes fast. After several partial updates, a full
 *              refresh is automatically performed to maintain display quality.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate6
 * - Serial settings: not used in this example
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select Soldered Inkplate6 in Arduino IDE and upload the sketch.
 * 2) After startup, a number appears in the center of the display.
 * 3) Touch the pads on the bottom of the PCB:
 *    - Touch pad 1 to decrease the number.
 *    - Touch pad 2 to reset the number to zero.
 *    - Touch pad 3 to increase the number.
 * 4) The display updates each time a pad is touched.
 *
 * Expected output:
 * - Display: A large number that changes according to touchpad input.
 * - Display: Symbols "-", "0", and "+" printed above the touchpads as visual
 *   indicators of their functions.
 *
 * Notes:
 * - Display mode: 1-bit black-and-white (INKPLATE_1BIT).
 * - Partial updates are used for faster refresh. After ~20 partial updates,
 *   a full refresh is performed automatically.
 * - Capacitive touchpads are sensitive to environment and grounding and may
 *   behave differently depending on humidity, grounding, or enclosures.
 * - touchpad.read() returns 1 when the pad is touched and 0 when it is not.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-15
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library

#define REFRESH_DELAY 1000 // Delay between reading RTC time
unsigned long time1;       // Time for measuring refresh in millis

// Set initial time and date
uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;

void setup()
{
    Serial.begin(115200);                 // Init serial communication
    display.begin();                      // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();               // Clear frame buffer of display
    display.setTextColor(INKPLATE_BLACK); // Set text color to black
    display.setTextSize(4);               // Set text to be 4 times bigger than classic 5x7 px text

    pinMode(GPIO_NUM_36, INPUT); // Set wake-up button as input

    // Some Inkplates has external capacitors for RTC crystal, but you can use internal one if you have issues with accuracy. 
    // IMPORTANT:
    //  - If you use an internal capacitor, you have to remove the external ones. 
    //  - If you use an external one, you don't have the next line of code. 
    // Here we setting internal capacitor value (7 pF):
    // display.rtc.SetInternalCapacitor(RTC_7PF); 
    // Another option is 12.5 pF:
    display.rtc.SetInternalCapacitor(RTC_12_5PF);

    // Set offset for RTC crystal
    // The first argument is a mode (0 or 1):
    // 0 means that the offset is made once every two hours (Each LSB introduces an offset of 4.34 ppm)
    // 1 means that the offset is made every 4 minutes (Each LSB introduces an offset of 4.069 ppm)
    // The second argument is the offset value in decimal (from -64 to 63).
    // The real offset depends on the mode and it is equal to the: offset in ppm for specific mode * offset value in
    // decimal. For example: mode 0 (4.34 ppm), offset value 15 = + 65.1 ppm every 2 hours
    // See 8.2.3 in the datasheet for more details
    
    display.rtc.SetClockOffset(1, -63); 

    // How to calculate this offset?
    // 1. Measure the frequency on the clock pin of the RTC (let's call it fMeasured)
    // 2. Convert it to time (tMeasured = 1 / fMeasured)
    // 3. Calculate the difference to the ideal period of 1 / 32768.00: D = 1 / 32768 - tMeasured
    // 4. Calculate the ppm deviation compared to the measured value: Eppm = 1000000 * D / tMeasured
    // 5. Calculate the offset register value:
    // Mode = 0 -> Offset = Eppm / 4.34
    // Mode = 1 -> Offset = Eppm / 4.069
    // Round this number and this is the second parameter of the function

    // If you don't have an oscilloscope or something to measure the frequency, here is a procedure for you. 
    // NOTE: This is a longer, but more precise method to calibrate RTC.
    // When you run for the first time to see how much rtc misses, 
    // you MUST comment the display.rtc.SetClockOffset() function above.
    // Once again, if you are using external capacitor, you don't need neither 
    // display.rtc.SetInternalCapacitor(); so also comment this line.

    // First, upload the code to the Inkplate.
    // It would be best if you had a clock on the side (on a phone or computer).
    // Press the wake button at a certain time.
    // Remember that time or write it down somewhere because you will need it for the calculation.
    // Let the Inkplate count for a while.
    // The longer you wait, the more accurate the calculation will be. We recommend waiting 2-3 days.
    // When that time has passed, remember the current time and the time displayed on the Inkplate AT THE SAME TIME 
    // (We recommend taking pictures of them next to each other).
    // Now calculate how many seconds of real-time have passed and how many the Inkplate has counted.
    // Calculate the time counted by the Inkplate divided by the time that actually passed.
    // Then you will get a number that needs to be multiplied by 32768.
    // The number you get is the RTC frequency.
    // With that number, perform the above calculation as if you measured the frequency with an oscilloscope.
    // Pass the resulting number as the second argument of the function.
    
    // Print a message
    display.println("RTC calibration");
    display.setTextColor(INKPLATE_RED);
    display.setTextSize(3);
    display.println("\nOpen Serial Monitor at 115200\nbaud rate to see what's\nhappening");
    display.display();
    Serial.println("Press the wake-up button to start RTC");

    // Waiting for the button press
    while(digitalRead(GPIO_NUM_36) == HIGH)
    {
        // Waiting
    }

    // Set the RTC to begin
    display.rtc.SetTime(hours, minutes, seconds);    // Send time to RTC
}

void loop()
{ 
    // Print new time every second
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        display.rtc.GetRtcData();           // Get the time and date from RTC
        seconds = display.rtc.GetSecond();  // Store senconds in a variable
        minutes = display.rtc.GetMinute();  // Store minutes in a variable
        hours = display.rtc.GetHour();      // Store hours in a variable

        // Print the time on Serial Monitor
        Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
            
        time1 = millis(); // Store current millis
    }
}
