#pragma once

#include "ModbusMaster.h"
#include "OpenKNX.h"
#include <Arduino.h>
#include <pico/sync.h>

class HWSensorchannel : public OpenKNX::Channel
{
  public:
    enum class SensorState
    {
        UNKNOWN,
        OK,
        ACTIVITY,
        ERROR
    };

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

    bool m_activity = false;

    float m_temperature = NAN;
    uint32_t m_temperature_last_poll_millis = 10000;
    uint32_t m_temperature_last_success_millis = 0;

    float m_humidity = NAN;
    uint32_t m_humidity_last_poll_millis = 10000;
    uint32_t m_humidity_last_success_millis = 0;

    float m_pressure = NAN;
    uint32_t m_pressure_last_poll_millis = 12000;
    uint32_t m_pressure_last_success_millis = 0;

    float m_light = NAN;
    uint32_t m_light_last_poll_millis = 14000;
    uint32_t m_light_last_success_millis = 0;

    uint8_t m_uvi = 0xff;
    uint32_t m_uvi_last_poll_millis = 14000;
    uint32_t m_uvi_last_success_millis = 0;

    float m_wind = NAN;
    uint32_t m_wind_last_poll_millis = 16000;
    uint32_t m_wind_last_success_millis = 0;

    float m_gust = NAN;
    uint32_t m_gust_last_poll_millis = 16000;
    uint32_t m_gust_last_success_millis = 0;

    uint8_t m_winddir = 0xff;
    uint32_t m_winddir_last_poll_millis = 16000;
    uint32_t m_winddir_last_success_millis = 0;

    int32_t m_rain = -1;
    uint32_t m_rain_last_poll_millis = 18000;
    uint32_t m_rain_last_success_millis = 0;

    float ReadTemperature();
    float ReadHumidity();
    float ReadPressure();
    float ReadLight();
    uint8_t ReadUVI();
    float ReadWind();
    float ReadGust();
    uint8_t ReadWindDir();
    int32_t ReadRain();

  public:
    HWSensorchannel();
    virtual void Setup(uint8_t adr, uint8_t channel_number, Stream &serial);
    virtual bool Loop();

    float GetTemperature();
    float GetHumidity();
    float GetPressure();
    float GetLight();
    uint8_t GetUVI();
    float GetWind();
    float GetGust();
    uint8_t GetWindDir();
    int32_t GetRain();

    void SetTemperature(float temperature);
    void SetHumidity(float humidity);
    void SetPressure(float pressure);

    void SetLight(float light);
    void SetUVI(uint8_t uvi);
    void SetWind(float wind);
    void SetGust(float gust);
    void SetWindDir(uint8_t winddir);
    void SetRain(int32_t rain);
    SensorState GetState();

    const std::string name() override;
};