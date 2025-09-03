#ifndef __INKPLATE10DRIVER_H__
#define __INKPLATE10DRIVER_H__

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE10V2

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 10"

// Include main header file for the Arduino.
#include "Arduino.h"

// Include library for PCAL6416A GPIO expander.
#include "../../system/pcalExpander.h"

#include "pins.h"

// Include waveforms for EPD
#include "waveforms.h"

#include "Wire.h"

#include "EEPROM.h"

#include "../../graphics/GraphicsDefs.h"


#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        internalIO.digitalWriteIO(WAKEUP, HIGH);                                                                       \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        internalIO.digitalWriteIO(WAKEUP, LOW);                                                                        \
    }
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        internalIO.digitalWriteIO(PWRUP, HIGH);                                                                        \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        internalIO.digitalWriteIO(PWRUP, LOW);                                                                         \
    }
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        internalIO.digitalWriteIO(VCOM, HIGH);                                                                         \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        internalIO.digitalWriteIO(VCOM, LOW);                                                                          \
    }
#define OE_SET                                                                                                         \
    {                                                                                                                  \
        internalIO.digitalWriteIO(OE, HIGH);                                                                           \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        internalIO.digitalWriteIO(OE, LOW);                                                                            \
    }
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        internalIO.digitalWriteIO(GMOD, HIGH);                                                                         \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        internalIO.digitalWriteIO(GMOD, LOW);                                                                          \
    }
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        internalIO.digitalWriteIO(SPV, HIGH);                                                                          \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        internalIO.digitalWriteIO(SPV, LOW);                                                                           \
    }

class Inkplate;


class EPDDriver
{
  public:
    int initDriver(Inkplate *_inkplatePtr);
    void display(bool _leaveOn = 0);
    void selectDisplayMode(uint8_t displayMode);
    void clearDisplay();
    uint32_t partialUpdate(bool _forced = false, bool leaveOn = false);
    void setFullUpdateThreshold(uint16_t _numberOfPartialUpdates);
    uint8_t getDisplayMode();


    IOExpander internalIO;
    IOExpander externalIO;

    uint8_t _beginDone = 0;
    uint8_t _displayMode;


    uint32_t pinLUT[256];
    uint32_t *GLUT;
    uint32_t *GLUT2;
    uint8_t *DMemoryNew;
    uint8_t *_partial;
    uint8_t *DMemory4Bit;
    uint8_t *_pBuffer;
    uint8_t waveform3Bit[8][9] = WAVEFORM3BIT;
    uint16_t _partialUpdateLimiter = 10;
    uint16_t _partialUpdateCounter = 0;
    uint8_t _blockPartial = 1;


  private:
    void calculateLUTs();
    uint8_t readPowerGood();
    void pinsAsOutputs();
    int einkOn();
    void einkOff();
    void display1b(bool _leaveOn);
    void display3b(bool _leaveOn);
    void pinsZstate();
    uint8_t getPanelState();
    void setPanelState(uint8_t state);
    void clean(uint8_t c, uint8_t rep);
    void vscan_start();
    void hscan_start(uint32_t _d);
    void vscan_end();
    uint8_t _panelState = 0;
};


#endif
#endif