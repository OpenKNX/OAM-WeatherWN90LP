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
        mutex_t mx4;
        mutex_t mx5;
        mutex_t mx6;
        mutex_t mx7;
        mutex_t mx8;
        mutex_t mx9;


        ModbusMaster m_modbus;

        float m_temperature = NAN;
        uint32_t m_temperature_last_poll_millis = 10000;

        float m_humidity = NAN;
        uint32_t m_humidity_last_poll_millis = 10000;

        float m_pressure = NAN;
        uint32_t m_pressure_last_poll_millis = 12000;

        float m_light = NAN;
        uint32_t m_light_last_poll_millis = 14000;

        uint8_t m_uvi = NAN;
        uint32_t m_uvi_last_poll_millis = 14000;

        float m_wind = NAN;
        uint32_t m_wind_last_poll_millis = 16000;

        float m_gust = NAN;
        uint32_t m_gust_last_poll_millis = 16000;

        uint8_t m_winddir = NAN;
        uint32_t m_winddir_last_poll_millis = 16000;

        float m_rain = NAN;
        uint32_t m_rain_last_poll_millis = 18000;

        float ReadTemperature();
        float ReadHumidity();
        float ReadPressure();
        float ReadLight();
        uint8_t ReadUVI();
        float ReadWind();
        float ReadGust();
        uint8_t ReadWindDir();
        float ReadRain();


    public:
        HWSensorchannel();
        virtual void Setup(uint8_t adr, uint8_t channel_number);
	    virtual bool Loop();

        float GetTemperature();
        float GetHumidity();
        float GetPressure();
        float GetLight();
        uint8_t GetUVI();
        float GetWind();
        float GetGust();
        uint8_t GetWindDir();
        float GetRain();

        void SetTemperature(float temperature);
        void SetHumidity(float humidity);
        void SetPressure(float pressure);

        void SetLight(float light);
        void SetUVI(uint8_t uvi);
        void SetWind(float wind);
        void SetGust(float gust);
        void SetWindDir(uint8_t winddir);
        void SetRain(float rain);


        const std::string name() override;
};