#ifndef _FEATURE_SELECT_H
#define _FEATURE_SELECT_H

#ifdef ARDUINO_INKPLATE10V2
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#elif defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE5V2)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#endif
#endif