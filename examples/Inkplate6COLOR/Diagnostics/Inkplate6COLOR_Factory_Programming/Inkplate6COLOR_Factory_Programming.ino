/**
 **************************************************
 * @file        Inkplate6COLOR_Factory_Programming.ino
 * @brief       Factory programming and production test firmware for
 *              Soldered Inkplate 6COLOR.
 *
 * @details     This firmware is intended for factory flashing, hardware
 *              validation, and final production testing of Inkplate 6COLOR.
 *              On first startup, it initializes core subsystems, checks the
 *              I2C bus, runs peripheral test routines, stores a marker in
 *              EEPROM, and then shows a splash screen on the display.
 *
 *              After initialization, the firmware enters Peripheral Mode and
 *              listens for incoming Serial commands that can control the
 *              display. This makes the firmware suitable both for production
 *              verification and for preloading a ready-to-use serial control
 *              interface on the device.
 *
 *              Test output is sent primarily over Serial because full color
 *              e-paper refreshes are relatively slow. The production test flow
 *              may require additional hardware such as a microSD card, a device
 *              connected to the EasyC port, valid Wi-Fi credentials inside the
 *              test sources, and user interaction through the wake button.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      WiFi, microSD card, EasyC slave device, Serial Monitor
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - Serial Monitor: 115200 baud
 * - Edit WiFi credentials in test.cpp before running full tests
 * - Connect an EasyC / I2C slave device on the expected address
 *   (default noted in test.cpp)
 * - Insert a formatted microSD card
 * - Press the wake button when required by the test sequence
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Prepare the required test setup: Wi-Fi credentials, EasyC slave device,
 *    formatted microSD card, and Serial Monitor at 115200 baud.
 * 2) Select Soldered Inkplate 6COLOR in Arduino IDE and upload the firmware.
 * 3) On startup, the firmware initializes EEPROM, I2C, and display hardware.
 * 4) The factory test routine checks connected peripherals and reports results
 *    over Serial.
 * 5) After testing, a splash screen is shown on the display.
 * 6) The firmware remains in Peripheral Mode and processes incoming Serial
 *    commands for display control.
 *
 * Expected output:
 * - Serial: Factory test progress, pass/fail information, and diagnostic
 *   output from peripheral checks.
 * - Display: Splash screen image after initialization/testing.
 * - Runtime: Peripheral Mode command handling over Serial.
 *
 * Notes:
 * - Display mode: 3-bit color mode is used for the splash screen bitmap.
 * - This is factory/support firmware, not a minimal end-user demo.
 * - EEPROM is used to store a marker after the first test/programming pass.
 *   Avoid repurposing reserved EEPROM locations without reviewing the factory
 *   workflow.
 * - Peripheral Mode relies on Serial command parsing and is slower than direct
 *   native drawing code for high-volume pixel updates.
 * - EasyC/I2C, SD card, Wi-Fi, wake button, and EEPROM checks may all be part
 *   of the production validation path depending on the linked test sources.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATECOLORV2)
#error "Wrong board selection for this example, please select Inkplate 6 Color in the boards menu."
#endif

// Do not forget to add WiFi SSID and WiFi Password in test.cpp!

// Include needed libraries in the sketch
#include "EEPROM.h"
#include "Inkplate.h"
#include "Wire.h"

// Include our functions and image
#include "Peripheral.h"
#include "test.h"
#include "image.h"

Inkplate display;

// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMaddress = 0;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

void setup()
{
    Serial.begin(115200);
    display.setTextSize(3);
    EEPROM.begin(512);
    Wire.begin();

    // Wakeup button
    pinMode(GPIO_NUM_36, INPUT);

    bool isFirstStartup = true;

    if (isFirstStartup)
    {
        Wire.setTimeOut(3000);
        // Try to ping IO expander to test I2C
        Wire.beginTransmission(IO_INT_ADDR);
        if (Wire.endTransmission() != 0)
        {
            Serial.println("I2C Bus error!");
            failHandler();
        }
    }

    display.begin();

    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();

        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
    }

    memset(commandBuffer, 0, BUFFER_SIZE);

    showSplashScreen();
}

void loop()
{
    // Peripheral mode 
    // More about peripheral mode: https://inkplate.readthedocs.io/en/latest/peripheral-mode.html

    if (Serial.available())
    {
        while (Serial.available())
        {
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
            {
                commandBuffer[i] = commandBuffer[i + 1];
            }
            commandBuffer[BUFFER_SIZE - 1] = Serial.read();
        }
    }

    // Function in Peripheral.h
    run(commandBuffer, BUFFER_SIZE, &display);
}

// Print the initial image that remains on the screen
void showSplashScreen()
{
    display.clearDisplay();
    display.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.display();
}