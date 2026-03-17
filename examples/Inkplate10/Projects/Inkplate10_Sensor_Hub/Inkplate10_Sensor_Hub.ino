#include "src/includes.h"
#include "src/sensorReadings.h"
#include "src/gui.h"

Inkplate display(INKPLATE_1BIT);

SensorData sensorData;

static uint32_t lastSensorReadMs = 0;
static uint32_t lastGuiRefreshMs = 0;
static uint32_t updateCounter = 0;
static uint8_t partialRefreshCounter = 0;

void setup()
{
    Serial.begin(115200);
    delay(500);

    Wire.begin();

    display.begin();
    display.setRotation(0);

    guiInit();
    guiDrawStaticFrame();
    display.display();

    bool sensorInitOk = initSensors();

    Serial.println();
    Serial.println("Inkplate Sensor Hub started.");
    Serial.print("At least one sensor initialized: ");
    Serial.println(sensorInitOk ? "YES" : "NO");
}

void loop()
{
    uint32_t now = millis();

    if (now - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS)
    {
        lastSensorReadMs = now;
        readAllSensors(sensorData);
    }

    if (now - lastGuiRefreshMs >= GUI_REFRESH_INTERVAL_MS)
    {
        lastGuiRefreshMs = now;

        bool doFullRefresh = false;
        partialRefreshCounter++;

        if (partialRefreshCounter >= FULL_REFRESH_EVERY_N_UPDATES)
        {
            partialRefreshCounter = 0;
            doFullRefresh = true;
        }

        guiDrawData(sensorData, updateCounter, doFullRefresh);

        if (doFullRefresh)
        {
            display.display();
        }
        else
        {
            display.partialUpdate();
        }

        updateCounter++;
    }
}