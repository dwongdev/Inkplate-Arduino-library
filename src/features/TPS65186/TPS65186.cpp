#include "TPS65186.h"
#include "../../system/inkplateSemaphore.h"

/**
 * @brief Initialise the PMIC: store pin references and program the power-up /
 *        power-down sequencer registers. WAKEUP is asserted briefly for the I2C
 *        write, then de-asserted.
 */
void TPS65186::begin(IOExpander *expander, uint8_t wakeupPin, uint8_t pwrupPin, uint8_t vcomPin)
{
    _expander = expander;
    _wakeupPin = wakeupPin;
    _pwrupPin = pwrupPin;
    _vcomPin = vcomPin;

    _expander->digitalWrite(_wakeupPin, HIGH, true);
    delay(1);
    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_UPSEQ0);
    Wire.write(0x1B); // Power up seq.
    Wire.write(0x00); // Power up delay (3 ms per rail)
    Wire.write(0x1B); // Power down seq.
    Wire.write(0x00); // Power down delay (6 ms per rail)
    Wire.endTransmission();
    i2cEnd();
    delay(1);
    _expander->digitalWrite(_wakeupPin, LOW, true);
}

/**
 * @brief Write the rail-enable register (0x01). Pass true to enable rails
 *        (0x20), false to disable them (0x00).
 */
bool TPS65186::enableRails(bool enable)
{
    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_ENABLE);
    Wire.write(enable ? 0x20 : 0x00);
    bool result = Wire.endTransmission() == 0;
    i2cEnd();
    return result;
}

/**
 * @brief Full PMIC power-up sequence: wake, enable rails, configure
 *        sequencers, assert PWRUP, wait for all rails to be good, then
 *        enable VCOM.
 *
 * @param timeout Maximum time in milliseconds to wait for PWR_GOOD.
 * @return true if all rails came up within the timeout, false otherwise.
 */
bool TPS65186::powerUp(uint16_t timeout)
{
    _expander->digitalWrite(_wakeupPin, HIGH, true);
    delay(5);

    enableRails(true);

    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_UPSEQ0);
    Wire.write(0xE4);
    Wire.endTransmission();
    i2cEnd();

    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_DWNSEQ0);
    Wire.write(0x1B);
    Wire.endTransmission();
    i2cEnd();

    _expander->digitalWrite(_pwrupPin, HIGH, true);

    unsigned long timer = millis();
    do
    {
        delay(1);
    } while ((readPowerGood() != PWR_GOOD_OK) && (millis() - timer) < timeout);

    if ((millis() - timer) >= timeout)
        return false;

    _expander->digitalWrite(_vcomPin, HIGH, true);
    _poweredUp = true;
    return true;
}

/**
 * @brief Full PMIC power-down sequence: disable VCOM, de-assert PWRUP,
 *        wait for all rails to collapse, then de-assert WAKEUP and disable
 *        rails via I2C.
 */
void TPS65186::powerDown()
{
    _expander->digitalWrite(_vcomPin, LOW, true);
    _expander->digitalWrite(_pwrupPin, LOW, true);

    unsigned long timer = millis();
    do
    {
        delay(1);
    } while ((readPowerGood() != 0) && (millis() - timer) < 250);

    _expander->digitalWrite(_wakeupPin, LOW, true);
    enableRails(false);
    _poweredUp = false;
}

/**
 * @brief Read the panel temperature from the TPS65186 internal sensor.
 *        If the PMIC is not already powered up the function temporarily
 *        wakes it, takes the reading, then powers it back down.
 *
 * @return Temperature in °C (range -10 to 85, ±1 °C accuracy from 0–50 °C).
 */
int8_t TPS65186::readTemperature()
{
    int8_t temp;
    bool wakeForTemp = !_poweredUp;

    if (wakeForTemp)
    {
        _expander->digitalWrite(_wakeupPin, HIGH, true);
        _expander->digitalWrite(_pwrupPin, HIGH, true);
        delay(5);
    }

    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_TMST1);
    Wire.write(0x80);
    Wire.endTransmission();
    i2cEnd();
    delay(5);

    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_TEMP);
    Wire.endTransmission();
    Wire.requestFrom(TPS65186_I2C_ADDR, 1);
    temp = Wire.read();
    i2cEnd();

    if (wakeForTemp)
    {
        _expander->digitalWrite(_pwrupPin, LOW, true);
        _expander->digitalWrite(_wakeupPin, LOW, true);
        delay(5);
    }
    return temp;
}

/**
 * @brief Check whether all PMIC power rails have reached their target
 *        voltage (PWR_GOOD_OK).
 */
bool TPS65186::isPowerGood()
{
    return readPowerGood() == PWR_GOOD_OK;
}

/**
 * @brief Read the raw power-good status register (0x0F).
 */
uint8_t TPS65186::readPowerGood()
{
    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(TPS65186_REG_PWRGOOD);
    Wire.endTransmission();
    Wire.requestFrom(TPS65186_I2C_ADDR, 1);
    uint8_t result = Wire.read();
    i2cEnd();
    return result;
}

/**
 * @brief Write a single byte to a TPS65186 register.
 */
void TPS65186::writeReg(uint8_t reg, uint8_t data)
{
    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief Read a single byte from a TPS65186 register.
 */
uint8_t TPS65186::readReg(uint8_t reg)
{
    i2cStart();
    Wire.beginTransmission(TPS65186_I2C_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    uint8_t got = Wire.requestFrom(TPS65186_I2C_ADDR, (uint8_t)1);
    uint8_t result = got ? Wire.read() : 0xFF;
    i2cEnd();
    return result;
}
