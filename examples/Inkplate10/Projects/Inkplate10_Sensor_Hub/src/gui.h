#pragma once

#include "includes.h"

void guiInit();
void guiDrawStaticFrame();
void guiDrawData(const SensorData &data, uint32_t updateCount, bool fullRefresh);