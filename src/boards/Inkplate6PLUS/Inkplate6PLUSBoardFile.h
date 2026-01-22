/**
 **************************************************
 *
 * @file        Inkplate6PLUSBoardFile.h
 * @brief       Wrrapper for the different Inkplate boards and
 *              it's classes.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Borna Biro for soldered.com
 ***************************************************/

// Header guard.
#ifndef __INKPLATE6PLUS_BOARD_SELECT_H__
#define __INKPLATE6PLUS_BOARD_SELECT_H__

// Board select check.
#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)

// Include Inkplate6PLUS board header file.
#include "Inkplate6PLUSDriver.h"

// Wrapper for different Inkplate boards.
class InkplateBoardClass : public EPDDriver
{
  public:
    InkplateBoardClass(){};
};

#endif
#endif