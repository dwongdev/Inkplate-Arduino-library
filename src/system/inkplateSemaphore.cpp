/**
 **************************************************
 * @file        inkplateSemaphore.cpp
 * @brief       FreeRTOS mutex definitions and initialisation for thread-safe
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

#include "inkplateSemaphore.h"

SemaphoreHandle_t mutexI2C = NULL;
SemaphoreHandle_t mutexSPI = NULL;
SemaphoreHandle_t mutexDisplay = NULL;

void inkplateMutexInit()
{
    mutexI2C = xSemaphoreCreateRecursiveMutex();
    mutexSPI = xSemaphoreCreateRecursiveMutex();
    mutexDisplay = xSemaphoreCreateRecursiveMutex();
}
