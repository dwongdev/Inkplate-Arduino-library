/**
 **************************************************
 * @file        Inkplate6COLOR_Peripheral_Mode.ino
 * @brief       Enables UART-based Peripheral Mode control for Inkplate 6COLOR.
 *
 * @details     This example enables Peripheral Mode on Inkplate 6COLOR,
 *              allowing an external host to control the e-paper display over
 *              a UART command interface instead of running custom drawing code
 *              directly on the board.
 *
 *              In this mode, display operations are received as serial
 *              commands and executed by the Peripheral Mode parser. This makes
 *              Inkplate 6COLOR usable as a display peripheral for PCs,
 *              Raspberry Pi boards, microcontrollers, and other embedded
 *              systems that can communicate over UART.
 *
 *              Because UART bandwidth is limited, this approach is not ideal
 *              for large numbers of low-level pixel commands such as repeated
 *              drawPixel() operations. For image-heavy workflows, it is more
 *              efficient to store bitmap/image files on an SD card and trigger
 *              higher-level drawing operations from there.
 *
 *              This firmware is typically preloaded from the factory and can
 *              be extended if additional command functionality is needed.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB-C cable
 * - Extra:      External controller device (optional)
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 6COLOR
 * - UART settings: 115200 baud, standard parity
 * - Line ending: "\n\r" (newline + carriage return)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 6COLOR.
 * 2) Connect to the board over USB serial or directly to the ESP32 UART pins.
 * 3) Configure the serial terminal or host device for 115200 baud with
 *    newline + carriage return line endings.
 * 4) Wait for the "READY" message.
 * 5) Send supported Peripheral Mode commands to control the display.
 *
 * Expected output:
 * - Serial: "READY" after successful Peripheral Mode initialization.
 * - Display: Updates according to received UART commands.
 *
 * Notes:
 * - Display mode: Inkplate 6COLOR color e-paper mode.
 * - Peripheral Mode is convenient for cross-platform display control, but
 *   UART-driven rendering is slower than native local drawing code.
 * - Sending many pixel-level commands is inefficient; prefer higher-level
 *   commands or image loading from SD storage when possible.
 * - This firmware acts as a serial command interpreter and continuously
 *   processes incoming data in loop().
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-10-23
 * @license     GNU GPL V3
 **************************************************/

// Include Inkplate library
#include "Inkplate.h"

// Include Peripheral Mode library
#include "InkplatePeripheralMode.h"

// Include the header file with sketch settings (buffer size, serial timeout, argument termination char, etc.)
#include "settings.h"

// Pointer to the singleton Peripheral Mode instance
PeripheralMode *peripheral;

// Create Inkplate 6COLOR display object
Inkplate display;

void setup()
{
    // Initialize Inkplate library
    display.begin();

    // Create instance of Peripheral Mode object
    peripheral = PeripheralMode::getInstance();

    // Initialize Peripheral Mode (UART, display, buffer, etc.)
    if (!peripheral->begin(&Serial, &display, 115200ULL, SERIAL_UART_RX_PIN, SERIAL_UART_TX_PIN, SERIAL_BUFFER_SIZE))
    {
        // Send an error message to serial
        Serial.println("Peripheral Mode init failed!\nProgram halted!");

        // Stop program execution
        while (1);
    }

    Serial.println("READY");
}

void loop()
{
    // Check if there is incoming data on serial and process commands
    peripheral->getDataFromSerial(SERIAL_TIMEOUT_MS);
}
