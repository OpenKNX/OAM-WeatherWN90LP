#pragma once

#include "HWSensors.h"
#include "OpenKNX.h"
#include <Arduino.h>

#define OPENKNX_LEDFUNC_W90_A 300

// converts a relative (to the channel start) KO number to an absolute KO number of the device
#define AbsKO(asap) (W90_KoOffset + W90_KoBlockSize * _channelIndex + asap)

// converts a absolute KO number to a relative KO number (offset to the starting KO number of the channel)
#define RelKO(asap) (asap - W90_KoOffset - W90_KoBlockSize * _channelIndex)

#define TempKODPT (Dpt(9, 1))
#define HumKODPT (W90_SensorHumiditySend_ == 5 ? Dpt(5, 1) : Dpt(9, 7))
#define AbsHumKODPT Dpt(9, 29)
#define DewPointKODPT Dpt(9, 1)
#define PressKODPT Dpt(9, 6)
#define LightKODPT Dpt(9, 4)
#define UviKODPT Dpt(5, 5)
#define WindKODPT Dpt(9, 5)
#define GustKODPT Dpt(9, 5)
#define WindDirKODPT Dpt(5, 3)
#define RainKODPT Dpt(9, 26)
#define RainFlowKODPT Dpt(9, 25)
#define RainBoolKODPT Dpt(1, 2)

class Sensorchannel : public OpenKNX::Channel
{
  private:
    HWSensors *m_hwSensors;

    uint32_t m_temperature_last_send_millis = 0;
    uint32_t m_temperature_last_poll_millis = 0;
    float m_temperature_last_send_value = -1000;
    uint32_t m_temperature_alarmH_last_send_millis = 0;
    uint32_t m_temperature_alarmL_last_send_millis = 0;

    uint32_t m_humidity_last_send_millis = 0;
    float m_humidity_last_send_value = -1000;
    uint32_t m_humidity_alarmH_last_send_millis = 0;
    uint32_t m_humidity_alarmL_last_send_millis = 0;

    uint32_t m_abshumidity_last_send_millis = 0;
    float m_abshumidity_last_send_value = -1000;
    uint32_t m_abshumidity_alarmH_last_send_millis = 0;
    uint32_t m_abshumidity_alarmL_last_send_millis = 0;

    uint32_t m_dewpoint_last_send_millis = 0;
    float m_dewpoint_last_send_value = -1000;
    uint32_t m_dewpoint_alarmH_last_send_millis = 0;
    uint32_t m_dewpoint_alarmL_last_send_millis = 0;

    uint32_t m_pressure_last_send_millis = 0;
    float m_pressure_last_send_value = -1000;
    uint32_t m_pressure_alarmH_last_send_millis = 0;
    uint32_t m_pressure_alarmL_last_send_millis = 0;

    uint32_t m_light_last_send_millis = 0;
    float m_light_last_send_value = -1000;
    uint32_t m_light_alarmH_last_send_millis = 0;
    uint32_t m_light_alarmL_last_send_millis = 0;

    uint32_t m_uvi_last_send_millis = 0;
    uint8_t m_uvi_last_send_value = 0;
    uint32_t m_uvi_alarmH_last_send_millis = 0;
    uint32_t m_uvi_alarmL_last_send_millis = 0;

    uint32_t m_wind_last_send_millis = 0;
    float m_wind_last_send_value = -1000;
    uint32_t m_wind_alarmH_last_send_millis = 0;
    uint32_t m_wind_alarmL_last_send_millis = 0;

    uint32_t m_gust_last_send_millis = 0;
    float m_gust_last_send_value = -1000;
    uint32_t m_gust_alarmH_last_send_millis = 0;
    uint32_t m_gust_alarmL_last_send_millis = 0;

    uint32_t m_winddir_last_send_millis = 0;
    uint8_t m_winddir_last_send_value = 0;
    uint32_t m_winddir_alarmH_last_send_millis = 0;
    uint32_t m_winddir_alarmL_last_send_millis = 0;

    uint32_t m_rain_last_send_millis = 0;
    float m_rain_last_send_value = -1000;
    float m_rain_last_recv_value = -1000;
    float m_rain_last_recv_value_restored = -1000;
    uint32_t m_rain_last_recv_millis = 0;
    uint32_t m_rain_alarmH_last_send_millis = 0;
    uint32_t m_rain_alarmL_last_send_millis = 0;
    float m_rain_offset_value = 0;

    float m_rainflow_lastvalue = -1000;
    uint32_t m_rainflow_lastvalue_millis = 0;

    uint32_t m_rainbool_last_send_millis = 0;
    uint32_t m_state_last_send_millis = 0;

    uint32_t _lastActivity = 0;
    uint8_t _ledState = 0;

    float CalcDewPoint(float relative_humidity, float temperature);
    float CalcAbsHumidity(float relative_humidity, float temperature);
    float pressureToSeaLevel(float pressure, int16_t sensor_height);

    const std::string name() override;

    void loop_temperature(float temperature);
    void loop_humidity(float humidity);
    void loop_abshumidity(float abshumidity);
    void loop_dewpoint(float dewpoint);
    void loop_pressure(float pressure);
    void loop_light(float light);
    void loop_uvi(uint8_t uvi);
    void loop_wind(float wind);
    void loop_gust(float gust);
    void loop_winddir(uint8_t winddir);
    void loop_rain(float rain);

  public:
    Sensorchannel();
    void Setup(uint8_t channel_number, HWSensors *HWSensors);
    void loop();
    void processInputKo(GroupObject &ko);
    void save();
    void restore();
};