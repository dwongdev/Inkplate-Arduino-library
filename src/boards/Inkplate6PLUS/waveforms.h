#ifndef __WAVEFROMS_INKPLATE_6PLUS_ H__
#define __WAVEFROMS_INKPLATE_6PLUS_H__

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 0, 0, 0, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 1, 2, 1, 0}, {0, 2, 2, 2, 1, 1, 2, 1, 0},                            \
    {0, 0, 2, 2, 2, 1, 2, 1, 0}, {0, 0, 0, 0, 2, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 1, 1, 2, 0},                            \
    {0, 0, 2, 2, 2, 1, 1, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};


#define E_INK_WIDTH  1024
#define E_INK_HEIGHT 758
#endif
#endif
