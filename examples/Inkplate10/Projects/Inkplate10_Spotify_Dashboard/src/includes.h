#pragma once

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Inkplate.h"

// Helvetica-like fonts (Adafruit_GFX FreeFonts)
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSansBold18pt7b.h"
#include "fonts/FreeSansBold24pt7b.h"

// mbedTLS base64 for Basic auth header
#include "mbedtls/base64.h"


static const uint32_t POLL_SECONDS = 120;  // wake every 120s


static const int COVER_X = 60;
static const int COVER_Y = 60;
static const int COVER_W = 520;
static const int COVER_H = 520;

static const int TEXT_X  = 640;
static const int TEXT_Y  = 120;
static const int TEXT_W  = 800;
static const int TEXT_H  = 420;

static const int STATUS_X = 640;
static const int STATUS_Y = 560;


static const uint8_t BG_DARK   = BLACK;
static const uint8_t FG_LIGHT  = WHITE;
static const uint8_t FG_DARK   = BLACK;


struct SpotifyNowPlaying
{
    bool isPlaying = false;
    String albumId;
    String albumName;
    String artistName;
    String trackName;
    String imageUrl;
    uint32_t durationMs=0;
    uint32_t progressMs =0;
};
