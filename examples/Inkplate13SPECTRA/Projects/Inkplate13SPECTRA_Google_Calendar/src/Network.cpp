#include "Network.h"

// Constructor to initialize WiFi and Google Calendar credentials
NetworkTEST::NetworkTEST(const String& calendarID, const String& apiKey)
    : calendarID(calendarID), apiKey(apiKey) {}

// Fetches calendar events from Google Calendar and fills in the data structure
bool NetworkTEST::fetchCalendar(calendarData* data) {
    struct tm timeinfo;

    // Retry loop to wait for time sync (up to 10 seconds)
    int attempts = 0;
    while (!getLocalTime(&timeinfo) && attempts < 10) {
        Serial.println("Waiting for time sync...");
        delay(1000);
        attempts++;
    }

    if (!getLocalTime(&timeinfo)) {
        Serial.println("Cannot fetch calendar - time not available");
        return false;
    }

    // Set start time to today's date at 00:00:00 UTC
    char timeMin[64];
    strftime(timeMin, sizeof(timeMin), "%Y-%m-%dT00:00:00Z", &timeinfo);

    // Set end time to 14 days from now at 23:59:59 UTC
    time_t now = mktime(&timeinfo);
    now += 14 * 24 * 60 * 60;  // Add 14 days
    struct tm timeMaxInfo;
    gmtime_r(&now, &timeMaxInfo);  // Convert to UTC time

    char timeMax[64];
    strftime(timeMax, sizeof(timeMax), "%Y-%m-%dT23:59:59Z", &timeMaxInfo);

    data->clearEvents();

    String pageToken="";
    while(true){
        // Build the Google Calendar API URL
        String url = "https://www.googleapis.com/calendar/v3/calendars/" + calendarID +
                    "/events?singleEvents=true&orderBy=startTime&" +
                    "timeMin=" + String(timeMin) +
                    "&timeMax=" + String(timeMax) +
                    "&maxResults=60&key=" + apiKey;

        if(pageToken.length()>0){
            url += "&pageToken=" + urlEncode(pageToken);
        }

        Serial.println("Requesting URL:");
        Serial.println(url);

        // Make HTTP GET request to fetch calendar events
        HTTPClient http;
        http.begin(url);
        int httpCode = http.GET();

        if (httpCode != 200) {
            Serial.println("HTTP error: " + String(httpCode));
            return false;
        }

        String payload = http.getString();
        //Serial.println(payload);

        http.end();

        // Parse JSON response
        StaticJsonDocument<8192> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.println("JSON parse error");
            return false;
        }

        // Clear existing events and add new ones
        data->clearEvents();
        JsonArray items = doc["items"].as<JsonArray>();
        Serial.printf("Page items: %d\n", (int)items.size());
        for (JsonObject event : items) {
            String summary = event["summary"] | "No Title";
            JsonObject start = event["start"];
            JsonObject end = event["end"];
            String startTime = String((start["dateTime"] | start["date"]).as<const char*>());
            String endTime = String((end["dateTime"] | end["date"]).as<const char*>());

            data->addEvent(summary, startTime, endTime);
            /*if(data->getEventCount() >= 20){
                return true;
            }*/
        }
        if(doc.containsKey("nextPageToken")){
            String newToken = doc["nextPageToken"].as<String>();
            Serial.println("Has nextPageToken");
            Serial.println("nextPageToken = " + newToken);
            if(newToken.length()==0 || newToken == pageToken){
                Serial.println("pagination token stopped changing");
                break;
            }
            pageToken=newToken;
        }
        else{
            Serial.println("No nextPageToken");
            break;
        }
    }

    return true;
}


String NetworkTEST::urlEncode(const String &s){
    String out;
    const char *hex="0123456789ABCDEF";
    for(size_t i = 0;i<s.length();i++){
        unsigned char c = (unsigned char)s[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||(c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            out += (char)c;
        } 
        else {
            out += '%';
            out += hex[(c >> 4) & 0xF];
            out += hex[c & 0xF];
        }
    }
    return out;
}