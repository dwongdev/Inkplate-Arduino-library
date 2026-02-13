#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Inkplate.h"

// Helvetica-like fonts (Adafruit_GFX FreeFonts)
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

// mbedTLS base64 for Basic auth header
#include "mbedtls/base64.h"


static const char *WIFI_SSID = "Soldered Electronics";
static const char *WIFI_PASSWORD = "dasduino";

// Spotify Developer App credentials
static const char *SPOTIFY_CLIENT_ID     = "396d5f95bfb741219a5404a16b4c421e";
static const char *SPOTIFY_CLIENT_SECRET = "9e80e6ba89ab42a68779e79952dd192d";

// Refresh token for your Spotify account (one-time generated on PC)
static const char *SPOTIFY_REFRESH_TOKEN = "AQBZi4q5pL-lG4OuqfnQIHxpCqlFx-BVZTrYALcef8R46R8SjXXglmqYh9Y3fOU1T3ZQkRgIQbsikR8nUVyztVb1K8r_EnvLLHe-AlfYS6J4tl2sJwLJNJZ2xdy-5lCk2CA";

// -------------------------
// SLEEP / POLL CONFIG
// -------------------------
static const uint32_t POLL_SECONDS = 120;  // during playback, wake every 120s (also used when idle)

// -------------------------
// PRE-AGREED LAYOUT CONSTANTS
// (Set these to match your paper sketch)
// -------------------------
// Screen is large; choose constants that match your planned design.
// Tune these numbers once on-device until it matches your sketch.

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

// Colors (Inkplate 13 Spectra supports multiple colors; keep it simple for now)
static const uint8_t BG_DARK   = BLACK;
static const uint8_t FG_LIGHT  = WHITE;
static const uint8_t FG_DARK   = BLACK;

// -------------------------
// INKPLATE INSTANCE
// -------------------------
Inkplate display; // 3-bit mode for Spectra (7 colors). Keep UI simple.

// -------------------------
// STATE ACROSS DEEP SLEEP
// -------------------------
// RTC memory persists through deep sleep resets on ESP32.
RTC_DATA_ATTR char lastAlbumId[96] = {0};
RTC_DATA_ATTR bool lastWasNothingPlaying = true;

// -------------------------
// HELPERS
// -------------------------

static String base64Encode(const String &in)
{
  size_t outLen = 0;
  // Base64 expands by ~4/3; allocate safe.
  size_t bufLen = (in.length() * 4) / 3 + 16;
  uint8_t *out = (uint8_t *)malloc(bufLen);
  if (!out) return "";

  int rc = mbedtls_base64_encode(out, bufLen, &outLen,
                                (const uint8_t *)in.c_str(), in.length());
  if (rc != 0) {
    free(out);
    return "";
  }
  String s((char *)out, outLen);
  free(out);
  return s;
}

static void goToSleep(uint32_t seconds)
{
  Serial.printf("Deep sleeping for %u seconds...\n", seconds);
  // Give serial a moment to flush
  delay(50);

  esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
  esp_deep_sleep_start();
}

static bool connectWiFi()
{
  Serial.printf("Connecting to Wi-Fi: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if (millis() - start > 20000) {
      Serial.println("\nWi-Fi connect timeout");
      return false;
    }
  }
  Serial.printf("\nWi-Fi connected. IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

static bool spotifyRefreshAccessToken(String &outAccessToken)
{
  WiFiClientSecure client;
  client.setInsecure(); // NOTE: For production, pin certs; kept simple for example.

  HTTPClient http;
  const char *tokenUrl = "https://accounts.spotify.com/api/token";

  if (!http.begin(client, tokenUrl)) {
    Serial.println("HTTP begin failed (token)");
    return false;
  }

  String basic = String(SPOTIFY_CLIENT_ID) + ":" + String(SPOTIFY_CLIENT_SECRET);
  String authHeader = "Basic " + base64Encode(basic);

  http.addHeader("Authorization", authHeader);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String body = "grant_type=refresh_token&refresh_token=" + String(SPOTIFY_REFRESH_TOKEN);

  int code = http.POST(body);
  String payload = http.getString();
  http.end();

  if (code != 200) {
    Serial.printf("Token refresh failed. HTTP %d\n%s\n", code, payload.c_str());
    return false;
  }

  StaticJsonDocument<1536> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.printf("Token JSON parse error: %s\n", err.c_str());
    return false;
  }

  const char *token = doc["access_token"];
  if (!token || !strlen(token)) {
    Serial.println("No access_token in response");
    return false;
  }

  outAccessToken = token;
  Serial.println("Got access token.");
  return true;
}

