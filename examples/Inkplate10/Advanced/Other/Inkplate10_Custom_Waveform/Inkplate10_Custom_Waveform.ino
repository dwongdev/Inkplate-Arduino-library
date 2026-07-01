/**
 * Custom Waveform Tuning via UART
 *
 * Draws vertical grayscale bands on screen. Listens on Serial for a new
 * waveform in the format:
 *   TS;NUM_OF_COLORS;PHASES_PER_COLOR;v0;v1;...;vN;TE
 * where values are waveform entries row-major (color0phase0, color0phase1, ...).
 *
 * NUM_OF_COLORS can be 1–16. PHASES_PER_COLOR can be 1–16.
 * Values per phase: 0=discharge, 1=black drive, 2=white drive.
 *
 * Example for an 8x9 waveform:
 *   TS;8;9;0;0;0;0;0;0;0;1;0;0;0;0;2;2;2;1;1;0;...;TE
 *
 * Works on: Inkplate4TEMPERA, Inkplate5, Inkplate5V2, Inkplate6,
 *           Inkplate6FLICK, Inkplate6PLUS, Inkplate10, Inkplate10V2
 */

#include "Inkplate.h"

#ifdef ARDUINO_INKPLATE4TEMPERA
Inkplate display(INKPLATE_3BIT);
#elif defined(ARDUINO_INKPLATE5)
Inkplate display(INKPLATE_3BIT);
#elif defined(ARDUINO_INKPLATE5V2)
Inkplate display(INKPLATE_3BIT);
#elif defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE6V2)
Inkplate display(INKPLATE_3BIT);
#elif defined(ARDUINO_INKPLATE6FLICK)
Inkplate display(INKPLATE_3BIT);
#elif defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
Inkplate display(INKPLATE_3BIT);
#elif defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
Inkplate display(INKPLATE_3BIT);
#else
#error "This example does not support this Inkplate model."
#endif

#define MAX_COLORS  16
#define MAX_PHASES  16
#define SERIAL_BAUD 115200
#define TOKEN_START "TS"
#define TOKEN_END   "TE"

static uint8_t rxWaveform[MAX_COLORS][MAX_PHASES];
static int     currentNumColors = 8;

void drawGrayscaleBands(int numColors)
{
    display.clearDisplay();

    int bandWidth = display.width() / numColors;

    for (int i = 0; i < numColors; i++)
    {
        display.fillRect(i * bandWidth, 0, bandWidth, display.height(), i);

        // Only label if band is wide enough for a number
        if (bandWidth >= 20)
        {
            // White text on dark bands, black text on light bands
            display.setTextColor(i < numColors / 2 ? (numColors - 1) : 0);
            display.setTextSize(2);
            String label = String(i);
            int labelW = label.length() * 12;
            display.setCursor(i * bandWidth + (bandWidth - labelW) / 2,
                              display.height() / 2 - 8);
            display.print(label);
        }
    }

    display.display();
}

bool parseAndApplyWaveform(String &line)
{
    int pos = 0;

    // Find TS
    int tsPos = line.indexOf(TOKEN_START);
    if (tsPos < 0)
        return false;
    pos = tsPos + strlen(TOKEN_START);
    if (line.charAt(pos) != ';')
        return false;
    pos++;

    // Parse NUM_OF_COLORS
    int semicolon = line.indexOf(';', pos);
    if (semicolon < 0)
        return false;
    int numColors = line.substring(pos, semicolon).toInt();
    pos = semicolon + 1;

    if (numColors < 1 || numColors > MAX_COLORS)
    {
        Serial.print("Error: NUM_OF_COLORS out of range 1-");
        Serial.print(MAX_COLORS);
        Serial.print(", got ");
        Serial.println(numColors);
        return false;
    }

    // Parse PHASES_PER_COLOR
    semicolon = line.indexOf(';', pos);
    if (semicolon < 0)
        return false;
    int numPhases = line.substring(pos, semicolon).toInt();
    pos = semicolon + 1;

    if (numPhases < 1 || numPhases > MAX_PHASES)
    {
        Serial.print("Error: PHASES_PER_COLOR out of range 1-16, got ");
        Serial.println(numPhases);
        return false;
    }

    // Parse waveform values
    for (int c = 0; c < numColors; c++)
    {
        for (int p = 0; p < numPhases; p++)
        {
            semicolon = line.indexOf(';', pos);
            if (semicolon < 0)
            {
                Serial.println("Error: not enough waveform values");
                return false;
            }
            int val = line.substring(pos, semicolon).toInt();
            pos = semicolon + 1;
            if (val < 0 || val > 2)
            {
                Serial.print("Error: waveform value out of range 0-2 at color ");
                Serial.print(c);
                Serial.print(" phase ");
                Serial.println(p);
                return false;
            }
            rxWaveform[c][p] = (uint8_t)val;
        }
    }

    // Expect TE
    String tail = line.substring(pos);
    tail.trim();
    if (!tail.startsWith(TOKEN_END))
    {
        Serial.println("Error: missing TE end token");
        return false;
    }

    // Pack into a contiguous buffer with stride numPhases.
    // rxWaveform[MAX_COLORS][MAX_PHASES] has stride MAX_PHASES; setWaveform() expects stride numPhases.
    uint8_t packed[MAX_COLORS * MAX_PHASES];
    for (int c = 0; c < numColors; c++)
        memcpy(packed + c * numPhases, rxWaveform[c], numPhases);

    bool ok = display.setWaveform(packed, numColors, numPhases);
    if (!ok)
    {
        Serial.println("Error: setWaveform failed");
        return false;
    }

    currentNumColors = numColors;
    return true;
}

void setup()
{
    Serial.begin(SERIAL_BAUD);
    display.begin();

    Serial.println("Inkplate Custom Waveform Tuner");
    Serial.println("Send waveform as: TS;NUM_COLORS;PHASES;v0;v1;...;vN;TE");
    Serial.print("NUM_COLORS: 1-");
    Serial.print(MAX_COLORS);
    Serial.println("  PHASES: 1-16");

    drawGrayscaleBands(currentNumColors);
}

void loop()
{
    static String rxBuf;

    while (Serial.available())
    {
        char c = (char)Serial.read();
        if (c == '\n' || c == '\r')
        {
            if (rxBuf.length() > 0)
            {
                if (parseAndApplyWaveform(rxBuf))
                {
                    Serial.println("Waveform applied. Refreshing display...");
                    drawGrayscaleBands(currentNumColors);
                    Serial.println("Done.");
                }
                rxBuf = "";
            }
        }
        else
        {
            rxBuf += c;
        }
    }
}
