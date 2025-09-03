/**
 **************************************************
 *
 * @file        boardSelect.h
 * @brief       Main header file of the Inkpate board selector used to
 *              include proper files for the selected Inkplate board.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#ifndef __MY_TEST_LIBRARY_BOARD_SELECT_H__
#define __MY_TEST_LIBRARY_BOARD_SELECT_H__

#include <Arduino.h>

#ifdef ARDUINO_INKPLATE10V2
#include "boards/Inkplate10/Inkplate10BoardFile.h"
#else
#error "Board not selected!"
#endif

#endif