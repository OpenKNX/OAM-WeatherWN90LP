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
    mutex_init(&mx4);
    mutex_init(&mx5);
    mutex_init(&mx6);
    mutex_init(&mx7);
    mutex_init(&mx8);
    mutex_init(&mx9);
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

    if(delayCheckMillis(m_light_last_poll_millis, 20000))
    {
        SetLight(ReadLight());
        m_light_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_uvi_last_poll_millis, 20000))
    {
        SetUVI(ReadUVI());
        m_uvi_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_wind_last_poll_millis, 20000))
    {
        SetWind(ReadWind());
        m_wind_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_gust_last_poll_millis, 20000))
    {
        SetGust(ReadGust());
        m_gust_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_winddir_last_poll_millis, 20000))
    {
        SetWindDir(ReadWindDir());
        m_winddir_last_poll_millis = millis();
        return true;
    }

    if(delayCheckMillis(m_rain_last_poll_millis, 20000))
    {
        SetRain(ReadRain());
        m_rain_last_poll_millis = millis();
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

float HWSensorchannel::ReadLight()
{
/* 165
Value in hex
Light=value*10
(Range: 0lux ->
300,000lux)
If invalid fill with
0xFFFF
*/
    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x0165, 0x0001);
    if (result != m_modbus.ku8MBSuccess)
        return NAN;
    

    data = m_modbus.getResponseBuffer(0);

    if(data == 0xFFFF)
        return NAN;

    float rawdata = data * 10.0;
    logDebugP("Light: %f Lux", rawdata);
    return rawdata;
}

uint8_t HWSensorchannel::ReadUVI()
{
/* 166
Value in hex Uvi=UVI
value/10
(Range: 0 -> 150)
If invalid fill with
0xFFFF
*/
    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x0166, 0x0001);
    if (result != m_modbus.ku8MBSuccess)
        return NAN;
    

    data = m_modbus.getResponseBuffer(0);

    if(data == 0xFFFF)
        return NAN;

    uint8_t rawdata = data;
    logDebugP("UV-Index: %d", rawdata);
    return rawdata;
}

float HWSensorchannel::ReadWind()
{
/* 169
Value in hex
If invalid fill with
0xFFFF.Wind Speed =
WIND
value*0.1m/s(0~40m/s)
*/
    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x0169, 0x0001);
    if (result != m_modbus.ku8MBSuccess)
        return NAN;
    

    data = m_modbus.getResponseBuffer(0);

    if(data == 0xFFFF)
        return NAN;

    float rawdata = data * 0.1;
    logDebugP("Wind: %f m/s", rawdata);
    return rawdata;
}

float HWSensorchannel::ReadGust()
{
/* 16A
Value in hex
If invalid fill with
0xFFFF.Gust Speed =
GUST
value*0.1m/s(0~40m/s)
*/
    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x016A, 0x0001);
    if (result != m_modbus.ku8MBSuccess)
        return NAN;
    

    data = m_modbus.getResponseBuffer(0);

    if(data == 0xFFFF)
        return NAN;

    float rawdata = data * 0.1;
    logDebugP("Wind Gust: %f m/s", rawdata);
    return rawdata;
}

uint8_t HWSensorchannel::ReadWindDir()
{
/* 16B
Value in hex
(Range: 0°- 359°)
If invalid fill with
0xFFFF
*/
    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x016B, 0x0001);
    if (result != m_modbus.ku8MBSuccess)
        return NAN;
    

    data = m_modbus.getResponseBuffer(0);

    if(data > 359)
        return NAN;

    uint8_t rawdata = data;
    logDebugP("Wind Dir: %d °", rawdata);
    return rawdata;
}

float HWSensorchannel::ReadRain()
{
/* 016E
data in hex
Rain = value*0.01mm
0.18mm=12H
*/
    int8_t result;
    uint16_t data = 0xffff;
    result = m_modbus.readHoldingRegisters(0x016E, 0x0001);
    if (result != m_modbus.ku8MBSuccess)
        return NAN;
    

    data = m_modbus.getResponseBuffer(0);

    if(data == 0xFFFF)
        return NAN;

    float rawdata = data * 0.01;
    logDebugP("Wind: %f m/s", rawdata);
    return rawdata;
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

float HWSensorchannel::GetLight()
{
    float retval;
    if(mutex_try_enter(&mx4, nullptr))
    {
        retval = m_light;
        mutex_exit(&mx4);
        return retval;
    }
    else
    {
        return NAN;
    }
}

uint8_t HWSensorchannel::GetUVI()
{
    uint8_t retval;
    if(mutex_try_enter(&mx5, nullptr))
    {
        retval = m_uvi;
        mutex_exit(&mx5);
        return retval;
    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::GetWind()
{
    float retval;
    if(mutex_try_enter(&mx6, nullptr))
    {
        retval = m_wind;
        mutex_exit(&mx6);
        return retval;
    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::GetGust()
{
    float retval;
    if(mutex_try_enter(&mx7, nullptr))
    {
        retval = m_gust;
        mutex_exit(&mx7);
        return retval;
    }
    else
    {
        return NAN;
    }
}

uint8_t HWSensorchannel::GetWindDir()
{
    uint8_t retval;
    if(mutex_try_enter(&mx8, nullptr))
    {
        retval = m_winddir;
        mutex_exit(&mx8);
        return retval;
    }
    else
    {
        return NAN;
    }
}

float HWSensorchannel::GetRain()
{
    float retval;
    if(mutex_try_enter(&mx9, nullptr))
    {
        retval = m_rain;
        mutex_exit(&mx9);
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

void HWSensorchannel::SetLight(float light)
{
    mutex_enter_blocking(&mx4);
    m_light = light;
    mutex_exit(&mx4);
}

void HWSensorchannel::SetUVI(uint8_t uvi)
{
    mutex_enter_blocking(&mx5);
    m_uvi = uvi;
    mutex_exit(&mx5);
}

void HWSensorchannel::SetWind(float wind)
{
    mutex_enter_blocking(&mx6);
    m_wind = wind;
    mutex_exit(&mx6);
}   

void HWSensorchannel::SetGust(float gust)
{
    mutex_enter_blocking(&mx7);
    m_gust = gust;
    mutex_exit(&mx7);
}

void HWSensorchannel::SetWindDir(uint8_t winddir)
{
    mutex_enter_blocking(&mx8);
    m_winddir = winddir;
    mutex_exit(&mx8);
}

void HWSensorchannel::SetRain(float rain)
{
    mutex_enter_blocking(&mx9);
    m_rain = rain;
    mutex_exit(&mx9);
}

const std::string HWSensorchannel::name()
{
    return "HWSens";
}