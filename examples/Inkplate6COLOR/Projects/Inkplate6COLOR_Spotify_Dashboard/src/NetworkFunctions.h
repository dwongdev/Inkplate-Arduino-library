#pragma once
#include "includes.h"

class NetworkFunctions
{
public:
    NetworkFunctions(const char* wifiSsid,
                     const char* wifiPassword,
                     const char* spotifyClientId,
                     const char* spotifyClientSecret,
                     const char* spotifyRefreshToken);

    bool connectWiFi(uint32_t timeoutMs = 20000);

    bool spotifyRefreshAccessToken(String& outAccessToken);

    // Returns:
    //   true  => request succeeded (even if nothing playing)
    //   false => request failed
    bool spotifyGetCurrentlyPlaying(const String& accessToken, SpotifyNowPlaying& out);

private:
    const char* _wifiSsid;
    const char* _wifiPassword;

    const char* _clientId;
    const char* _clientSecret;
    const char* _refreshToken;

    String base64Encode(const String& in);
};
