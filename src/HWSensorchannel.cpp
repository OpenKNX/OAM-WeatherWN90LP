#include "HWSensorchannel.h"
#include <Wire.h>

void preTransmission()
{
    digitalWrite(RS485_UART_DIR_PIN, 1);
}

void postTransmission()
{
    digitalWrite(RS485_UART_DIR_PIN, 0);
}

HWSensorchannel::HWSensorchannel()
{
    mutex_init(&mx1);
    mutex_init(&mx2);
    mutex_init(&mx3);
}

void HWSensorchannel::Setup(uint8_t adr, uint8_t channel_number)
{
    m_adr = adr;
    _channelIndex = channel_number;

    m_modbus.preTransmission(preTransmission);
    m_modbus.postTransmission(postTransmission);
    m_modbus.begin(ParamW90_Address_, RS485_SERIAL);
}

bool HWSensorchannel::Loop()
{
    // ToDo Switch case
    if(delayCheckMillis(m_temperature_last_poll_millis, 20000))
    {
        SetTemperature(ReadTemperature());
        m_temperature_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_humidity_last_poll_millis, 20000))
    {
        SetHumidity(ReadHumidity());
        m_humidity_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_pressure_last_poll_millis, 20000))
    {
        SetPressure(ReadPressure());
        m_pressure_last_poll_millis = millis();
        return true;
    }
    return true;
}

float HWSensorchannel::ReadTemperature()
{

    /*
    Value in hex
10.5 C = 1F9h
-10.5 C = 127h
with 400 offset added
(Range: -40.0C ->
60.0C)
If invalid fill with
0xFFFF
    */

    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x0167, 0x0001);
    if (result == m_modbus.ku8MBSuccess)
    {
        data = m_modbus.getResponseBuffer(0);
        // convert and return
        float rawdata = (data - 400) / 10.0;
        logDebugP("Temp: %f °C", rawdata);
        return rawdata;

    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::ReadHumidity()
{

    /*
data in hex
(Range: 1% - 99%)
If invalid fill with
0xFFFF
    */

    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x0168, 0x0001);
    if (result == m_modbus.ku8MBSuccess)
    {
        data = m_modbus.getResponseBuffer(0);
        // convert and return
        float rawdata = data * 1.0;
        logDebugP("Hum: %f %", rawdata);
        return rawdata;

    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::ReadPressure()
{

    /*
Value in hex
ABS = value*0.1hPa
1002.6hPa=272AH
If invalid fill with
0xFFFF
    */

    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x016D, 0x0001);
    if (result == m_modbus.ku8MBSuccess)
    {
        data = m_modbus.getResponseBuffer(0);
        // convert and return
        float rawdata = data * 10.0;
        logDebugP("Pressure: %f Pa", rawdata);
        return rawdata;

    }
    else
    {
        return NAN;
    }
}

float ReadLight()
{
/* 165
Value in hex
Light=value*10
(Range: 0lux ->
300,000lux)
If invalid fill with
0xFFFF
*/
    return NAN;
}

uint8_t ReadUVI()
{
/* 166
Value in hex Uvi=UVI
value/10
(Range: 0 -> 150)
If invalid fill with
0xFFFF
*/
    return 0;
}

float ReadWind()
{
/* 169
Value in hex
If invalid fill with
0xFFFF.Wind Speed =
WIND
value*0.1m/s(0~40m/s)
*/
    return NAN;
}

float ReadGust()
{
/* 16A
Value in hex
If invalid fill with
0xFFFF.Gust Speed =
GUST
value*0.1m/s(0~40m/s)
*/
    return NAN;
}

uint8_t ReadWindDir()
{
/* 16B
Value in hex
(Range: 0°- 359°)
If invalid fill with
0xFFFF
*/
    return 0;
}

float ReadRain()
{
/* 016E
data in hex
Rain = value*0.01mm
0.18mm=12H
*/
    return NAN;
}





float HWSensorchannel::GetTemperature()
{
    float retval;
    if(mutex_try_enter(&mx1, nullptr))
    {
        retval = m_temperature;
        mutex_exit(&mx1);
        return retval;
    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::GetHumidity()
{
    float retval;
    if(mutex_try_enter(&mx2, nullptr))
    {
        retval = m_humidity;
        mutex_exit(&mx2);
        return retval;
    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::GetPressure()
{
    float retval;
    if(mutex_try_enter(&mx3, nullptr))
    {
        retval = m_pressure;
        mutex_exit(&mx3);
        return retval;
    }
    else
    {
        return NAN;
    }
}

void HWSensorchannel::SetTemperature(float temperature)
{
    logDebugP("Temp value %f", temperature);
    mutex_enter_blocking(&mx1);
    m_temperature = temperature;
    mutex_exit(&mx1);
}

void HWSensorchannel::SetHumidity(float humidity)
{
    mutex_enter_blocking(&mx2);
    m_humidity = humidity;
    mutex_exit(&mx2);
}

void HWSensorchannel::SetPressure(float pressure)
{
    mutex_enter_blocking(&mx3);
    m_pressure = pressure;
    mutex_exit(&mx3);
}

const std::string HWSensorchannel::name()
{
    return "HWSens";
}