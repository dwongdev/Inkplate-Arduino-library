/*
    Inkplate13SPECTRA_Spotify_Album Example
    Compatible with Soldered Inkplate 13SPECTRA

    Getting Started:
    For setup and documentation, visit: https://soldered.com/documentation/inkplate/

    Overview:
    This example demonstrates how to fetch and display album data from Spotify API using
    the Inkplate 13SPECTRA e-paper display.

    Before You Start:
        - you need a Spotify account
        
        note: the following process needs to be done only once. 
        - visit "https://developer.spotify.com/" -> Dashboard
            * Create a new app, fill out *App name**, *App description*, *Redirect URIs* and *APIs used* fields
                note: Redirect URIs can be a local address, but it needs to be in format: http://127.0.0.1:8888/callback
                note: In APIs used field select *Web API*

            * Copy *Client ID* and *Client Secret* and paste them into *SPOTIFY_CLIENT_ID* and *SPOTIFY_CLIENT_SECRET* variables
        
        - navigate to "src/spotify-token" and open token.js file in code editor of your choice, fill out *CLIENT_ID*, *CLIENT_SECRET* and *REDIRECT_URI* fields.
            * Open terimnal inside this folder and run command `node token.js` this function will send your api information to Spotify and will generate last piece
             that you need to use the Spotify API,the *REFRESH_TOKEN*.

            * Open the login page in browser of your choosing and search for "refresh_token", copy it into field **SPOTIFY_REFRESH_TOKEN**
        
        -You can now use the example!

    Overview:
    Thie example demonstrates how to connect your Inkplate 13SPECTRA to Spotify's WEB API service, parse received Json data and
    display it on the e-paper display.

    17 February 2026
*/      

#include "src/includes.h"
#include "src/NetworkFunctions.h"
#include "src/Gui.h"


// CREDENTIALS / SECRETS
static const char* WIFI_SSID     = "Soldered Electronics";
static const char* WIFI_PASSWORD = "dasduino";

static const char* SPOTIFY_CLIENT_ID     = "396d5f95bfb741219a5404a16b4c421e";
static const char* SPOTIFY_CLIENT_SECRET = "9e80e6ba89ab42a68779e79952dd192d";
static const char* SPOTIFY_REFRESH_TOKEN = "AQBZi4q5pL-lG4OuqfnQIHxpCqlFx-BVZTrYALcef8R46R8SjXXglmqYh9Y3fOU1T3ZQkRgIQbsikR8nUVyztVb1K8r_EnvLLHe-AlfYS6J4tl2sJwLJNJZ2xdy-5lCk2CA";


Inkplate display; 
Gui gui(display);


RTC_DATA_ATTR char lastAlbumId[96] = {0};
RTC_DATA_ATTR bool lastWasNothingPlaying = true;


static void goToSleep(uint32_t seconds)
{
    Serial.printf("Deep sleeping for %u seconds...\n", seconds);
    delay(50);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    gui.begin();

    Serial.println("\n--- Inkplate Spotify Album Dashboard ---");

    NetworkFunctions net(
        WIFI_SSID, WIFI_PASSWORD,
        SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET,
        SPOTIFY_REFRESH_TOKEN
    );

    if (!net.connectWiFi())
    {
        goToSleep(POLL_SECONDS);
        return;
    }

    String accessToken;
    if (!net.spotifyRefreshAccessToken(accessToken))
    {
        goToSleep(POLL_SECONDS);
        return;
    }

    SpotifyNowPlaying now;
    if (!net.spotifyGetCurrentlyPlaying(accessToken, now))
    {
        goToSleep(POLL_SECONDS);
        return;
    }


    if (!now.isPlaying)
    {
        if (!lastWasNothingPlaying)
        {
            Serial.println("State changed: was playing -> now nothing playing. Updating display.");
            gui.renderNothingPlaying();
            lastWasNothingPlaying = true;
            lastAlbumId[0] = 0;
        }
        else
        {
            Serial.println("Still nothing playing. Skipping display update.");
        }

        goToSleep(POLL_SECONDS);
        return;
    }

    if (now.albumId.length() == 0)
    {
        Serial.println("Playing but albumId empty. Showing Nothing playing UI.");
        if (!lastWasNothingPlaying)
        {
            gui.renderNothingPlaying();
            lastWasNothingPlaying = true;
            lastAlbumId[0] = 0;
        }
        else
        {
            Serial.println("Still in nothing-playing UI state. Skipping display update.");
        }

        goToSleep(POLL_SECONDS);
        return;
    }

    bool albumSame = (strncmp(lastAlbumId, now.albumId.c_str(), sizeof(lastAlbumId) - 1) == 0);

    if (albumSame && !lastWasNothingPlaying)
    {
        Serial.println("Album unchanged since last wake. Skipping e-paper refresh.");
    }
    else
    {
        Serial.println("Album changed (or returning from nothing-playing). Updating display.");
        gui.renderAlbumScreen(now.albumName, now.artistName, now.trackName, now.imageUrl, now.progressMs, now.durationMs);

        strncpy(lastAlbumId, now.albumId.c_str(), sizeof(lastAlbumId) - 1);
        lastAlbumId[sizeof(lastAlbumId) - 1] = 0;
        lastWasNothingPlaying = false;
    }

    goToSleep(POLL_SECONDS);
}

void loop()
{
    // Not used: device deep-sleeps at end of setup()
}

