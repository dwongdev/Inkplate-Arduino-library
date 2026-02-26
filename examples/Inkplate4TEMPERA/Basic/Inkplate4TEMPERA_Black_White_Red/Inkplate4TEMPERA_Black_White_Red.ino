/**
 **************************************************
 * @file        Inkplate4TEMPERA_Black_White_Red.ino
 * @brief       Demonstrates Adafruit GFX drawing primitives and text/bitmap
 *              rendering in 3-color (BLACK/WHITE/RED) mode on Inkplate 4 TEMPERA.
 *
 * @details     This example runs the Inkplate 4 TEMPERA in 3-color mode using
 *              INKPLATE_3BIT, enabling BLACK, WHITE, and RED rendering on the
 *              tri-color e-paper panel. It demonstrates a selection of Adafruit
 *              GFX-compatible drawing operations (pixels, lines, thick lines,
 *              rectangles, circles, rounded rectangles, triangles, ellipses,
 *              polygons), plus rendering a bitmap/logo in RED and showing text
 *              in both BLACK and RED.
 *
 *              Each demo step draws into the ESP32 frame buffer first and then
 *              performs a full refresh using display.display(). A small status
 *              label is printed at the bottom-left to indicate the current demo
 *              step. The sketch finishes by continuously rotating a RED text
 *              message using setRotation().
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
 * 2) The display will cycle through multiple BLACK/RED drawing demonstrations.
 * 3) After the final section, a RED "Inkplate 4TEMPERA" message rotates
 *    continuously.
 *
 * Expected output:
 * - E-paper: A sequence of geometry and text/bitmap demos using BLACK and RED,
 *   with full refreshes between steps. Ends with rotating RED text.
 *
 * Notes:
 * - Display mode is tri-color (BLACK/WHITE/RED) via INKPLATE_3BIT. Tri-color
 *   refreshes are slower than 1-bit BW and may show more noticeable flashing.
 * - Partial update is not available in tri-color mode; use full refreshes only.
 * - Frequent full refreshes are not recommended. Use multi-second delays between
 *   updates (this example uses a fixed delay for demo pacing).
 * - RED pigment updates can be slower and may require complete refresh cycles;
 *   for best quality, avoid rapid alternating changes in red-heavy content.
 * - Bitmap/logo data is provided as a compiled array (logo.h) and is drawn using
 *   the Inkplate image helper, not from a BMP/JPEG file at runtime.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-12
 * @license     GNU GPL V3
 **************************************************/

#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"
Inkplate display(INKPLATE_3BIT); // <-- IMPORTANT: 3-color mode (BLACK/WHITE/RED)

#define DELAY_MS 5000 // Recommended delay between full refreshes

#include "logo.h"

// Helper: alternate between BLACK and RED in demos
static inline uint16_t altColor(int i) { return (i & 1) ? RED : BLACK; }

void setup()
{
    display.begin();
    display.clearDisplay();
    display.display();

    // Welcome screen in RED
    display.setCursor(40, 300);
    display.setTextSize(3);
    display.setTextColor(RED, WHITE);
    display.print("Welcome to Inkplate 4TEMPERA!");
    display.display();
    delay(5000);

    // Reset default text style
    display.setTextColor(BLACK, WHITE);
}

