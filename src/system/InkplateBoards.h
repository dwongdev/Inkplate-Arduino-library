/**
 **************************************************
 *
 * @file        InkplateBoards.h
 * @brief       File used for the Inkplate board selection. It selects
 *              correct Inkplate Driver code as well as correct peripherals,
 *              defines etc.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#ifndef __INKPLATE_BOARDS_H__
#define __INKPLATE_BOARDS_H__

// Board selector. It only includes files for selected board.
#ifdef ARDUINO_INKPLATE10V2
#include "../boards/Inkplate10/Inkplate10Driver.h"
#elif defined(ARDUINO_INKPLATE6V2)
#include "../boards/Inkplate6/Inkplate6Driver.h"
#elif defined(ARDUINO_INKPLATE5V2)
#include "../boards/Inkplate5V2/Inkplate5V2Driver.h"
#endif
#endif