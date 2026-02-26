/**
 **************************************************
 * @file        Inkplate4TEMPERA_Pedometer.ino
 * @brief       Use the onboard LSM6DS3 accelerometer/gyroscope as a pedometer
 *              and display step count with a simple walking animation.
 *
 * @details     This example configures the LSM6DS3 embedded pedometer feature
 *              and continuously reads the internal step counter registers. When
 *              the step count changes, the sketch updates the on-screen counter
 *              and advances a small animation frame to provide visual feedback
 *              while walking.
 *
 *              The display runs in 1-bit (BW) mode to enable partial updates
 *              for faster, lower-flash refreshes. Most updates use
 *              partialUpdate(false, true) to keep the e-paper power rails
 *              enabled between updates (faster, but higher power). After all
 *              animation frames have been shown, a full refresh is triggered to
 *              maintain image quality.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 4 TEMPERA
 * - Serial settings (if relevant): none
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select the Inkplate 4 TEMPERA board and upload the sketch.
 * 2) After boot, the screen shows "Start walking!" briefly.
 * 3) Walk with the device; the step count updates when new steps are detected.
 * 4) Watch the small animation advance as steps are counted.
 *
 * Expected output:
 * - E-paper: "Steps taken: <number>" plus a small animated icon that changes
 *   frames as you walk.
 *
 * Notes:
 * - Display mode is 1-bit (BW). Partial updates are supported only in BW mode.
 * - The LSM6DS3 pedometer is not instantaneous; it may require a few steps to
 *   start counting after you resume walking. This is expected behavior of the
 *   embedded algorithm/filtering.
 * - partialUpdate(false, true) keeps the panel powered for faster successive
 *   updates. For battery-focused designs, consider using leaveOn=false and/or
 *   batching updates.
 * - Full refresh is performed periodically (after a full animation cycle) to
 *   reduce ghosting from repeated partial updates.
 * - This example reads the LSM6DS3 step counter registers directly; resetting
 *   embedded functions can clear the counter, so avoid reinitializing the sensor
 *   unless you intend to reset the step count.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-08-27
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"          // Include Inkplate library to the sketch
#include "Fonts\Inter16pt7b.h" // Include the font used for this sketch
#include "animationFrames.h"   // Include the animation frames for the walking animation

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
// This helps with faster refreshes

// Variable that keeps count on how many times the screen has been partially updated
int numRefreshes = 0;

// To remember the number of steps
int stepsTaken = 0;

// To remember the frame of the animation which we're on
int animationFrameIndex = 0;

// Setup code, runs only once
void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    display.setTextColor(BLACK);   // Set the text color to black also
    display.setFont(&Inter16pt7b); // Set the custom font

    // Try to init the acccelerometer
    if (display.lsm6ds3.begin() != 0)
    {
        // In case of initialization failure, notify the user
        display.setCursor(50, 50);
        display.print("ERROR: can't init LSM6DS3!");
        display.display();

        // Go to sleep
        esp_deep_sleep_start();
    }

    // Let's configure the accelerometer to count steps
    uint8_t errorAccumulator = 0; // Remember any errors during config
    uint8_t dataToWrite = 0;      // Temporary variable
    // Setup the accelerometer
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_26Hz;
    // Write the patched together data
    errorAccumulator += display.lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
    // Set the ODR bit
    errorAccumulator += display.lsm6ds3.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);
    // Enable embedded functions -- ALSO clears the pedometer step count
    errorAccumulator += display.lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_CTRL10_C, 0x3E);
    // Enable pedometer algorithm
    errorAccumulator += display.lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x40);

    // Have any errors occured?
    if (errorAccumulator)
    {
        // In case of initialization failure, notify the user
        display.setCursor(50, 50);
        display.print("ERROR: can't config LSM6DS3!");
        display.display();

        // Go to sleep
        esp_deep_sleep_start();
    }

    // Setup is complete, show a message to the user to start walking:
    display.setCursor(200, 280);
    display.print("Start walking!");
    display.display();      // Display the message
    display.clearDisplay(); // Clear the frame buffer
}

void loop()
{
    // Let's read the nubmer of steps
    uint8_t readDataByte = 0;
    uint16_t newStepsTaken = 0;

    // Read the 16bit value by two 8bit operations
    display.lsm6ds3.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_STEP_COUNTER_H);
    newStepsTaken = ((uint16_t)readDataByte) << 8;
    display.lsm6ds3.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_STEP_COUNTER_L);
    newStepsTaken |= readDataByte;

    // If the newly read number of steps taken is different than the old one, draw it to the display
    if (stepsTaken != newStepsTaken)
    {
        // Remember the new number of steps
        stepsTaken = newStepsTaken;

        // Print the text in the correct position
        display.setCursor(175, 330);
        display.print("Steps taken: ");

        // Draw a white rectangle to clear the last written number of steps taken
        display.fillRect(359, 295, 290, 45, WHITE);
        // This enables us to not have to use clearDisplay each draw, which is faster

        // Print the number of steps
        display.print(stepsTaken);

        // Draw the next frame of the animation in the correct position
        display.image.draw(frames[animationFrameIndex], 275, 240, 50, 50, BLACK, WHITE);
        animationFrameIndex++;
        // If all the frames of the animation are complete...
        if (animationFrameIndex == sizeof(frames) / sizeof(frames[0]))
        {
            // Do a full refresh and reset the counter
            display.display();
            animationFrameIndex = 0;
        }
        else
        {
            // If not, do a fast partial update while keeping the ePaper display on
            display.partialUpdate(false, true);
        }
    }

    // Wait a short time before checking again
    delay(200);
}
