#include "NetworkFunctions.h"

void NetworkFunctions::begin(char *ssid, char *pass)
{
    // Works even if WiFi is already connected by display.connectWiFi()
    WiFi.mode(WIFI_STA);
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, pass);

        int cnt = 0;
        Serial.print(F("Waiting for WiFi to connect..."));
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(F("."));
            delay(1000);
            if (++cnt == 20)
            {
                Serial.println("Can't connect to WIFI, restarting");
                delay(100);
                ESP.restart();
            }
        }
        Serial.println(F(" connected"));
    }

    // Create HTTPS client once
    if (timeApiClient == nullptr)
    {
        timeApiClient = new WiFiClientSecure();
        timeApiClient->setHandshakeTimeout(30);
        timeApiClient->setInsecure();

        // IMPORTANT: milliseconds (5ms breaks HTTP)
        timeApiClient->setTimeout(15000);
    }
}

bool NetworkFunctions::getAllCities()
{
    if (timeApiClient == nullptr)
    {
        Serial.println("NetworkFunctions not initialized! Call network.begin() first.");
        return false;
    }

    DynamicJsonDocument doc(24576);

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected!");
        return false;
    }

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    const char *request = "/api/TimeZone/AvailableTimeZones";

    HTTPClient http;
    if (!http.begin(*timeApiClient, "timeapi.io", 443, request, true))
    {
        Serial.println("Couldn't connect to timeAPI!");
        WiFi.setSleep(sleep);
        return false;
    }

    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(8000);
    http.setConnectTimeout(8000);

    int httpCode = http.GET();
    String response = http.getString();
    http.end();

    WiFi.setSleep(sleep);

    if (!(httpCode == 200 || httpCode == 201))
    {
        Serial.print("TimeAPI HTTP error: ");
        Serial.print(httpCode);
        Serial.println("!");
        return false;
    }

    DeserializationError error = deserializeJson(doc, response);
    if (error)
    {
        Serial.print("deserializeJson failed: ");
        Serial.println(error.c_str());
        return false;
    }

    JsonArray arr = doc.as<JsonArray>();
    numCities = 0;

    for (JsonVariant v : arr)
    {
        if (numCities >= 650) break;
        const char *tz = v.as<const char *>();
        if (!tz) continue;

        strncpy(allFetchedCities[numCities], tz, 32);
        allFetchedCities[numCities][32] = '\0';
        numCities++;
    }

    Serial.print("Fetched numCities=");
    Serial.println(numCities);

    return (numCities > 0);
}

bool NetworkFunctions::getData(char *city, int *hours, int *minutes)
{
    if (timeApiClient == nullptr)
    {
        Serial.println("NetworkFunctions not initialized! Call network.begin() first.");
        return false;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected!");
        return false;
    }

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    bool cityFound = false;
    char request[160];

    for (int i = 0; i < numCities; i++)
    {
        if (strstr(allFetchedCities[i], city))
        {
            snprintf(request, sizeof(request), "/api/Time/current/zone?timeZone=%s", allFetchedCities[i]);
            cityFound = true;
            break;
        }
    }

    if (!cityFound)
    {
        Serial.println("City not found!");
        WiFi.setSleep(sleep);
        return false;
    }

    HTTPClient http;
    if (!http.begin(*timeApiClient, "timeapi.io", 443, request, true))
    {
        Serial.println("Couldn't connect to timeAPI!");
        WiFi.setSleep(sleep);
        return false;
    }

    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(8000);
    http.setConnectTimeout(8000);

    int httpCode = http.GET();
    String response = http.getString();
    http.end();

    WiFi.setSleep(sleep);

    if (!(httpCode == 200 || httpCode == 201))
    {
        Serial.print("TimeAPI HTTP error: ");
        Serial.print(httpCode);
        Serial.println("!");
        return false;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    if (error)
    {
        Serial.print("deserializeJson failed: ");
        Serial.println(error.c_str());
        return false;
    }

    *hours = doc["hour"] | 0;
    *minutes = doc["minute"] | 0;

    return true;
}

char *NetworkFunctions::getFullCityName(char *city)
{
    static char fullName[33];

    for (int i = 0; i < numCities; i++)
    {
        if (strstr(allFetchedCities[i], city))
        {
            strcpy(fullName, allFetchedCities[i]);
            return fullName;
        }
    }

    return 0;
}