/**
 **************************************************
 * @file        NetworkClient.h
 * @brief       Basic funtions for working with network
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#ifndef __NETWORKCLIENT_H__
#define __NETWORKCLIENT_H__

#include "Arduino.h"
#include "HTTPClient.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "WiFiMulti.h"
#include "defines.h"
#include "time.h"
#include <string.h>

/**
 * @brief       BitmapHeader structure that includes standard bitmap parameters
 */
struct bitmapHeader
{
    uint16_t signature;     // Is picture a legal BMP
    uint32_t fileSize;      // Size of image in bytes
    uint32_t startRAW;      // Address where raw data (pixel array) can be found/is started
    uint32_t dibHeaderSize; // Size of the header in bytes
    uint32_t width;         // Width of image
    uint32_t height;        // Height of image
    uint16_t color;         // The number of bits per pixel, which is the color depth of
                            // the image. Typical values are 1, 4, 8, 16, 24 and 32
    uint32_t compression;   // The compression method being used. See the next table
                            // for a list of possible values
};

#define WIFI_TIMEOUT 23

/**
 * @brief       NetworkClient class that holds standard functions for working
 * with network
 */
class NetworkClient
{
  public:
    bool connectWiFi(const char *ssid, const char *pass, int timeout = WIFI_TIMEOUT, bool printToSerial = false);
    bool connectWiFiMulti(int numNetworks, const char **ssids, const char **passwords, int timeout = WIFI_TIMEOUT,
                          bool printToSerial = false);
    void disconnect();
    bool isConnected();

    uint8_t *downloadFile(const char *url, int32_t *defaultLen);
    uint8_t *downloadFileHTTPS(const char *url, int32_t *defaultLen);
    uint8_t *downloadFile(WiFiClient *url, int32_t len);
    void applyHttpsCertificate(const char *certificate);

    // The default parameters for nptServer here are cast to (char*) to keep the compiler happy
    bool getNTPEpoch(time_t *timeEpoch, int timeZone = 0, char *ntpServer = (char *)"pool.ntp.org",
                     int daylightSavingsOffsetHours = 0);
    bool getNTPDateTime(tm *timeEpoch, int timeZone = 0, char *ntpServer = (char *)"pool.ntp.org",
                        int daylightSavingsOffsetHours = 0);

    void setFollowRedirects(followRedirects_t f);

  private:
    WiFiClientSecure *client;
    followRedirects_t followRedirects;
    char *getHostFromURL(const char *urlToGetHostFrom);
    char *getPathToResourceFromURL(const char *urlToGetPathToResourceFrom);
    char *certificate;
};

#endif