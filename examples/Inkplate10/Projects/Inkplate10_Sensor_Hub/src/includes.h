#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#include <Inkplate.h>

// Soldered / compatible sensor libraries
#include <INA219-SOLDERED.h>
#include <BME280-SOLDERED.h>


#include <LSM6DSO-SOLDERED.h>

extern Inkplate display;

#define SENSOR_READ_INTERVAL_MS 250
#define GUI_REFRESH_INTERVAL_MS 400
#define FULL_REFRESH_EVERY_N_UPDATES 40

// ---------------- Screen ----------------
#define SCREEN_W 1200
#define SCREEN_H 825

#define MARGIN 18
#define GAP 12

#define TOP_H 72
#define BOTTOM_H 120

#define LEFT_W 360
#define RIGHT_W 360
#define CENTER_W (SCREEN_W - (2 * MARGIN) - LEFT_W - RIGHT_W - (2 * GAP))
#define MID_H (SCREEN_H - (2 * MARGIN) - TOP_H - BOTTOM_H - (2 * GAP))

// ---------------- I2C addresses ----------------
#define ADDR_POT  0x30
#define ADDR_SOIL 0x31
#define ADDR_PIR  0x32
#define ADDR_HALL 0x34

#define ADDR_INA219 0x40
#define ADDR_BME280 0x76
#define ADDR_LSM6DSO 0x6B


struct SensorData
{
    // Environment
    float temperatureC = NAN;
    float humidityPct = NAN;
    float pressurehPa = NAN;

    // Power
    float busVoltageV = NAN;
    float shuntVoltagemV = NAN;
    float currentmA = NAN;
    float powermW = NAN;

    // Simple Qwiic boards
    uint16_t potentiometer = 0;
    uint16_t hallRaw = 0;
    uint16_t soilRaw = 0;
    bool pirDetected = false;

    // IMU raw
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 1.0f;

    float gx = 0.0f;
    float gy = 0.0f;
    float gz = 0.0f;

    // Derived orientation
    float pitchDeg = 0.0f;
    float rollDeg = 0.0f;

    // Status flags
    bool bmeOk = false;
    bool inaOk = false;
    bool imuOk = false;
    bool potOk = false;
    bool hallOk = false;
    bool soilOk = false;
    bool pirOk = false;
};


inline float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

inline int mapClamped(int x, int inMin, int inMax, int outMin, int outMax)
{
    x = constrain(x, inMin, inMax);
    return map(x, inMin, inMax, outMin, outMax);
}