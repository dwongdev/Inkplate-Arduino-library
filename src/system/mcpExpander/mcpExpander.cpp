/**
 **************************************************
 * @file        Mcp.cpp
 * @brief       class for controling io exapnder expander
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
 * @authors     Soldered
 ***************************************************/
#if defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE6PLUS)
#include "mcpExpander.h"

/**
 * @brief       begin function starts MCP23017 expander and reads registers.
 *
 * @param       uint8_t _addr
 *              IO expander I2C address
 *
 * @return      true if successful, false otherwise
 */
bool IOExpander::begin(uint8_t _addr)
{
    _ioExpanderI2CAddress = _addr;

    Wire.beginTransmission(_ioExpanderI2CAddress);
    int error = Wire.endTransmission();
    if (error)
        return false;

    readMCPRegisters();

#ifdef IO_INT_ADDR
    if (_addr == IO_INT_ADDR)
    {
        for (uint8_t pin = 0; pin < 9; pin++)
        {
            blockPinUsage(pin);
        }
    }
#endif

    return true;
}

/**
 * @brief       readMCPRegisters function uses I2C to read all MCP registers to avoid read-modify-write.
 */
void IOExpander::readMCPRegisters()
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, (uint8_t)22);
    for (int i = 0; i < 22; i++)
    {
        _ioExpanderRegs[i] = Wire.read();
    }
}

/**
 * @brief       readMCPRegisters function uses I2C to read selected MCP registers.
 *
 * @param       uint8_t _regIndex
 *              Start index of the MCP23017 registers
 * @param       uint8_t _n
 *              number of bytes/registers to read
 */
void IOExpander::readMCPRegisters(uint8_t _regIndex, uint8_t _n)
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(regAddresses[_regIndex]);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, _n);
    for (int i = 0; i < _n; i++)
    {
        _ioExpanderRegs[_regIndex + i] = Wire.read();
    }
}

/**
 * @brief       readMCPRegister function uses I2C to read one selected MCP register.
 *
 * @param       uint8_t _regIndex
 *              Start index of the MCP23017 registers
 */
void IOExpander::readMCPRegister(uint8_t _regIndex)
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(regAddresses[_regIndex]);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, (uint8_t)1);
    _ioExpanderRegs[_regIndex] = Wire.read();
}

/**
 * @brief       updateMCPAllRegisters function uses I2C to update all MCP registers.
 */
void IOExpander::updateMCPAllRegisters()
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 22; i++)
    {
        Wire.write(_ioExpanderRegs[i]);
    }
    Wire.endTransmission();
}

/**
 * @brief       updateMCPRegister function uses I2C to update selected MCP register.
 *
 * @param       uint8_t _regIndex
 *              Start index of the MCP23017 registers
 * @param       uint8_t _d
 *              data to be uploaded
 */
void IOExpander::updateMCPRegister(uint8_t _regIndex, uint8_t _d)
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(regAddresses[_regIndex]);
    Wire.write(_d);
    Wire.endTransmission();
}

/**
 * @brief       pinMode sets I/O expander pin mode.
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02, INPUT_PULLUP=0x05)
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 */
void IOExpander::pinMode(uint8_t _pin, uint8_t _mode, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return;

    pinModeInternal(_pin, _mode);
}

/**
 * @brief       digitalWrite sets I/O expander output pin state.
 *
 * @param       uint8_t _pin
 *              pin to set output
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 */
void IOExpander::digitalWrite(uint8_t _pin, uint8_t _state, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return;

    digitalWriteInternal(_pin, _state);
}

/**
 * @brief       digitalRead reads I/O expander pin state.
 *
 * @param       uint8_t _pin
 *              pin to read
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t IOExpander::digitalRead(uint8_t _pin, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return 0;

    return digitalReadInternal(_pin);
}

/**
 * @brief       setIntPin enables interrupt on change on IO Expander pin.
 *
 * @param       uint8_t _pin
 *              pin to set interrupt on
 */
void IOExpander::setIntPin(uint8_t _pin)
{
    setIntPinInternal(_pin);
}

/**
 * @brief       removeIntPin removes interrupt from pin.
 *
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 */
void IOExpander::removeIntPin(uint8_t _pin)
{
    removeIntPinInternal(_pin);
}

/**
 * @brief       getInt reads interrupt register state.
 *
 * @return      returns interrupt registers state
 */
uint16_t IOExpander::getInt()
{
    return getINTInternal();
}

/**
 * @brief       setPorts sets states on every IO Expander pin at once.
 *
 * @param       uint16_t _d
 *              GPIO pin state of all IO Expander pins.
 */
void IOExpander::setPorts(uint16_t _d)
{
    setPortsInternal(_d);
}

/**
 * @brief       getPorts reads GPIO pin state on every IO Expander pin at once.
 */
uint16_t IOExpander::getPorts()
{
    return getPortsInternal();
}

/**
 * @brief   Sets block on specific pin so user could not use it.
 *
 * @param   uint8_t _pin
 *          I/O expander GPIO pin (IO_PIN_A0 - IO_PIN_A7, IO_PIN_B0 - IO_PIN_B7).
 */
void IOExpander::blockPinUsage(uint8_t _pin)
{
    _pin &= 15;
    _blockedPinsForUser |= 1ULL << _pin;
}

/**
 * @brief   Remove block on specific pin so user could use it.
 *
 * @param   uint8_t _pin
 *          I/O expander GPIO pin (IO_PIN_A0 - IO_PIN_A7, IO_PIN_B0 - IO_PIN_B7).
 */
void IOExpander::unblockPinUsage(uint8_t _pin)
{
    _pin &= 15;
    _blockedPinsForUser &= ~(1ULL << _pin);
}

/**
 * @brief       pinModeInternal sets MCP expander pin mode.
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02, INPUT_PULLUP=0x05)
 */
