#include "HWSensors.h"


HWSensors::HWSensors()
{
  
}

void HWSensors::Setup(const uint8_t addrs[])
{
    pinMode(RS485_UART_DIR_PIN, OUTPUT);
    digitalWrite(RS485_UART_DIR_PIN, 0);

    RS485_SERIAL.setRX(RS485_UART_RX_PIN);
    RS485_SERIAL.setTX(RS485_UART_TX_PIN);
    RS485_SERIAL.begin(9600, SERIAL_8N1);

    for(int i = 0;i<W90_ChannelCount;i++)
    {
        m_HWSensorchannels[i] = new HWSensorchannel();
        if(m_HWSensorchannels[i] != nullptr)
            m_HWSensorchannels[i]->Setup(addrs[i], i);
    }
}

void HWSensors::Loop()
{
    //query one sensor at a time
    bool nextsensor = true;
    if(m_HWSensorchannels[m_Loop_i] != nullptr)
    {
        nextsensor = m_HWSensorchannels[m_Loop_i]->Loop();
    }
    if(nextsensor)
    {
        m_Loop_i++;
        if(m_Loop_i >= W90_ChannelCount)
            m_Loop_i = 0;
    }
}

float HWSensors::GetTemperature(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetTemperature();
    }
    else
        return NAN;
}

float HWSensors::GetHumidity(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetHumidity();
    }
    else
        return NAN;
}

float HWSensors::GetPressure(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetPressure();
    }
    else
        return NAN;
}

float HWSensors::GetLight(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetLight();
    }
    else
        return NAN;
}

uint8_t HWSensors::GetUVI(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetUVI();
    }
    else
        return NAN;
}

float HWSensors::GetWind(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetWind();
    }
    else
        return NAN;
}

float HWSensors::GetGust(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetGust();
    }
    else
        return NAN;
}

uint8_t HWSensors::GetWindDir(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetWindDir();
    }
    else
        return NAN;
}

float HWSensors::GetRain(uint8_t channel)
{
    if(m_HWSensorchannels[channel] != nullptr)
    {
        return m_HWSensorchannels[channel]->GetRain();
    }
    else
        return NAN;
}
