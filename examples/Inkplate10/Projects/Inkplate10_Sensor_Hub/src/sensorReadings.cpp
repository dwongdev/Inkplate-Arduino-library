#include "sensorReadings.h"

// ---------------- Library objects ----------------
INA219 ina;
BME280 bme280;
Soldered_LSM6DSO lsm6dso;

// ---------------- Internal helpers ----------------
static bool readSimpleI2CWord(uint8_t address, uint16_t &value)
{
    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    uint8_t bytesRead = Wire.requestFrom((int)address, 2);
    if (bytesRead < 2)
    {
        return false;
    }

    uint8_t highByte = Wire.read();
    uint8_t lowByte = Wire.read();

    value = ((uint16_t)highByte << 8) | lowByte;
    return true;
}

static bool readSimpleI2CByte(uint8_t address, uint8_t &value)
{
    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    uint8_t bytesRead = Wire.requestFrom((int)address, 1);
    if (bytesRead < 1)
    {
        return false;
    }

    value = Wire.read();
    return true;
}

static void computePitchRoll(SensorData &data)
{
    data.pitchDeg = atan2f(-data.ax, sqrtf(data.ay * data.ay + data.az * data.az)) * 180.0f / PI;
    data.rollDeg = atan2f(data.ay, data.az) * 180.0f / PI;
}

// ---------------- Public functions ----------------
bool initSensors()
{
    bool anySensorStarted = false;

    // ---------------- BME280 ----------------
    bme280.begin();
    anySensorStarted = true;

    // ---------------- INA219 ----------------
    ina.begin();
    ina.configure(
        INA219_RANGE_32V,
        INA219_GAIN_320MV,
        INA219_BUS_RES_12BIT,
        INA219_SHUNT_RES_12BIT_1S
    );

    // Rshunt = 0.1 ohm, max expected current = 2 A
    ina.calibrate(0.1, 2.0);
    anySensorStarted = true;

    // ---------------- LSM6DSO ----------------
    lsm6dso.begin();

    if (lsm6dso.getVariant() == LSM6DSO_VARIANT_LSM6DSO32)
    {
        lsm6dso.setAcceleratorFullScale(32);
    }
    else
    {
        lsm6dso.setAcceleratorFullScale(16);
    }

    lsm6dso.enableAccelerator();
    lsm6dso.enableGyro();
    anySensorStarted = true;

    return anySensorStarted;
}

void readAllSensors(SensorData &data)
{
    // Reset status flags before reading
    data.bmeOk = false;
    data.inaOk = false;
    data.imuOk = false;
    data.potOk = false;
    data.hallOk = false;
    data.soilOk = false;
    data.pirOk = false;

    // ---------------- BME280 ----------------
    {
        float temperature;
        float humidity;
        float pressure;

        bme280.readSensorData(temperature, humidity, pressure);

        if (!isnan(temperature) && !isnan(humidity) && !isnan(pressure))
        {
            data.temperatureC = temperature;
            data.humidityPct = humidity;
            data.pressurehPa = pressure;
            data.bmeOk = true;
        }
    }

    // ---------------- INA219 ----------------
    {
        data.busVoltageV = ina.readBusVoltage();
        data.powermW = ina.readBusPower() * 1000.0f;
        data.shuntVoltagemV = ina.readShuntVoltage() * 1000.0f;
        data.currentmA = ina.readShuntCurrent() * 1000.0f;

        if (!isnan(data.busVoltageV))
        {
            data.inaOk = true;
        }
    }

    // ---------------- LSM6DSO ----------------
    {
        int32_t accel[3];
        int32_t gyro[3];

        lsm6dso.getAcceleratorAxes(accel);
        lsm6dso.getGyroAxes(gyro);

        // mg -> g
        data.ax = accel[0] / 1000.0f;
        data.ay = accel[1] / 1000.0f;
        data.az = accel[2] / 1000.0f;

        // mdps -> dps
        data.gx = gyro[0] / 1000.0f;
        data.gy = gyro[1] / 1000.0f;
        data.gz = gyro[2] / 1000.0f;

        computePitchRoll(data);
        data.imuOk = true;
    }

    // ---------------- Slider potentiometer ----------------
    {
        uint16_t raw = 0;
        if (readSimpleI2CWord(ADDR_POT, raw))
        {
            data.potentiometer = raw;
            data.potOk = true;
        }
    }

    // ---------------- Hall effect sensor ----------------
    {
        uint16_t raw = 0;
        if (readSimpleI2CWord(ADDR_HALL, raw))
        {
            data.hallRaw = raw;
            data.hallOk = true;
        }
    }

    // ---------------- Soil humidity sensor ----------------
    {
        uint16_t raw = 0;
        if (readSimpleI2CWord(ADDR_SOIL, raw))
        {
            data.soilRaw = raw;
            data.soilOk = true;
        }
    }

    // ---------------- PIR motion sensor ----------------
    {
        uint8_t raw = 0;
        if (readSimpleI2CByte(ADDR_PIR, raw))
        {
            data.pirDetected = (raw != 0);
            data.pirOk = true;
        }
    }
}

float soilPercentFromRaw(uint16_t raw)
{
    return clampf(100.0f - ((float)raw / 4095.0f) * 100.0f, 0.0f, 100.0f);
}

float potPercentFromRaw(uint16_t raw)
{
    return clampf(((float)raw / 4095.0f) * 100.0f, 0.0f, 100.0f);
}

float hallPercentFromRaw(uint16_t raw)
{
    return clampf(((float)raw / 4095.0f) * 100.0f, 0.0f, 100.0f);
}