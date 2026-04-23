#include "NetworkFunctions.h"

NetworkFunctions::NetworkFunctions(const char* wifiSsid,
                                   const char* wifiPassword,
                                   const char* spotifyClientId,
                                   const char* spotifyClientSecret,
                                   const char* spotifyRefreshToken)
: _wifiSsid(wifiSsid),
  _wifiPassword(wifiPassword),
  _clientId(spotifyClientId),
  _clientSecret(spotifyClientSecret),
  _refreshToken(spotifyRefreshToken)
{
}

String NetworkFunctions::base64Encode(const String& in)
{
    size_t outLen = 0;
    size_t bufLen = (in.length() * 4) / 3 + 16;
    uint8_t* out = (uint8_t*)malloc(bufLen);
    if (!out) return "";

    int rc = mbedtls_base64_encode(out, bufLen, &outLen,
                                  (const uint8_t*)in.c_str(), in.length());
    if (rc != 0)
    {
        free(out);
        return "";
    }

    String s((char*)out, outLen);
    free(out);
    return s;
}

bool NetworkFunctions::connectWiFi(uint32_t timeoutMs)
{
    Serial.printf("Connecting to Wi-Fi: %s\n", _wifiSsid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSsid, _wifiPassword);

    const uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(".");
        if (millis() - start > timeoutMs)
        {
            Serial.println("\nWi-Fi connect timeout");
            return false;
        }
    }

    Serial.printf("\nWi-Fi connected. IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
}

bool NetworkFunctions::spotifyRefreshAccessToken(String& outAccessToken)
{
    WiFiClientSecure client;
    client.setInsecure(); // For production: pin certs

    HTTPClient http;
    const char* tokenUrl = "https://accounts.spotify.com/api/token";

    if (!http.begin(client, tokenUrl))
    {
        Serial.println("HTTP begin failed (token)");
        return false;
    }

    String basic = String(_clientId) + ":" + String(_clientSecret);
    String authHeader = "Basic " + base64Encode(basic);

    http.addHeader("Authorization", authHeader);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body = "grant_type=refresh_token&refresh_token=" + String(_refreshToken);

    int code = http.POST(body);
    String payload = http.getString();
    http.end();

    if (code != 200)
    {
        Serial.printf("Token refresh failed. HTTP %d\n%s\n", code, payload.c_str());
        return false;
    }

    StaticJsonDocument<1536> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err)
    {
        Serial.printf("Token JSON parse error: %s\n", err.c_str());
        return false;
    }

    const char* token = doc["access_token"];
    if (!token || !strlen(token))
    {
        Serial.println("No access_token in response");
        return false;
    }

    outAccessToken = token;
    Serial.println("Got access token.");
    return true;
}

bool NetworkFunctions::spotifyGetCurrentlyPlaying(const String& accessToken, SpotifyNowPlaying& out)
{
    out = SpotifyNowPlaying{}; // reset

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    const char* url = "https://api.spotify.com/v1/me/player/currently-playing";

    if (!http.begin(client, url))
    {
        Serial.println("HTTP begin failed (currently-playing)");
        return false;
    }

    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Accept", "application/json");

    int code = http.GET();

    if (code == 204)
    {
        Serial.println("Spotify: nothing playing (204).");
        http.end();
        out.isPlaying = false;
        return true;
    }

    String payload = http.getString();
    http.end();

    if (code != 200)
    {
        Serial.printf("Currently-playing failed. HTTP %d\n%s\n", code, payload.c_str());
        return false;
    }

    StaticJsonDocument<24 * 1024> doc;
    DeserializationError err = deserializeJson(doc, payload);

    if (err)
    {
        Serial.printf("Currently-playing JSON parse error: %s\n", err.c_str());
        return false;
    }

    out.isPlaying = doc["is_playing"] | false;

    if(doc["progress_ms"].isNull()){
        out.progressMs=0;
    }
    else{
        out.progressMs = doc["progress_ms"].as<uint32_t>();
    }
    if (!out.isPlaying)
    {
        Serial.println("Spotify: not playing (is_playing=false).");
        return true;
    }
    JsonObject item = doc["item"];
    if (item.isNull())
    {
        Serial.println("Spotify: playing but item is null.");
        return true;
    }
    out.durationMs=item["duration_ms"] | 0;


    const char* itemType = item["type"] | "";
    if (String(itemType) != "track")
    {
        Serial.printf("Spotify: item type is '%s' (expected 'track').\n", itemType);
        out.isPlaying = false;
        return true;
    }

    out.trackName = (const char*)(item["name"] | "");

    JsonObject album = item["album"];
    out.albumId   = (const char*)(album["id"] | "");
    out.albumName = (const char*)(album["name"] | "");

    JsonArray artists = item["artists"].as<JsonArray>();
    if (!artists.isNull() && artists.size() > 0)
    {
        out.artistName = (const char*)(artists[0]["name"] | "");
    }

    JsonArray images = album["images"].as<JsonArray>();
    if (!images.isNull() && images.size() > 0)
    {
        out.imageUrl = (const char*)(images[1]["url"] | "");
    }

    Serial.println("Spotify: playing track parsed:");
    Serial.printf("  albumId: %s\n", out.albumId.c_str());
    Serial.printf("  album:   %s\n", out.albumName.c_str());
    Serial.printf("  artist:  %s\n", out.artistName.c_str());
    Serial.printf("  track:   %s\n", out.trackName.c_str());
    Serial.printf("  image:   %s\n", out.imageUrl.c_str());

    return true;
}
