#pragma once

#include "HWSensorchannel.h"
#include "knxprod.h"
#include "OpenKNX.h"



class HWSensors
{
    //members
    HWSensorchannel *m_HWSensorchannels[W90_ChannelCount];
    uint8_t m_Loop_i = 0;


    //constructors
    public: HWSensors();

	
    public: void Setup(const uint8_t addrs[]);
	
	public: void Loop();

    public: float GetTemperature(uint8_t channel);
    public: float GetHumidity(uint8_t channel);
    public: float GetPressure(uint8_t channel);
};