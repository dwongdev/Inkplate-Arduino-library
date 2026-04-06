/**
 **************************************************
 * @file        ImageDither.cpp
 * @brief       dither functionalities for images
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
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA)
#include "Inkplate.h"
#include "Image.h"


/**
 * @brief       ditherGetPixelBmp calculates dither for given pixel using the
 *              currently selected kernel. The dither buffer is a circular
 *              buffer indexed by j & ditherRowMask so error propagates
 *              naturally across row and block boundaries without any swap step.
 *
 * @param       uint32_t px
 *              pixel value (8-bit grayscale or palette index)
 * @param       int i
 *              absolute column position
 * @param       int j
 *              absolute row position
 * @param       int w
 *              image width (used for error-diffusion boundary clamping)
 * @param       bool paletted
 *              true if px is a palette index that should be looked up first
 *
 * @return      new dithered pixel value (3-bit grayscale, 0-7)
 */
uint8_t Image::ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    if (_inkplate->getDisplayMode() == INKPLATE_1BIT)
        px = (uint16_t)px >> 1;

    const int rowIdx = j & ditherRowMask;
    int16_t *row = ditherBuffer[rowIdx];

    int16_t oldPixel = (int16_t)px + row[i];
    row[i] = 0; // clear after reading

    oldPixel = max((int16_t)0, min((int16_t)255, oldPixel));

    uint8_t newPixel = (uint8_t)oldPixel & (_inkplate->getDisplayMode() == INKPLATE_1BIT ? B10000000 : B11100000);
    int16_t quantError = oldPixel - newPixel;

    const DitherKernelDef *kernelDef = currentKernel;
    const int kernelWidth = kernelDef->width;
    const int kernelHeight = kernelDef->height;
    const int kernelX = kernelDef->x;
    const int coef = kernelDef->coef;
    const unsigned char *kernelData = kernelDef->data;

    const int minOffset = max(-kernelX, -i);
    const int maxOffset = min(kernelWidth - kernelX - 1, w - 1 - i);

    for (int k = 0; k < kernelHeight; ++k)
    {
        const int nextRowIdx = (rowIdx + k) & ditherRowMask;
        int16_t *nextRow = ditherBuffer[nextRowIdx];
        for (int l = minOffset; l <= maxOffset; ++l)
        {
            const int weight = kernelData[k * kernelWidth + (l + kernelX)];
            if (!weight)
                continue;
            nextRow[i + l] += (weight * quantError) / coef;
        }
    }

    return newPixel >> 5;
}

/**
 * @brief       ditherSwap is retained for API compatibility but is now a no-op.
 *              The circular dither buffer advances automatically via j & ditherRowMask
 *              in ditherGetPixelBmp, so no manual swap is needed.
 */
void Image::ditherSwap(int w)
{
    (void)w;
}
#endif
