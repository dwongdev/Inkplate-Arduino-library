/*
  QuotableNetwork.h
  Inkplate 6 Arduino library
  David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
  September 24, 2020
  https://github.com/e-radionicacom/Inkplate-6-Arduino-library

  For support, please reach over forums: forum.e-radionica.com/en
  For more info about the product, please check: www.inkplate.io

  This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
  Please review the LICENSE file included with this example.
  If you have any questions about licensing, please contact techsupport@e-radionica.com
  Distributed as-is; no warranty is given.
*/



#ifndef QUOTABLESNETWORK_H
#define QUOTABLESNETWORK_H

#include "Arduino.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Wifi ssid and password
extern char ssid[];
extern char pass[];

// All functions defined in Network.cpp

class NetworkFunctions
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    bool getData(char* text, char* auth);

  private:
    // Functions called from within our class
};

#endif

