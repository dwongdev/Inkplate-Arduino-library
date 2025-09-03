#ifndef __WAVEFROMS_INKPLATE_10_H__
#define __WAVEFROMS_INKPLATE_10_H__

#ifdef ARDUINO_INKPLATE10V2

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0},                            \
     {0, 1, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 2, 2, 2, 2, 2, 2, 1, 0},                            \
     {0, 0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};



// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN IO_PIN_B2 // 10

#define E_INK_WIDTH  1200
#define E_INK_HEIGHT 825

#endif
#endif