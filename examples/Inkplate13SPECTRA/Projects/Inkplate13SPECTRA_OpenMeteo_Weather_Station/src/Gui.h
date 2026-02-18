#ifndef GUI_H
#define GUI_H

#include "Inkplate.h"
#include "NetworkFunctions.h"
#include "WeatherData.h"
#include <Arduino.h>

class Gui
{

public:
    Gui(Inkplate &inkplate);
    void drawBackground();
    void displayWeatherData(WeatherData *weatherData, NetworkFunctions::UserInfo *userInfo);
    void wifiError();
    void apiError();
    int batteryLevel;
    double voltage;

private:
    Inkplate &inkplate;
    void drawTemperaturePrecipGraph(WeatherData *weatherData, NetworkFunctions::UserInfo *userInfo);
    const uint8_t* getWeatherIcon(int code);
    const uint8_t* getBatteryIcon(int percentage);
    int voltageToPercentage(double voltage);

};

#endif
