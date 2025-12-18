/**
 **************************************************
 * @file        ImageDitherColor.cpp
 * @brief       dither functionalities for colored images
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/
#include "Inkplate.h"
#include "ImageColor.h"

#ifdef USE_COLOR_IMAGE

extern ImageColor *_imagePtrJpeg;

/**
 * @brief       findClosestPalette return closes pallete for given pixel
 *
 * @param       uint32_t c
 *              color of the given pixel
 *
 * @return      closest color in pallete array
 */
uint8_t ImageColor::findClosestPalette(int16_t r, int16_t g, int16_t b)
{
    int64_t minDistance = INT64_MAX;
    uint8_t contenderCount = 0;
    uint8_t contenderList[sizeof pallete / sizeof pallete[0]];

    for (int i = 0; i < sizeof pallete / sizeof pallete[0]; ++i)
    {
        int16_t pr = RED8(pallete[i]);
        int16_t pg = GREEN8(pallete[i]);
        int16_t pb = BLUE8(pallete[i]);

        int32_t dr = r - pr;
        int32_t dg = g - pg;
        int32_t db = b - pb;

        // Perceptual weighted RGB distance (Rec.601)
        int32_t currentDistance =
              30 * dr * dr
            + 59 * dg * dg
            + 11 * db * db;

        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            contenderList[0] = i;
            contenderCount = 1;
        }
        else if (currentDistance == minDistance)
        {
            if (contenderCount < sizeof pallete / sizeof pallete[0])
                contenderList[contenderCount++] = i;
        }
    }

    return contenderList[0];
}


/**
 * @brief       ditherGetPixelBmp finds dithered value for given pixel
 *
 * @param       uint32_t px
 *              pixel to find value for
 * @param       int i
 *              x plane pixel position
 * @param       int j
 *              y plane pixel position
 * @param       int w
 *              image width
 * @param       bool paletted
 *              1 if palleted image, 0 if not
 *
 * @return      new dithered pixel
 */
uint8_t ImageColor::ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    int16_t r = RED8(px) + ditherBuffer[0][j % 8][i];
    int16_t g = GREEN8(px) + ditherBuffer[1][j % 8][i];
    int16_t b = BLUE8(px) + ditherBuffer[2][j % 8][i];

    ditherBuffer[0][j % 8][i] = 0;
    ditherBuffer[1][j % 8][i] = 0;
    ditherBuffer[2][j % 8][i] = 0;

    r = max((int16_t)0, min((int16_t)255, r));
    g = max((int16_t)0, min((int16_t)255, g));
    b = max((int16_t)0, min((int16_t)255, b));

    int closest = findClosestPalette(r, g, b);

    int32_t rErr = r - (int32_t)((pallete[closest] >> 16) & 0xFF);
    int32_t gErr = g - (int32_t)((pallete[closest] >> 8) & 0xFF);
    int32_t bErr = b - (int32_t)((pallete[closest] >> 0) & 0xFF);


    for (int k = 0; k < kernelHeight; ++k)
    {
        for (int l = -kernelX; l < kernelWidth - kernelX; ++l)
        {
            if (!(0 <= i + l && i + l < w))
                continue;
            ditherBuffer[0][(j + k) % 8][i + l] += (kernel[k][l + kernelX] * rErr) / coef;
            ditherBuffer[1][(j + k) % 8][i + l] += (kernel[k][l + kernelX] * gErr) / coef;
            ditherBuffer[2][(j + k) % 8][i + l] += (kernel[k][l + kernelX] * bErr) / coef;
        }
    }

    return closest;
}


#endif
