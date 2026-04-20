/**
 **************************************************
 *
 * @file        InkplateBoardFile.h
 * @brief       Wrrapper for the different Inkplate boards and
 *              it's classes.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Borna Biro for soldered.com
 ***************************************************/

// Header guard.
#ifndef __INKPLATE_BOARD_SELECT_H__
#define __INKPLATE_BOARD_SELECT_H__

// Include Inkplate10 board header file.
#include "driverSelect.h"

// Wrapper for different Inkplate boards.
class InkplateBoardClass : public EPDDriver
{
  public:
    InkplateBoardClass(){};
};

#endif