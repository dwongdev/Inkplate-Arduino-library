/**
 **************************************************
 * @file        inkplateSemaphore.h
 * @brief       FreeRTOS mutex handles and convenience macros for thread-safe
 *              I2C and SPI access on Inkplate boards
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *              License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example. If you
 *              have any questions about licensing, please contact
 *              assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t mutexI2C;
extern SemaphoreHandle_t mutexSPI;
extern SemaphoreHandle_t mutexDisplay;

void inkplateMutexInit();

#define i2cStart()     xSemaphoreTakeRecursive(mutexI2C, portMAX_DELAY)
#define i2cEnd()       xSemaphoreGiveRecursive(mutexI2C)
#define spiStart()     xSemaphoreTakeRecursive(mutexSPI, portMAX_DELAY)
#define spiEnd()       xSemaphoreGiveRecursive(mutexSPI)
#define displayStart() xSemaphoreTakeRecursive(mutexDisplay, portMAX_DELAY)
#define displayEnd()   xSemaphoreGiveRecursive(mutexDisplay)
