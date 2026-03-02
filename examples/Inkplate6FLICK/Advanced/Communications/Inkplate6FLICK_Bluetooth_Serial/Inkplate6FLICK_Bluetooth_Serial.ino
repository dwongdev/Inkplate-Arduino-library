/**
 **************************************************
 * @file        Inkplate6FLICK_Bluetooth_Serial.ino
 * @brief       Bluetooth SPP serial bridge demo for Soldered Inkplate 6FLICK.
 *
 * @details     Starts Bluetooth Serial on Inkplate 6FLICK and mirrors text
 *              between a phone terminal app and the Inkplate (Serial Monitor
 *              + e-paper display). Useful for validating Bluetooth pairing
 *              and basic serial communication.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB-C cable
 * - Extra:      Serial Bluetooth Terminal (Android) or equivalent app
 *
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6FLICK.
 * 2) Pair your phone with the device named "Inkplate 6FLICK"
 *    (or the custom name defined in btDeviceName).
 * 3) Open a Bluetooth serial terminal app and connect.
 * 4) Send text from the phone → appears on Inkplate display and Serial Monitor.
 * 5) Send text from Serial Monitor → forwarded to the phone via Bluetooth.
 *
 * Expected output:
 * - Inkplate display shows received Bluetooth text.
 * - Serial Monitor logs transmitted/received messages.
 * - Bluetooth device visible for pairing.
 *
 * Notes:
 * - Some iOS apps don’t support classic Bluetooth SPP profiles.
 * - Partial screen updates are used for faster e-paper refresh.
 * - Make sure the correct Inkplate board package is installed.
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

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate 6FLICK";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT;        // Create SerialBT object for Bluetooth communication

// Font scale for the text on the screen. The scale of 1 is 7px height
#define BIG_TEXT_SCALE   4
#define SMALL_TEXT_SCALE 3

void setup()
{
    display.begin();                     // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();              // Clear frame buffer of display
    display.setTextSize(BIG_TEXT_SCALE); // Scale text to be 5 times bigger then original (5x7 px)
    display.println("Bluetooth Serial Example");
    display.println();
    display.display(); // Display on the screen

    // Init SerialBT
    if (!SerialBT.begin(btDeviceName))
    {
        // Something goes wrong, print an error message
        display.println("An error occurred initializing Bluetooth");
    }
    else
    {
        // Bluetooth started successfully, print the messages on the screen
        display.println("The device started, now you can ");
        display.println("pair it with Bluetooth and send");
        display.println("messages");
        display.println();
    }

    // Init serial communication
    Serial.begin(115200);

    // Display messages on the screen
    display.partialUpdate();

    display.setTextSize(SMALL_TEXT_SCALE);
}

void loop()
{
    // If you type something on the serial port, send that data via Bluetooth to the connected phone
    if (Serial.available())
    {
        // Load the whole message and send it
        while (Serial.available())
        {
            SerialBT.write(Serial.read());
        }
    }

    // If there are bytes available to read in the Bluetooth port, display those data on the screen
    if (SerialBT.available())
    {
        // Check if the content fits on the screen expecting one line of the text
        if (display.getCursorY() + SMALL_TEXT_SCALE * 7 > E_INK_HEIGHT)
        {
            // Clear the frame buffer of the display and set cursor to the beginning of the screen
            display.setCursor(0, 0);
            display.clearDisplay();

            // Do a full refresh
            display.display();
        }

        // Load the whole message and print it on the screen
        while (SerialBT.available())
        {
            char c = SerialBT.read();
            display.print(c);
        }

        // Actually display the message
        display.partialUpdate();
    }

    // Wait a bit
    delay(20);
}
