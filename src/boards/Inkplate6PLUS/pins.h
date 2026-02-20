#ifndef __INKPLATE6PLUS_PINS_H__
#define __INKPLATE6PLUS_PINS_H__
#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#if defined(ARDUINO_INKPLATE6PLUSV2)
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x21
#else
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x22
#endif

#define SD_PMOS_PIN IO_PIN_B5 // 13

#define INKPLATE_NORMAL_PWR_MODE 0x11
#define INKPLATE_USB_PWR_ONLY    0x88

#define FRONTLIGHT_EN 11

// Touchscreen defines
#define TOUCHSCREEN_EN          12
#define TOUCHSCREEN_RST         10
#define TOUCHSCREEN_INT         36
#define TOUCHSCREEN_ADDR        0x15
#define TOUCHSCREEN_IO_EXPANDER IO_INT_ADDR
#define TOUCHSCREEN_IO_REGS     ioRegsInt

#define WAKEUP 3
#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        internalIO.digitalWrite(WAKEUP, HIGH, true);                                                                         \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        internalIO.digitalWrite(WAKEUP, LOW, true);                                                                          \
    }
#define PWRUP 4
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        internalIO.digitalWrite(PWRUP, HIGH, true);                                                                          \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        internalIO.digitalWrite(PWRUP, LOW, true);                                                                           \
    }
#define VCOM 5
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        internalIO.digitalWrite(VCOM, HIGH, true);                                                                           \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        internalIO.digitalWrite(VCOM, LOW, true);                                                                            \
    }
#define OE 0
#define OE_SET                                                                                                         \
    {                                                                                                                  \
        internalIO.digitalWrite(OE, HIGH, true);                                                                             \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        internalIO.digitalWrite(OE, LOW, true);                                                                              \
    }
#define GMOD 1
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        internalIO.digitalWrite(GMOD, HIGH, true);                                                                           \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        internalIO.digitalWrite(GMOD, LOW, true);                                                                            \
    }
#define SPV 2
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        internalIO.digitalWrite(SPV, HIGH, true);                                                                            \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        internalIO.digitalWrite(SPV, LOW, true);                                                                             \
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