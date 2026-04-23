/**
 **************************************************
 * @file        Inkplate10_Set_VCOM.ino
 * @brief       Reads and programs the Inkplate 10 EPD VCOM voltage via Serial.
 *
 * @details     This example shows how to read the currently stored VCOM value
 *              from the display power IC/EEPROM and (optionally) program a new
 *              VCOM value entered in the Serial Monitor. After reading or
 *              programming, a simple grayscale test pattern is drawn and the
 *              stored VCOM value is shown on the e-paper display.
 *
 *              VCOM is stored in EEPROM and can only be programmed a limited
 *              number of times. Do NOT run this sketch repeatedly or "tune"
 *              VCOM by trial-and-error. Program it once (only if needed) and
 *              leave it unchanged to avoid prematurely wearing out EEPROM.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 * - Serial Monitor: 115200 baud, Newline (LF)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 10.
 * 2) Open Serial Monitor at 115200 baud and set line ending to Newline (LF).
 * 3) Read the prompt and enter a VCOM value in the range [-5.0, 0.0]
 *    (example: -1.23), then press Send.
 * 4) The sketch validates the range, programs VCOM (if valid), and refreshes
 *    the display to show the stored VCOM and a grayscale bar pattern.
 *
 * Expected output:
 * - Display: "Stored VCOM: <value> V" and an 8-step 3-bit grayscale pattern.
 * - Serial:  Prompt + requested VCOM value + success/failure message.
 *
 * Notes:
 * - Display mode: 3-bit grayscale (INKPLATE_3BIT). Uses a full refresh.
 * - WARNING: VCOM is written to EEPROM with limited write endurance. Avoid
 *   repeated programming to prevent permanent wear/damage.
 * - If programming fails, verify you selected the correct Inkplate 10 board and
 *   use a stable USB connection/power source.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-07-29
 * @license     GNU GPL V3
 **************************************************/

 #if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Select Inkplate10 board"
#endif

#include <Inkplate.h>

Inkplate display(INKPLATE_3BIT);

void displayTestImage()
{
    display.clearDisplay();

    double vcom = display.getVCOMValue();

    display.setTextColor(BLACK);
    display.setTextSize(2);
    display.setCursor(5, 5);
    display.print("Stored VCOM: ");
    display.print(vcom, 2);
    display.print(" V");

    for (int i = 0; i < 8; i++)
    {
        int x = (display.width() / 8) * i;

        display.drawRect(x, 40, display.width() / 8, display.height(), i);
        display.fillRect(x, 40, display.width() / 8, display.height(), i);
    }

    display.display(true);
}

void setup()
{
    Serial.begin(115200);
    display.begin();

    Serial.println("Enter VCOM value [-5.0, 0.0]");
    Serial.println("Example: -1.23");

    displayTestImage();
}

void loop()
{
    if (!Serial.available())
        return;

    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() == 0)
        return;

    double v = line.toFloat();

    Serial.print("Requested VCOM: ");
    Serial.println(v, 2);

    if (v < -5.0 || v > 0.0)
    {
        Serial.println("VCOM out of range!");
        return;
    }

    if (display.setVCOM(v))
        Serial.println("VCOM programmed OK");
    else
        Serial.println("VCOM programming failed");

    displayTestImage();
}