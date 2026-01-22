#ifndef _FEATURE_SELECT_H
#define _FEATURE_SELECT_H

#ifdef ARDUINO_INKPLATE10V2
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#elif defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE5V2) || defined(ARDUINO_INKPLATECOLOR) ||                \
    defined(ARDUINO_INKPLATE13SPECTRA) || defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE10)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#elif defined(ARDUINO_INKPLATE6FLICK)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#include "touchscreen/touchCypress/TouchCypress.h"
#include "frontlight/Frontlight.h"
#elif defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#include "frontlight/Frontlight.h"
#include "touchscreen/touchElan/touchElan.h"
#endif
#endif