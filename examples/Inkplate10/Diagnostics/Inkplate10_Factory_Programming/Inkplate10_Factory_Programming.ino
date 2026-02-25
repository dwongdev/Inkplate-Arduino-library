/**
 **************************************************
 * @file        Inkplate10_Factory_Programming.ino
 * @brief       Factory programming and production test harness for Inkplate 10.
 *
 * @details     This sketch is intended for manufacturing / factory workflows.
 *              It performs a one-time production test sequence on first boot,
 *              then switches into a serial-controlled peripheral command loop.
 *
 *              On first startup (detected via a marker byte in ESP32 EEPROM),
 *              the sketch initializes required peripherals (EEPROM, I2C) and
 *              performs hardware validation via the test suite in test.h/.cpp.
 *              Test results are printed to the Serial Monitor (screen updates
 *              are avoided for speed). After a successful run, a marker is
 *              stored in EEPROM so the tests are skipped on subsequent boots.
 *
 *              After initialization, the sketch displays a 3-bit splash bitmap
 *              and then continuously processes incoming UART commands using
 *              the Peripheral mode runner (run()), allowing external hosts to
 *              control the Inkplate without flashing new firmware.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      microSD card (FAT32), EasyC/I2C slave device for testing (optional)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 * - Serial Monitor: 115200 baud
 * - WiFi credentials and test parameters are configured in test.cpp
 * - EasyC/I2C test slave address defaults to 0x76 (configurable in test.cpp)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Prepare test conditions:
 *    - Set WiFi credentials in test.cpp (if WiFi is part of the test suite).
 *    - Connect an I2C/EasyC slave device (default address 0x76), or use the
 *      helper firmware in the InkplateEasyCTester folder on a Dasduino board.
 *    - Insert a formatted microSD card (contents may be non-empty).
 * 2) Flash this sketch to Inkplate 10.
 * 3) Open Serial Monitor at 115200 baud to observe test output.
 * 4) Follow on-screen/Serial instructions (e.g., press the wake button when
 *    prompted) to complete the test sequence.
 * 5) After tests pass, the unit boots into peripheral command processing and
 *    shows a splash image on the display.
 *
 * Expected output:
 * - Serial Monitor: factory test progress, pass/fail results, and diagnostics.
 * - Display: a 3-bit splash bitmap after initialization.
 * - After first successful run: subsequent boots skip tests and go directly to
 *   command processing.
 *
 * Notes:
 * - Display mode: 3-bit grayscale (INKPLATE_3BIT). Partial updates are not
 *   supported in grayscale mode; the splash is shown via a full refresh.
 * - EEPROM wear: a marker byte is written to EEPROM to record that factory
 *   tests have completed; do not repeatedly erase/rewear EEPROM unnecessarily.
 * - I2C bus check: the first-startup path verifies the internal I/O expander
 *   address (IO_INT_ADDR) and halts on I2C errors.
 * - The command loop uses a sliding-window serial buffer; ensure the host
 *   protocol matches the Peripheral runner expectations (see Peripheral.*).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "EEPROM.h"
#include "Inkplate.h"
#include "Wire.h"

#include "Peripheral.h"
#include "test.h"
#include "image.h"

Inkplate inkplate(INKPLATE_3BIT);

const int EEPROMaddress = 0;

#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

void showSplashScreen();

void setup()
{
    Serial.begin(115200);

    inkplate.setTextSize(4);
    EEPROM.begin(512);
    Wire.begin();

    pinMode(GPIO_NUM_18, INPUT);

    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);

    if (isFirstStartup)
    {
        Wire.setTimeOut(3000);

        Wire.beginTransmission(IO_INT_ADDR);
        if (Wire.endTransmission() != 0)
        {
            Serial.println("I2C Bus error!");
            failHandler();
        }
    }

    inkplate.begin();

    if (isFirstStartup)
    {
        testPeripheral();
        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
    }

    memset(commandBuffer, 0, sizeof(commandBuffer));

    showSplashScreen();
}

void loop()
{
    if (Serial.available())
    {
        while (Serial.available())
        {
            // sliding window buffer
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
                commandBuffer[i] = commandBuffer[i + 1];

            commandBuffer[BUFFER_SIZE - 1] = (char)Serial.read();
        }
    }

    run(commandBuffer, BUFFER_SIZE, &inkplate);
}

void showSplashScreen()
{
    inkplate.clearDisplay();
    inkplate.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    inkplate.display();
}