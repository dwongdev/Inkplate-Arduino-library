/**
 **************************************************
 * @file        Inkplate2_Bluetooth_Serial.ino
 * @brief       Bluetooth Serial (SPP) example for Inkplate 2 with data
 *              forwarding between smartphone and Serial Monitor.
 *
 * @details     This example demonstrates classic Bluetooth Serial Port
 *              Profile (SPP) communication on the Inkplate 2 (ESP32-based).
 *              After pairing a smartphone with the device, a Bluetooth serial
 *              terminal app can be used to exchange text data.
 *
 *              Data typed in the Arduino Serial Monitor is sent to the phone
 *              over Bluetooth. Data received from the phone is forwarded to
 *              the Serial Monitor. The e-paper display is used only for status
 *              messages during initialization.
 *
 *              Inkplate 2 operates in 1-bit (black/white) display mode. This
 *              example performs a single full display refresh during setup and
 *              does not use partial updates.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Smartphone + Bluetooth serial terminal app
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate2
 * - Serial Monitor: 115200 baud
 * - Bluetooth name: edit btDeviceName (optional)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select the Inkplate 2 board in Tools and upload the sketch.
 * 2) Open Serial Monitor at 115200 baud.
 * 3) Pair your phone with the device name defined in btDeviceName.
 * 4) Open a Bluetooth serial terminal app and connect to Inkplate 2.
 * 5) Type in Serial Monitor to send data to the phone.
 * 6) Type in the phone app to send data back to the Serial Monitor.
 *
 * Expected output:
 * - Display: initialization and pairing instructions shown once at startup.
 * - Serial Monitor: shows all data received from the smartphone via Bluetooth.
 * - Phone app: shows all data sent from the Serial Monitor.
 *
 * Notes:
 * - This is classic Bluetooth SPP (not BLE). Pairing is required.
 * - Display mode is 1-bit (BW). Only a full refresh is used in this example.
 * - No deep sleep is used; ESP32 remains active while powered.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-17
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate2";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display;         // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT; // Create SerialBT object for Bluetooth communication


void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(2); // Scale text to be 2 times bigger then original (5x7 px)
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.println("Bluetooth Serial Example");
    display.println();
    display.setTextSize(1);

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

        display.println("Open Serial Monitor at 115200");
        display.println("baud!");
    }

    // Init serial communication
    Serial.begin(115200);

    // Display messages on the screen
    display.display();
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
        // Load the whole message and print it on the screen
        while (SerialBT.available())
        {
            Serial.write(SerialBT.read());
        }
    }

    // Wait a bit
    delay(20);
}
