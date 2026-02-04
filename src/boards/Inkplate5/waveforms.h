#ifdef ARDUINO_INKPLATE5
#ifndef __WAVEFROMS_INKPLATE_5_H__
#define __WAVEFROMS_INKPLATE_5_H__

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 1, 1, 0, 1, 1, 1, 0}, {0, 1, 1, 1, 1, 2, 0, 1, 0}, {1, 2, 2, 0, 2, 1, 1, 1, 0},                            \
     {1, 1, 1, 2, 0, 1, 1, 2, 0}, {0, 1, 1, 1, 2, 0, 1, 2, 0}, {0, 0, 0, 1, 1, 2, 1, 2, 0},                            \
     {1, 1, 1, 2, 0, 2, 1, 2, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}};

#ifndef E_INK_WIDTH
#define E_INK_WIDTH 960
#endif
#ifndef E_INK_HEIGHT
#define E_INK_HEIGHT 540
#endif

#endif
#endif