// Parses currently-playing into album-focused metadata.
// Returns:
//   - true  => request succeeded (even if nothing playing)
//   - false => request failed
static bool spotifyGetCurrentlyPlaying(
  const String &accessToken,
  bool &outIsPlaying,
  String &outAlbumId,
  String &outAlbumName,
  String &outArtistName,
  String &outTrackName,
  String &outImageUrl
) {
  outIsPlaying = false;
  outAlbumId = "";
  outAlbumName = "";
  outArtistName = "";
  outTrackName = "";
  outImageUrl = "";

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  // One endpoint to determine playback + retrieve album metadata + image URL
  const char *url = "https://api.spotify.com/v1/me/player/currently-playing";

  if (!http.begin(client, url)) {
    Serial.println("HTTP begin failed (currently-playing)");
    return false;
  }

  http.addHeader("Authorization", "Bearer " + accessToken);
  http.addHeader("Accept", "application/json");

  int code = http.GET();

  // 204 = No Content (nothing playing)
  if (code == 204) {
    Serial.println("Spotify: nothing playing (204).");
    http.end();
    outIsPlaying = false;
    return true;
  }

  String payload = http.getString();
  http.end();

  if (code != 200) {
    Serial.printf("Currently-playing failed. HTTP %d\n%s\n", code, payload.c_str());
    return false;
  }

  // Parse JSON (keep doc moderate; Spotify payload can be big, but we only need a few fields)
  StaticJsonDocument<24 * 1024> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.printf("Currently-playing JSON parse error: %s\n", err.c_str());
    return false;
  }

  outIsPlaying = doc["is_playing"] | false;
  if (!outIsPlaying) {
    Serial.println("Spotify: not playing (is_playing=false).");
    return true;
  }

  // Usually item is a track when music plays
  JsonObject item = doc["item"];
  if (item.isNull()) {
    Serial.println("Spotify: playing but item is null.");
    return true;
  }

  const char *itemType = item["type"] | "";
  if (String(itemType) != "track") {
    Serial.printf("Spotify: item type is '%s' (expected 'track').\n", itemType);
    // Still try to treat it as not supported for display
    outIsPlaying = false;
    return true;
  }

  outTrackName = (const char *)(item["name"] | "");

  JsonObject album = item["album"];
  outAlbumId   = (const char *)(album["id"] | "");
  outAlbumName = (const char *)(album["name"] | "");

  // Artist: prefer first artist
  JsonArray artists = item["artists"].as<JsonArray>();
  if (!artists.isNull() && artists.size() > 0) {
    outArtistName = (const char *)(artists[0]["name"] | "");
  }

  // Image: pick the first in album.images (usually largest)
  JsonArray images = album["images"].as<JsonArray>();
  if (!images.isNull() && images.size() > 0) {
    outImageUrl = (const char *)(images[0]["url"] | "");
  }

  Serial.println("Spotify: playing track parsed:");
  Serial.printf("  albumId: %s\n", outAlbumId.c_str());
  Serial.printf("  album:   %s\n", outAlbumName.c_str());
  Serial.printf("  artist:  %s\n", outArtistName.c_str());
  Serial.printf("  track:   %s\n", outTrackName.c_str());
  Serial.printf("  image:   %s\n", outImageUrl.c_str());

  return true;
}

