/**
 **************************************************
 * @file        Inkplate4TEMPERA_Mandelbrot_Set.ino
 * @brief       Render a Mandelbrot set view on Inkplate 4 TEMPERA in 1-bit
 *              (BW) mode by computing each pixel on the ESP32.
 *
 * @details     This example computes and renders a Mandelbrot set region by
 *              iterating the complex function z = z² + c for each pixel on the
 *              display. For every (x, y) coordinate mapped into the complex
 *              plane, the code runs up to MAXITERATIONS iterations and decides
 *              whether the point escapes. Points that do not escape are drawn
 *              as black (inside the set) and others remain white.
 *
 *              Rendering is performed fully in the ESP32 frame buffer and then
 *              pushed to the e-paper panel with a full refresh via display.display().
 *              Because the computation is heavy and uses double-precision math,
 *              this sketch can take a long time to complete on an MCU.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 4 TEMPERA
 * - Serial Monitor: 115200 baud (optional; prints progress per rendered row)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/10/quick-start-guide/
 *
 * How to use:
 * 1) Select the Inkplate 4 TEMPERA board and upload the sketch.
 * 2) Wait for the render to complete (may take several minutes).
 * 3) The final Mandelbrot image is displayed; after a short delay the render
 *    repeats.
 * 4) To explore other regions, change xFrom/xTo/yFrom/yTo and re-upload.
 *
 * Expected output:
 * - E-paper: A black/white Mandelbrot set rendering of the selected coordinate
 *   window.
 * - Serial: Row index (j) printed as progress while rendering.
 *
 * Notes:
 * - Display mode is 1-bit (BW). This example uses full refresh (display.display()).
 * - Rendering is CPU-intensive (nested loops over the full resolution with
 *   iterative complex math). Expect long runtimes and higher power draw while
 *   computing.
 * - MAXITERATIONS controls detail vs. speed. Increasing it improves boundary
 *   detail but increases render time.
 * - Partial updates are not used here; the whole image is generated before the
 *   first refresh.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-24
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"

#define MAXITERATIONS 150

Inkplate display(INKPLATE_1BIT);

// Takes a long time to render, approx. 3 minutes

// Explore different positions to draw
// Some interesting ones can be found here http://www.cuug.ab.ca/dewara/mandelbrot/Mandelbrowser.html
double xFrom = -0.7423, xTo = -0.8463;
double yFrom = 0.1092, yTo = 0.2102;

void setup()
{
    Serial.begin(115200);
    display.begin();
}

void loop()
{
    display.clearDisplay();
    for (int j = 0; j < E_INK_HEIGHT; ++j)
    {
        for (int i = 0; i < E_INK_WIDTH; ++i)
            display.drawPixel(
                i, j, colorAt(xFrom + (double)i * (xTo - xFrom) / 800.0, yFrom + (double)j * (yTo - yFrom) / 600.0));
        // for whole set:
        // display.drawPixel(i, j, colorAt(-2.0 + (3.0 * (double)i / 800.0), -1.0 + 2.0 * (double)j / 600.0));
        Serial.println(j);
    }
    display.display();
    delay(5000);
}

struct complex
{
    double re;
    double im;
};

void addComplex(struct complex *z, struct complex *c)
{
    z->re += c->re;
    z->im += c->im;
}

void squareComplex(struct complex *z)
{
    double re = z->re;
    double im = z->im;
    z->re = re * re - im * im;
    z->im = 2 * re * im;
}

double modulusComplexSqr(struct complex *z)
{
    return z->re * z->re + z->im * z->im;
}

uint8_t colorAt(double x, double y)
{
    struct complex z = {0.0, 0.0};
    struct complex c = {x, y};

    int i;
    for (i = 0; i < MAXITERATIONS && modulusComplexSqr(&z) <= 4.0; ++i)
    {
        squareComplex(&z);
        addComplex(&z, &c);
    }
    return i == MAXITERATIONS;
}
