#ifndef __INKPLATE10V2_PINS_H__
#define __INKPLATE10V2_PINS_H__

#if defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE10)
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x21

// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN IO_PIN_B2 // 10

#define WAKEUP 3
#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        expander1.digitalWrite(WAKEUP, HIGH, true);                                                                    \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        expander1.digitalWrite(WAKEUP, LOW, true);                                                                     \
    }
#define PWRUP 4
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        expander1.digitalWrite(PWRUP, HIGH, true);                                                                     \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        expander1.digitalWrite(PWRUP, LOW, true);                                                                      \
    }
#define VCOM 5
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        expander1.digitalWrite(VCOM, HIGH, true);                                                                      \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        expander1.digitalWrite(VCOM, LOW, true);                                                                       \
    }
#define OE 0
#define OE_SET                                                                                                         \
    {                                                                                                                  \
        expander1.digitalWrite(OE, HIGH, true);                                                                        \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        expander1.digitalWrite(OE, LOW, true);                                                                         \
    }
#define GMOD 1
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        expander1.digitalWrite(GMOD, HIGH, true);                                                                      \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        expander1.digitalWrite(GMOD, LOW, true);                                                                       \
    }
#define SPV 2
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        expander1.digitalWrite(SPV, HIGH, true);                                                                       \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        expander1.digitalWrite(SPV, LOW, true);                                                                        \
    }
#define CL 0x01
#define CL_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = CL;                                                                                            \
    }
#define CL_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = CL;                                                                                            \
    }
#define CKV 0x01
#define CKV_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out1_w1ts.val = CKV;                                                                                      \
    }
#define CKV_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out1_w1tc.val = CKV;                                                                                      \
    }
#define SPH 0x02
#define SPH_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out1_w1ts.val = SPH;                                                                                      \
    }
#define SPH_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out1_w1tc.val = SPH;                                                                                      \
    }
#define LE 0x04
#define LE_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = LE;                                                                                            \
    }
#define LE_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = LE;                                                                                            \
    }

#endif
#endif