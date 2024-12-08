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

    if(delayCheckMillis(m_temperature_last_poll_millis, 20000))
    {
        SetTemperature(ReadTemperature());
        m_temperature_last_poll_millis = millis();
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
        //logDebugP("ReadTemperature: %d | %d | %d ms | %d ms", data, result, read-start, read3-read2);
        // convert and return
        float rawdata = (data - 400) / 10;
        logDebugP("Temp: %f", rawdata);
        return rawdata;

    }
    else
    {
        return NAN;
    }
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