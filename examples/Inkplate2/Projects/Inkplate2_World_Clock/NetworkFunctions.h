/*
NetworkFunctions.h
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For more info about the product, please check: https://docs.soldered.com/inkplate/

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Arduino.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H

/**
 * @brief           Gelper class for all the network operations
 * 
*/
class NetworkFunctions
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    char *getFullCityName(char *city);
    bool getData(char *city, int * hours, int * minutes);
    bool getAllCities();

  private:
    char allFetchedCities[650][33];
    int numCities = 0;
    WiFiClientSecure *timeApiClient;
};

#endif
