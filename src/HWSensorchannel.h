#pragma once

#include <Arduino.h>
#include <pico/sync.h>
#include "OpenKNX.h"
#include "ModbusMaster.h"


class HWSensorchannel: public OpenKNX::Channel
{
    protected:
        uint8_t m_adr = 0;
        mutex_t mx1;
        mutex_t mx2;
        mutex_t mx3;

        ModbusMaster m_modbus;

        float m_temperature = NAN;
        uint32_t m_temperature_last_poll_millis = 0;
        float m_humidity = NAN;
        float m_pressure = NAN;

        float ReadTemperature();

    public:
        HWSensorchannel();
        virtual void Setup(uint8_t adr, uint8_t channel_number);
	    virtual bool Loop();

        float GetTemperature();
        float GetHumidity();
        float GetPressure();

        void SetTemperature(float temperature);
        void SetHumidity(float humidity);
        void SetPressure(float pressure);

        const std::string name() override;
};