// Basic text wrapping in a rectangle using Adafruit_GFX getTextBounds.
// (Not perfect typography, but good enough for a demo sketch.)
static void drawWrappedText(const String &text, int x, int y, int w, int h, int lineGapPx)
{
  int16_t x1, y1;
  uint16_t tw, th;

  int cursorX = x;
  int cursorY = y;

  String remaining = text;
  remaining.trim();

  while (remaining.length() > 0) {
    // Stop if next line would exceed bounds
    display.getTextBounds("Ag", 0, 0, &x1, &y1, &tw, &th);
    if (cursorY + (int)th > y + h) break;

    // Greedy line build
    int bestCut = -1;
    int cut = 0;

    // If there's no spaces, hard cut
    bool hasSpace = remaining.indexOf(' ') >= 0;

    while (cut < (int)remaining.length()) {
      int nextSpace = remaining.indexOf(' ', cut);
      if (nextSpace < 0) nextSpace = remaining.length();

      String candidate = remaining.substring(0, nextSpace);
      candidate.trim();

      display.getTextBounds(candidate.c_str(), 0, 0, &x1, &y1, &tw, &th);
      if ((int)tw <= w) {
        bestCut = nextSpace;
        cut = nextSpace + 1;
        if (nextSpace == (int)remaining.length()) break;
      } else {
        break;
      }
    }

    if (bestCut < 0) {
      if (!hasSpace) {
        // Hard cut long word
        int hard = min((int)remaining.length(), 20);
        String line = remaining.substring(0, hard);
        display.setCursor(cursorX, cursorY);
        display.print(line);
        remaining = remaining.substring(hard);
        remaining.trim();
      } else {
        // One word longer than width; print it anyway (will overflow)
        int sp = remaining.indexOf(' ');
        if (sp < 0) sp = remaining.length();
        String line = remaining.substring(0, sp);
        display.setCursor(cursorX, cursorY);
        display.print(line);
        remaining = remaining.substring(sp);
        remaining.trim();
      }
    } else {
      String line = remaining.substring(0, bestCut);
      line.trim();
      display.setCursor(cursorX, cursorY);
      display.print(line);
      remaining = remaining.substring(bestCut);
      remaining.trim();
    }

    cursorY += (int)th + lineGapPx;
  }
}

static void renderNothingPlaying()
{
  Serial.println("Rendering: Nothing playing screen");
  display.clearDisplay();

  // Dark background
  display.fillRect(0, 0, display.width(), display.height(), BG_DARK);

  display.setTextColor(FG_LIGHT);
  display.setFont(&FreeSansBold24pt7b);

  const char *msg = "Nothing playing";
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

  int cx = (display.width() - (int)w) / 2;
  int cy = (display.height() + (int)h) / 2;

  display.setCursor(cx, cy);
  display.print(msg);

  // Full refresh (Spectra)
  display.display();
}

static void renderAlbumScreen(const String &albumName, const String &artistName, const String &trackName, const String &imageUrl)
{
  Serial.println("Rendering: Album screen");
  display.clearDisplay();
  display.fillRect(0, 0, display.width(), display.height(), WHITE);

  // Cover placeholder frame
  display.drawRect(COVER_X, COVER_Y, COVER_W, COVER_H, FG_DARK);

  // --- Album cover art ---
  // This uses Inkplate's helper to fetch and draw an image from URL.
  // It should handle JPEG/PNG depending on your Inkplate library build.
  //
  // If your Inkplate library version does NOT have drawImage(url, x, y, w, h),
  // you can:
  //   - update Inkplate library, or
  //   - download the JPEG into a buffer and decode it (TJpg_Decoder) to the display.
  //
  // For a simple demo, we attempt to fit image into the cover rectangle.
  bool imgOk = false;

  // Some Inkplate versions provide drawImage(url, x, y, invert, dither) OR similar.
  // This call signature may vary by version. The most common convenient one is:
  //   display.drawImage(url, x, y, w, h);
  //
  // If compilation fails here, check your Inkplate drawImage example in your library
  // and adjust this one line to match the installed API.
  Serial.print("image url=");
  Serial.println(imageUrl.c_str());
  imgOk = display.image.draw(imageUrl.c_str(), COVER_X, COVER_Y, COVER_W, COVER_H);

  if (!imgOk) {
    Serial.println("WARN: Failed to draw cover image (image.draw returned false).");
    display.setTextColor(FG_DARK);
    display.setFont(&FreeSans12pt7b);
    display.setCursor(COVER_X + 20, COVER_Y + 40);
    display.print("Image load failed");
  }

  // --- Text area ---
  display.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, FG_DARK);

  // Album title (bold)
  display.setTextColor(FG_DARK);
  display.setFont(&FreeSansBold24pt7b);
  drawWrappedText(albumName, TEXT_X + 20, TEXT_Y + 60, TEXT_W - 40, 180, 10);

  // Artist (regular)
  display.setFont(&FreeSans18pt7b);
  drawWrappedText(artistName, TEXT_X + 20, TEXT_Y + 240, TEXT_W - 40, 120, 8);

  // Optional: track name (smaller)
  if (trackName.length() > 0) {
    display.setFont(&FreeSans12pt7b);
    String t = String("Track: ") + trackName;
    drawWrappedText(t, TEXT_X + 20, TEXT_Y + 340, TEXT_W - 40, 80, 6);
  }

  // Status/footer
  display.setFont(&FreeSans9pt7b);
  display.setCursor(STATUS_X, STATUS_Y);
  display.print("Spotify Now Playing (album-focused)");

  // Full refresh (Spectra)
  display.display();
}

