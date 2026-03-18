#pragma once

#include "includes.h"

bool initSensors();
void readAllSensors(SensorData &data);

// Conversion helpers for GUI / human-readable values
float soilPercentFromRaw(uint16_t raw);
float potPercentFromRaw(uint16_t raw);
float hallPercentFromRaw(uint16_t raw);