void IOExpander::pinModeInternal(uint8_t _pin, uint8_t _mode)
{
    if (_pin > 15)
        return;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case INPUT:
        _ioExpanderRegs[MCP23017_IODIRA + _port] |= 1 << _p;
        _ioExpanderRegs[MCP23017_GPPUA + _port] &= ~(1 << _p);
        updateMCPRegister(MCP23017_IODIRA + _port, _ioExpanderRegs[MCP23017_IODIRA + _port]);
        updateMCPRegister(MCP23017_GPPUA + _port, _ioExpanderRegs[MCP23017_GPPUA + _port]);
        break;

    case INPUT_PULLUP:
        _ioExpanderRegs[MCP23017_IODIRA + _port] |= 1 << _p;
        _ioExpanderRegs[MCP23017_GPPUA + _port] |= 1 << _p;
        updateMCPRegister(MCP23017_IODIRA + _port, _ioExpanderRegs[MCP23017_IODIRA + _port]);
        updateMCPRegister(MCP23017_GPPUA + _port, _ioExpanderRegs[MCP23017_GPPUA + _port]);
        break;

    case OUTPUT:
        _ioExpanderRegs[MCP23017_IODIRA + _port] &= ~(1 << _p);
        _ioExpanderRegs[MCP23017_GPPUA + _port] &= ~(1 << _p);
        updateMCPRegister(MCP23017_IODIRA + _port, _ioExpanderRegs[MCP23017_IODIRA + _port]);
        updateMCPRegister(MCP23017_GPPUA + _port, _ioExpanderRegs[MCP23017_GPPUA + _port]);
        break;
    }
}

/**
 * @brief       digitalWriteInternal sets MCP expander output pin state.
 *
 * @param       uint8_t _pin
 *              pin to set output
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 */
void IOExpander::digitalWriteInternal(uint8_t _pin, uint8_t _state)
{
    if (_pin > 15)
        return;
    _state &= 1;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    _state ? (_ioExpanderRegs[MCP23017_GPIOA + _port] |= (1 << _p))
           : (_ioExpanderRegs[MCP23017_GPIOA + _port] &= ~(1 << _p));
    updateMCPRegister(MCP23017_GPIOA + _port, _ioExpanderRegs[MCP23017_GPIOA + _port]);
}

/**
 * @brief       digitalReadInternal reads MCP expander pin state.
 *
 * @param       uint8_t _pin
 *              pin to read
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t IOExpander::digitalReadInternal(uint8_t _pin)
{
    if (_pin > 15)
        return 0;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    readMCPRegister(MCP23017_GPIOA + _port);
    return (_ioExpanderRegs[MCP23017_GPIOA + _port] & (1 << _p)) ? HIGH : LOW;
}

/**
 * @brief       setIntPinInternal enables interrupt on change for selected pin.
 *
 * @param       uint8_t _pin
 *              selected pin
 */
void IOExpander::setIntPinInternal(uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    _ioExpanderRegs[MCP23017_INTCONA + _port] &= ~(1 << _p);
    _ioExpanderRegs[MCP23017_GPINTENA + _port] |= (1 << _p);

    updateMCPRegister(MCP23017_INTCONA + _port, _ioExpanderRegs[MCP23017_INTCONA + _port]);
    updateMCPRegister(MCP23017_GPINTENA + _port, _ioExpanderRegs[MCP23017_GPINTENA + _port]);
}

/**
 * @brief       removeIntPinInternal removes interrupt from selected pin.
 *
 * @param       uint8_t _pin
 *              selected pin
 */
void IOExpander::removeIntPinInternal(uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    _ioExpanderRegs[MCP23017_GPINTENA + _port] &= ~(1 << _p);
    updateMCPRegister(MCP23017_GPINTENA + _port, _ioExpanderRegs[MCP23017_GPINTENA + _port]);
}

/**
 * @brief       getINTInternal reads interrupt pin state for all pins.
 *
 * @return      returns interrupt state of both ports (INTF)
 */
uint16_t IOExpander::getINTInternal()
{
    readMCPRegisters(MCP23017_INTFA, 2);
    return ((_ioExpanderRegs[MCP23017_INTFB] << 8) | _ioExpanderRegs[MCP23017_INTFA]);
}

/**
 * @brief       setPortsInternal sets all pins at once.
 *
 * @param       uint16_t _d
 *              GPIO data. Every bit represents one GPIO pin.
 */
void IOExpander::setPortsInternal(uint16_t _d)
{
    _ioExpanderRegs[MCP23017_GPIOA] = _d & 0xff;
    _ioExpanderRegs[MCP23017_GPIOB] = (_d >> 8) & 0xff;
    updateMCPRegister(MCP23017_GPIOA, _ioExpanderRegs[MCP23017_GPIOA]);
    updateMCPRegister(MCP23017_GPIOB, _ioExpanderRegs[MCP23017_GPIOB]);
}

/**
 * @brief       getPortsInternal reads all pins at once.
 *
 * @return      GPIO data. Every bit represents one GPIO pin.
 */
uint16_t IOExpander::getPortsInternal()
{
    readMCPRegisters(MCP23017_GPIOA, 2);
    return (_ioExpanderRegs[MCP23017_GPIOA] | (_ioExpanderRegs[MCP23017_GPIOB] << 8));
}

/**
 * @brief   Checks for the pins that users are not allowed to use.
 *
 * @param   uint8_t _pin
 *          GPIO pin on the I/O expander
 * @return  bool
 *          true - Pin is blocked for the user
 * @return  false - User can use this pin.
 */
bool IOExpander::checkForBlockedPins(uint8_t _pin)
{
    if (_blockedPinsForUser & (1ULL << _pin))
        return true;

    return false;
}
#endif