void loop()
{
    // Draw a pixel (RED)
    display.clearDisplay();
    displayCurrentAction("Drawing a RED pixel", RED);
    display.drawPixel(100, 50, RED);
    display.display();
    delay(DELAY_MS);

    // Random pixels: mix BLACK and RED
    display.clearDisplay();
    for (int i = 0; i < 600; i++)
    {
        uint16_t c = (random(0, 3) == 0) ? RED : BLACK; // ~1/3 red, 2/3 black
        display.drawPixel(random(0, 599), random(0, 599), c);
    }
    displayCurrentAction("Drawing random BLACK/RED pixels", RED);
    display.display();
    delay(DELAY_MS);

    // Two diagonal lines: one BLACK, one RED
    display.clearDisplay();
    display.drawLine(0, 0, 599, 599, BLACK);
    display.drawLine(599, 0, 0, 599, RED);
    displayCurrentAction("Diagonal lines (BLACK + RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Random lines: alternate BLACK/RED
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawLine(random(0, 599), random(0, 599), random(0, 599), random(0, 599), altColor(i));
    }
    displayCurrentAction("50 random lines (BLACK/RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Thick lines: alternate BLACK/RED
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawThickLine(random(0, 599), random(0, 599), random(0, 599), random(0, 599), altColor(i),
                              (float)random(1, 20));
    }
    displayCurrentAction("50 random thick lines (BLACK/RED)", RED);
    display.display();
    delay(DELAY_MS);

    // One horizontal (RED)
    display.clearDisplay();
    display.drawFastHLine(100, 300, 400, RED);
    displayCurrentAction("One horizontal line (RED)", RED);
    display.display();
    delay(DELAY_MS);

    // One vertical (BLACK)
    display.clearDisplay();
    display.drawFastVLine(300, 100, 400, BLACK);
    displayCurrentAction("One vertical line (BLACK)", RED);
    display.display();
    delay(DELAY_MS);

    // Grid: vertical BLACK, horizontal RED
    display.clearDisplay();
    for (int i = 0; i < 600; i += 8)
        display.drawFastVLine(i, 0, 600, BLACK);
    for (int i = 0; i < 600; i += 12)
        display.drawFastHLine(0, i, 600, RED);
    displayCurrentAction("Grid (BLACK vertical, RED horizontal)", RED);
    display.display();
    delay(DELAY_MS);

    // Rectangle outline (RED)
    display.clearDisplay();
    display.drawRect(150, 200, 300, 200, RED);
    displayCurrentAction("Rectangle outline (RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Many rectangles: alternate BLACK/RED
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
        display.drawRect(random(0, 599), random(0, 599), 100, 150, altColor(i));
    displayCurrentAction("Many rectangles (BLACK/RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Filled rectangle (RED)
    display.clearDisplay();
    display.fillRect(150, 200, 300, 200, RED);
    displayCurrentAction("Filled rectangle (RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Many filled small squares: mix BLACK/RED
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        uint16_t c = (random(0, 2) == 0) ? RED : BLACK;
        display.fillRect(random(0, 599), random(0, 599), 30, 30, c);
    }
    displayCurrentAction("Many filled squares (BLACK/RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Circle outline (BLACK) + inner circle (RED)
    display.clearDisplay();
    display.drawCircle(300, 300, 90, BLACK);
    display.drawCircle(300, 300, 60, RED);
    displayCurrentAction("Circles (BLACK + RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Filled circle (RED)
    display.clearDisplay();
    display.fillCircle(300, 300, 75, RED);
    displayCurrentAction("Filled circle (RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Rounded rect outline (BLACK) + filled rounded rect (RED)
    display.clearDisplay();
    display.drawRoundRect(150, 200, 300, 200, 12, BLACK);
    display.fillRoundRect(170, 220, 260, 160, 18, RED);
    displayCurrentAction("Rounded rect (BLACK outline, RED fill)", RED);
    display.display();
    delay(DELAY_MS);

    // Triangle outline (RED) + filled triangle (BLACK)
    display.clearDisplay();
    display.drawTriangle(150, 400, 450, 400, 300, 100, RED);
    display.fillTriangle(200, 350, 400, 350, 300, 150, BLACK);
    displayCurrentAction("Triangle (RED outline, BLACK fill)", RED);
    display.display();
    delay(DELAY_MS);

    // Bitmap demo: draw logo in RED (instead of black)
    display.clearDisplay();
    display.image.draw(logo, 43, 247, 513, 105, RED);
    displayCurrentAction("Soldered logo (RED)", RED);
    display.display();
    delay(DELAY_MS);

    // Text demo: BLACK text + RED text
    display.clearDisplay();
    for (int i = 0; i < 5; i++)
    {
        display.setTextSize(i + 1);
        display.setCursor(90, (i * i * 10));
        display.setTextColor(BLACK, WHITE);
        display.print("Inkplate 4TEMPERA");
    }

    for (int i = 0; i < 5; i++)
    {
        display.setTextSize(i + 1);
        display.setCursor(90, 260 + (i * i * 10));
        display.setTextColor(RED, WHITE);
        display.print("Inkplate 4TEMPERA!");
    }

    displayCurrentAction("Text (BLACK top, RED bottom)", RED);
    display.display();
    delay(DELAY_MS);

    // Ellipse outline (RED) then filled (BLACK)
    display.clearDisplay();
    display.drawElipse(400, 200, 300, 300, RED);
    displayCurrentAction("Ellipse outline (RED)", RED);
    display.display();
    delay(DELAY_MS);

    display.clearDisplay();
    display.fillElipse(400, 200, 300, 300, BLACK);
    displayCurrentAction("Filled ellipse (BLACK)", RED);
    display.display();
    delay(DELAY_MS);

    // Polygon demo: outline RED, fill BLACK
    int xt[10];
    int yt[10];
    int n = 10;
    for (int i = 0; i < n; ++i)
    {
        xt[i] = random(50, 550);
        yt[i] = random(50, 550);
    }

    int k;
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (atan2(yt[j] - 300, xt[j] - 400) < atan2(yt[i] - 300, xt[i] - 400))
            {
                k = xt[i], xt[i] = xt[j], xt[j] = k;
                k = yt[i], yt[i] = yt[j], yt[j] = k;
            }

    display.clearDisplay();
    display.drawPolygon(xt, yt, n, RED);
    displayCurrentAction("Polygon outline (RED)", RED);
    display.display();
    delay(DELAY_MS);

    display.clearDisplay();
    display.fillPolygon(xt, yt, n, BLACK);
    displayCurrentAction("Filled polygon (BLACK)", RED);
    display.display();
    delay(DELAY_MS);

    // Rotate text forever: RED text on WHITE background
    int r = 0;
    display.setTextSize(5);
    display.setTextColor(RED, WHITE);
    while (true)
    {
        display.setCursor(120, 250);
        display.clearDisplay();
        display.setRotation(r);
        display.print("Inkplate 4TEMPERA");
        display.display();
        r++;
        delay(DELAY_MS);
    }
}

// Writes small text at bottom-left indicating what's currently done.
// (Added a color parameter so the label can be RED too.)
void displayCurrentAction(String text, uint16_t color)
{
    display.setTextSize(2);
    display.setTextColor(color, WHITE);
    display.setCursor(20, 560);
    display.print(text);

    // Reset to default for rest of drawings unless they override it
    display.setTextColor(BLACK, WHITE);
}