// -------------------------
// MAIN
// -------------------------
void setup()
{
  Serial.begin(115200);
  delay(200);

  // Inkplate init
  display.begin();
  // Optional: speed up Wi-Fi on some boards by disabling not-needed peripherals, etc.

  Serial.println("\n--- Inkplate Spotify Album Example ---");

  if (!connectWiFi()) {
    // If Wi-Fi fails, go to sleep and try later
    goToSleep(POLL_SECONDS);
    return;
  }

  // Get access token via refresh token
  String accessToken;
  if (!spotifyRefreshAccessToken(accessToken)) {
    // If token fails, sleep and try later
    goToSleep(POLL_SECONDS);
    return;
  }

  // Query currently playing
  bool isPlaying = false;
  String albumId, albumName, artistName, trackName, imageUrl;

  if (!spotifyGetCurrentlyPlaying(accessToken, isPlaying, albumId, albumName, artistName, trackName, imageUrl)) {
    // API call failed; sleep and retry later
    goToSleep(POLL_SECONDS);
    return;
  }

  // -------------------------
  // Refresh rule (must-have)
  // -------------------------
  // Only refresh e-paper when album ID changes; otherwise skip refresh and go back to sleep.
  // This avoids slow refresh annoyance and reduces unnecessary panel wear.

  if (!isPlaying) {
    if (!lastWasNothingPlaying) {
      Serial.println("State changed: was playing -> now nothing playing. Updating display.");
      renderNothingPlaying();
      lastWasNothingPlaying = true;
      lastAlbumId[0] = 0;
    } else {
      Serial.println("Still nothing playing. Skipping display update.");
    }
    goToSleep(POLL_SECONDS);
    return;
  }

  // If playing but albumId missing, treat as "nothing playing" for UI purposes
  if (albumId.length() == 0) {
    Serial.println("Playing but albumId empty. Showing Nothing playing UI.");
    if (!lastWasNothingPlaying) {
      renderNothingPlaying();
      lastWasNothingPlaying = true;
      lastAlbumId[0] = 0;
    } else {
      Serial.println("Still in nothing-playing UI state. Skipping display update.");
    }
    goToSleep(POLL_SECONDS);
    return;
  }

  // Compare with last album ID stored in RTC memory
  bool albumSame = (strncmp(lastAlbumId, albumId.c_str(), sizeof(lastAlbumId) - 1) == 0);

  if (albumSame && !lastWasNothingPlaying) {
    Serial.println("Album unchanged since last wake. Skipping e-paper refresh.");
  } else {
    Serial.println("Album changed (or returning from nothing-playing). Updating display.");
    renderAlbumScreen(albumName, artistName, trackName, imageUrl);

    // Store new state
    strncpy(lastAlbumId, albumId.c_str(), sizeof(lastAlbumId) - 1);
    lastAlbumId[sizeof(lastAlbumId) - 1] = 0;
    lastWasNothingPlaying = false;
  }

  goToSleep(POLL_SECONDS);
}

void loop()
{
  // Not used: we deep-sleep at end of setup().
}