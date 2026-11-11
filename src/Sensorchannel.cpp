#include "Sensorchannel.h"
#include "OpenKNX/Led/FunctionManager.h"
#include <knx.h>

Sensorchannel::Sensorchannel()
{
}

void Sensorchannel::Setup(uint8_t channel_number, HWSensors *HWSensors)
{
    _channelIndex = channel_number;
    m_hwSensors = HWSensors;

    // Initialize Min/Max Ko
    // if save data are in flash, this will be overwritten
    KoW90_SensorTempMinValue_.valueNoSend((float)1000, TempKODPT);
    KoW90_SensorTempMaxValue_.valueNoSend((float)-1000, TempKODPT);
    // humidity KO may be DPT 5.001, which only accepts 0..100
    KoW90_SensorHumMinValue_.valueNoSend((float)100, HumKODPT);
    KoW90_SensorHumMaxValue_.valueNoSend((float)0, HumKODPT);
    KoW90_SensorAbsHumMinValue_.valueNoSend((float)1000, AbsHumKODPT);
    KoW90_SensorAbsHumMaxValue_.valueNoSend((float)-1000, AbsHumKODPT);
    KoW90_SensorDewPointMinValue_.valueNoSend((float)1000, DewPointKODPT);
    KoW90_SensorDewPointMaxValue_.valueNoSend((float)-1000, DewPointKODPT);
    KoW90_SensorPressMinValue_.valueNoSend((float)500000, PressKODPT);
    KoW90_SensorPressMaxValue_.valueNoSend((float)-500000, PressKODPT);
    KoW90_SensorLightMinValue_.valueNoSend((float)500000, LightKODPT);
    KoW90_SensorLightMaxValue_.valueNoSend((float)-500000, LightKODPT);
    KoW90_SensorUVIMinValue_.valueNoSend((uint8_t)255, UviKODPT);
    KoW90_SensorUVIMaxValue_.valueNoSend((uint8_t)0, UviKODPT);
    KoW90_SensorWindMinValue_.valueNoSend((float)500000, WindKODPT);
    KoW90_SensorWindMaxValue_.valueNoSend((float)-500000, WindKODPT);
    KoW90_SensorGustMinValue_.valueNoSend((float)500000, GustKODPT);
    KoW90_SensorGustMaxValue_.valueNoSend((float)-500000, GustKODPT);
}

void Sensorchannel::loop()
{
    float temperature = m_hwSensors->GetTemperature(_channelIndex);
    float humidity = m_hwSensors->GetHumidity(_channelIndex);
    float abshumidity = CalcAbsHumidity(humidity, temperature);
    float dewpoint = CalcDewPoint(humidity, temperature);
    float pressure = m_hwSensors->GetPressure(_channelIndex);
    float light = m_hwSensors->GetLight(_channelIndex);
    uint8_t uvi = m_hwSensors->GetUVI(_channelIndex);
    float wind = m_hwSensors->GetWind(_channelIndex);
    float gust = m_hwSensors->GetGust(_channelIndex);
    uint16_t winddir = m_hwSensors->GetWindDir(_channelIndex);
    int32_t rain = m_hwSensors->GetRain(_channelIndex);

    loop_temperature(temperature);
    loop_humidity(humidity);
    loop_abshumidity(abshumidity);
    loop_dewpoint(dewpoint);
    loop_pressure(pressure);
    loop_light(light);
    loop_uvi(uvi);
    loop_wind(wind);
    loop_gust(gust);
    loop_winddir(winddir);
    loop_rain(rain);

    OpenKNX::Led::FunctionGroup *led = openknx.ledFunctions.get(OPENKNX_LEDFUNC_W90_A + _channelIndex);
    HWSensorchannel::SensorState state = m_hwSensors->GetState(_channelIndex);
    bool stateb = (state == HWSensorchannel::SensorState::ACTIVITY || state == HWSensorchannel::SensorState::OK);
    KoW90_SensorState_.valueCompareTime(stateb, Dpt(1,2),m_state_last_send_millis,5*60*1000);


    if (led->active())
    {
        switch (state)
        {
            case HWSensorchannel::SensorState::ACTIVITY:
                _lastActivity = millis();
            case HWSensorchannel::SensorState::OK:
                if(_ledState != 1)
                {
                    led->color(OpenKNX::Led::Color::Green);
                    led->activity(_lastActivity, true);
                    _ledState = 1;
                }
                break;
            case HWSensorchannel::SensorState::ERROR:
                if(_ledState != 2)
                {
                    led->color(OpenKNX::Led::Color::Red);
                    led->on(OpenKNX::Led::Capability::COLOR);
                    led->off(OpenKNX::Led::Capability::MONOCHROME);
                    _ledState = 2;
                }
                break;
            case HWSensorchannel::SensorState::UNKNOWN:
                if(_ledState != 3)
                {
                    led->color(OpenKNX::Led::Color::Yellow);
                    led->on(OpenKNX::Led::Capability::COLOR);
                    led->off(OpenKNX::Led::Capability::MONOCHROME);
                    _ledState = 3;
                }
                break;
            default:
                led->off();
                _ledState = 0;
                break;
        }
    }

}

// Sends the value cyclic (sendMillis > 0) and/or on change (sendOnChange), otherwise only updates the KO.
// changeAmount <= 0 means: send on every change.
void Sensorchannel::processSend(GroupObject &ko, float value, const Dpt &dpt, uint32_t sendMillis, bool sendOnChange, float changeAmount, uint32_t &lastSendMillis, float &lastSendValue)
{
    bool sendnow = false;
    if (sendMillis)
    {
        sendnow = lastSendMillis == 0 || millis() - lastSendMillis > sendMillis;
    }
    if (!sendnow && sendOnChange)
    {
        if (isnan(lastSendValue))
            sendnow = true;
        else if (changeAmount > 0)
            sendnow = fabs(value - lastSendValue) >= changeAmount;
        else
            sendnow = value != lastSendValue;
    }

    if (sendnow)
    {
        ko.value(value, dpt);
        lastSendMillis = millis();
        lastSendValue = value;
    }
    else
    {
        ko.valueNoSend(value, dpt);
    }
}

// value has to be the calibrated value
void Sensorchannel::processMinMax(GroupObject &koMin, GroupObject &koMax, float value, const Dpt &dpt)
{
    if (value > (float)koMax.value(dpt))
        koMax.valueNoSend(value, dpt);
    if (value < (float)koMin.value(dpt))
        koMin.valueNoSend(value, dpt);
}

// Sends the alarm on change. An active alarm is repeated with sendMillis (only if sendMillis > 0).
void Sensorchannel::processAlarm(GroupObject &koAlarmL, GroupObject &koAlarmH, float value, float warnL, float warnH, uint32_t sendMillis, uint32_t &lastSendL, uint32_t &lastSendH)
{
    bool alarmH = value > warnH;
    if ((bool)koAlarmH.value(AlarmKODPT) != alarmH ||                              // alarm value has changed
        (alarmH && sendMillis > 0 && millis() - lastSendH > sendMillis)) // alarm is true and has not been sent for sendMillis
    {
        koAlarmH.value(alarmH, AlarmKODPT);
        lastSendH = millis();
    }

    bool alarmL = value < warnL;
    if ((bool)koAlarmL.value(AlarmKODPT) != alarmL ||                              // alarm value has changed
        (alarmL && sendMillis > 0 && millis() - lastSendL > sendMillis)) // alarm is true and has not been sent for sendMillis
    {
        koAlarmL.value(alarmL, AlarmKODPT);
        lastSendL = millis();
    }
}

void Sensorchannel::resetMinMax(GroupObject &koValue, GroupObject &koMin, GroupObject &koMax, const Dpt &dpt)
{
    float value = koValue.value(dpt);
    koMin.valueNoSend(value, dpt);
    koMax.valueNoSend(value, dpt);
}

void Sensorchannel::loop_temperature(float temperature)
{
    if (isnan(temperature))
        return;

    float value = temperature + ParamW90_SensorTemperatureAlign_;
    uint32_t send_millis = ParamW90_SensorTemperatureSendCycle_ * 60000;

    processSend(KoW90_SensorTemp_, value, TempKODPT, send_millis, ParamW90_SensorTemperatureSendChange_, ParamW90_SensorTemperatureSendChangeAmount_, m_temperature_last_send_millis, m_temperature_last_send_value);

    if (ParamW90_SensorTemperatureMinMax_)
        processMinMax(KoW90_SensorTempMinValue_, KoW90_SensorTempMaxValue_, value, TempKODPT);

    if (ParamW90_SensorTemperatureWarn_)
        processAlarm(KoW90_SensorTempAlarmL_, KoW90_SensorTempAlarmH_, value, ParamW90_SensorTemperatureWarnL_, ParamW90_SensorTemperatureWarnH_, send_millis, m_temperature_alarmL_last_send_millis, m_temperature_alarmH_last_send_millis);
}

void Sensorchannel::loop_humidity(float humidity)
{
    if (isnan(humidity))
        return;

    float value = constrain(humidity + ParamW90_SensorHumidityAlign_, 0.0f, 100.0f);
    uint32_t send_millis = ParamW90_SensorHumiditySendCycle_ * 60000;

    processSend(KoW90_SensorHum_, value, HumKODPT, send_millis, ParamW90_SensorHumiditySendChange_, ParamW90_SensorHumiditySendChangeAmount_, m_humidity_last_send_millis, m_humidity_last_send_value);

    if (ParamW90_SensorHumidityMinMax_)
        processMinMax(KoW90_SensorHumMinValue_, KoW90_SensorHumMaxValue_, value, HumKODPT);

    if (ParamW90_SensorHumidityWarn_)
        processAlarm(KoW90_SensorHumAlarmL_, KoW90_SensorHumAlarmH_, value, ParamW90_SensorHumidityWarnL_, ParamW90_SensorHumidityWarnH_, send_millis, m_humidity_alarmL_last_send_millis, m_humidity_alarmH_last_send_millis);
}

void Sensorchannel::loop_abshumidity(float abshumidity)
{
    if (isnan(abshumidity))
        return;

    float value = abshumidity + ParamW90_SensorAbsHumidityAlign_;
    uint32_t send_millis = ParamW90_SensorAbsHumiditySendCycle_ * 60000;

    processSend(KoW90_SensorAbsHum_, value, AbsHumKODPT, send_millis, ParamW90_SensorAbsHumiditySendChange_, ParamW90_SensorAbsHumiditySendChangeAmount_, m_abshumidity_last_send_millis, m_abshumidity_last_send_value);

    if (ParamW90_SensorAbsHumidityMinMax_)
        processMinMax(KoW90_SensorAbsHumMinValue_, KoW90_SensorAbsHumMaxValue_, value, AbsHumKODPT);

    if (ParamW90_SensorAbsHumidityWarn_)
        processAlarm(KoW90_SensorAbsHumAlarmL_, KoW90_SensorAbsHumAlarmH_, value, ParamW90_SensorAbsHumidityWarnL_, ParamW90_SensorAbsHumidityWarnH_, send_millis, m_abshumidity_alarmL_last_send_millis, m_abshumidity_alarmH_last_send_millis);
}

void Sensorchannel::loop_dewpoint(float dewpoint)
{
    if (isnan(dewpoint))
        return;

    float value = dewpoint + ParamW90_SensorDewPointAlign_;
    uint32_t send_millis = ParamW90_SensorDewPointSendCycle_ * 60000;

    processSend(KoW90_SensorDewPoint_, value, DewPointKODPT, send_millis, ParamW90_SensorDewPointSendChange_, ParamW90_SensorDewPointSendChangeAmount_, m_dewpoint_last_send_millis, m_dewpoint_last_send_value);

    if (ParamW90_SensorDewPointMinMax_)
        processMinMax(KoW90_SensorDewPointMinValue_, KoW90_SensorDewPointMaxValue_, value, DewPointKODPT);

    if (ParamW90_SensorDewPointWarn_)
        processAlarm(KoW90_SensorDewPointAlarmL_, KoW90_SensorDewPointAlarmH_, value, ParamW90_SensorDewPointWarnL_, ParamW90_SensorDewPointWarnH_, send_millis, m_dewpoint_alarmL_last_send_millis, m_dewpoint_alarmH_last_send_millis);
}

void Sensorchannel::loop_pressure(float pressure)
{
    if (isnan(pressure))
        return;

    if (ParamW90_SensorPressureHeightASL_ != 0) // under / above sea level height correction enabled
    {
        pressure = pressureToSeaLevel(pressure, ParamW90_SensorPressureHeightASL_);
    }

    // DPT 9.004/9.005/9.006 do not accept negative values
    float value = max(0.0f, pressure + ParamW90_SensorPressureAlign_);
    uint32_t send_millis = ParamW90_SensorPressureSendCycle_ * 60000;

    processSend(KoW90_SensorPress_, value, PressKODPT, send_millis, ParamW90_SensorPressureSendChange_, ParamW90_SensorPressureSendChangeAmount_, m_pressure_last_send_millis, m_pressure_last_send_value);

    if (ParamW90_SensorPressureMinMax_)
        processMinMax(KoW90_SensorPressMinValue_, KoW90_SensorPressMaxValue_, value, PressKODPT);

    if (ParamW90_SensorPressureWarn_)
        processAlarm(KoW90_SensorPressAlarmL_, KoW90_SensorPressAlarmH_, value, ParamW90_SensorPressureWarnL_, ParamW90_SensorPressureWarnH_, send_millis, m_pressure_alarmL_last_send_millis, m_pressure_alarmH_last_send_millis);
}

void Sensorchannel::loop_light(float light)
{
    if (isnan(light))
        return;

    float value = max(0.0f, light + ParamW90_SensorLightAlign_);
    uint32_t send_millis = ParamW90_SensorLightSendCycle_ * 60000;

    processSend(KoW90_SensorLight_, value, LightKODPT, send_millis, ParamW90_SensorLightSendChange_, ParamW90_SensorLightSendChangeAmount_, m_light_last_send_millis, m_light_last_send_value);

    if (ParamW90_SensorLightMinMax_)
        processMinMax(KoW90_SensorLightMinValue_, KoW90_SensorLightMaxValue_, value, LightKODPT);

    if (ParamW90_SensorLightWarn_)
        processAlarm(KoW90_SensorLightAlarmL_, KoW90_SensorLightAlarmH_, value, ParamW90_SensorLightWarnL_, ParamW90_SensorLightWarnH_, send_millis, m_light_alarmL_last_send_millis, m_light_alarmH_last_send_millis);
}

void Sensorchannel::loop_uvi(uint8_t uvi)
{
    if (uvi == 0xff)
        return;

    float value = min(255.0f, (float)uvi + ParamW90_SensorUVIAlign_); // DPT 5.005: 0..255
    uint32_t send_millis = ParamW90_SensorUVISendCycle_ * 60000;

    processSend(KoW90_SensorUVI_, value, UviKODPT, send_millis, ParamW90_SensorUVISendChange_, ParamW90_SensorUVISendChangeAmount_, m_uvi_last_send_millis, m_uvi_last_send_value);

    if (ParamW90_SensorUVIMinMax_)
        processMinMax(KoW90_SensorUVIMinValue_, KoW90_SensorUVIMaxValue_, value, UviKODPT);

    if (ParamW90_SensorUVIWarn_)
        processAlarm(KoW90_SensorUVIAlarmL_, KoW90_SensorUVIAlarmH_, value, ParamW90_SensorUVIWarnL_, ParamW90_SensorUVIWarnH_, send_millis, m_uvi_alarmL_last_send_millis, m_uvi_alarmH_last_send_millis);
}

void Sensorchannel::loop_wind(float wind)
{
    if (isnan(wind))
        return;

    float value = max(0.0f, wind + ParamW90_SensorWindAlign_);
    uint32_t send_millis = ParamW90_SensorWindSendCycle_ * 60000;

    processSend(KoW90_SensorWind_, value, WindKODPT, send_millis, ParamW90_SensorWindSendChange_, ParamW90_SensorWindSendChangeAmount_, m_wind_last_send_millis, m_wind_last_send_value);

    if (ParamW90_SensorWindMinMax_)
        processMinMax(KoW90_SensorWindMinValue_, KoW90_SensorWindMaxValue_, value, WindKODPT);

    if (ParamW90_SensorWindWarn_)
        processAlarm(KoW90_SensorWindAlarmL_, KoW90_SensorWindAlarmH_, value, ParamW90_SensorWindWarnL_, ParamW90_SensorWindWarnH_, send_millis, m_wind_alarmL_last_send_millis, m_wind_alarmH_last_send_millis);
}

void Sensorchannel::loop_gust(float gust)
{
    if (isnan(gust))
        return;

    float value = max(0.0f, gust + ParamW90_SensorGustAlign_);
    uint32_t send_millis = ParamW90_SensorGustSendCycle_ * 60000;

    processSend(KoW90_SensorGust_, value, GustKODPT, send_millis, ParamW90_SensorGustSendChange_, ParamW90_SensorGustSendChangeAmount_, m_gust_last_send_millis, m_gust_last_send_value);

    if (ParamW90_SensorGustMinMax_)
        processMinMax(KoW90_SensorGustMinValue_, KoW90_SensorGustMaxValue_, value, GustKODPT);

    if (ParamW90_SensorGustWarn_)
        processAlarm(KoW90_SensorGustAlarmL_, KoW90_SensorGustAlarmH_, value, ParamW90_SensorGustWarnL_, ParamW90_SensorGustWarnH_, send_millis, m_gust_alarmL_last_send_millis, m_gust_alarmH_last_send_millis);
}

void Sensorchannel::loop_winddir(uint16_t winddir)
{
    if (winddir == W90_WINDDIR_INVALID)
        return;

    // 0..359 degrees, calibration rotates the direction
    uint16_t value = (winddir + ParamW90_SensorWindDirAlign_) % 360;
    uint32_t send_millis = ParamW90_SensorWindDirSendCycle_ * 60000;

    bool sendnow = false;
    if (send_millis)
    {
        sendnow = m_winddir_last_send_millis == 0 || millis() - m_winddir_last_send_millis > send_millis;
    }
    if (!sendnow && ParamW90_SensorWindDirSendChange_)
    {
        if (m_winddir_last_send_value == W90_WINDDIR_INVALID)
        {
            sendnow = true;
        }
        else
        {
            // angular difference, e.g. 359° -> 1° is 2°
            uint16_t diff = abs((int16_t)value - (int16_t)m_winddir_last_send_value);
            if (diff > 180)
                diff = 360 - diff;
            uint8_t amount = ParamW90_SensorWindDirSendChangeAmount_;
            sendnow = amount ? diff >= amount : diff != 0;
        }
    }

    // DPT 5.003 takes the angle in degrees (0..360) and scales it to 0..255
    if (sendnow)
    {
        KoW90_SensorWindDir_.value((float)value, WindDirKODPT);
        logDebugP("Send winddirKO: %d", value);
        m_winddir_last_send_millis = millis();
        m_winddir_last_send_value = value;
    }
    else
    {
        KoW90_SensorWindDir_.valueNoSend((float)value, WindDirKODPT);
    }
}

void Sensorchannel::loop_rain(int32_t rain)
{
    if (rain < 0)
        return;

    const bool firstValue = m_rain_last_recv_value < 0;
    const bool changed = !firstValue && m_rain_last_recv_value != rain;

    // detect counter overflow and resets of the station - otherwise the counter of the station never decreases
    if (firstValue)
    {
        // first value after start of the knx device: if the counter is lower than the last known value,
        // the station has been powered off as well (e.g. power outage) and restarted counting from 0
        if (m_rain_last_recv_value_restored > 0 && rain < m_rain_last_recv_value_restored)
        {
            m_rain_offset_value += m_rain_last_recv_value_restored;
            logDebugP("rain gauge of station was reset during power outage, offset += %d", m_rain_last_recv_value_restored);
        }
    }
    else if (rain < m_rain_last_recv_value)
    {
        if (m_rain_last_recv_value > 65400 && rain < 100)
        {
            // value overflow detected. Value Range 0xffff
            m_rain_offset_value += 0x10000;
            logDebugP("rain value overflow, offset += 0x10000");
        }
        else
        {
            // rain gauge of station was reset (powercycle of station only)
            m_rain_offset_value += m_rain_last_recv_value;
            logDebugP("rain gauge of station was reset, offset += %d", m_rain_last_recv_value);
        }
    }

    // send the raw value (on change only)
    KoW90_SensorRainGaugeRaw_.valueCompare(rain * 0.01, RainKODPT);
    KoW90_SensorRainGaugeRaw2_.valueCompare(rain, Dpt(12,1));

    // continuous rain gauge in 0.01 mm, independent of overflow and resets of the station
    const int32_t total = rain + m_rain_offset_value;
    // timeout for rain detection, 0 = rain detection disabled
    const uint32_t rainTimeout = ParamW90_SensorRainTime_ * 60000;

    // rain flow (l/m² per hour = mm/h)
    if (m_rainflow_lastvalue < 0)
    {
        m_rainflow_lastvalue = total;
        m_rainflow_lastvalue_millis = millis();
    }
    else
    {
        uint32_t delta = millis() - m_rainflow_lastvalue_millis;
        if ((total != m_rainflow_lastvalue && delta > 30000) || // new rain, calculate at most every 30s
            (rainTimeout && delta >= rainTimeout))              // no (more) rain within the timeout
        {
            float rainflow = (total - m_rainflow_lastvalue) * 0.01f * 3600000.0f / delta;
            KoW90_SensorRainFlow_.value(rainflow, RainFlowKODPT);
            logDebugP("rainflow: raindelta %d, delta %d ms, rainflow %f", total - m_rainflow_lastvalue, delta, rainflow);
            m_rainflow_lastvalue = total;
            m_rainflow_lastvalue_millis = millis();
        }
    }

    // rain detection (bool)
    if (changed || firstValue)
    {
        m_rain_last_recv_millis = millis();
    }
    if (rainTimeout)
    {
        if (changed)
        {
            KoW90_SensorRain_.valueCompareTime(true, RainBoolKODPT, m_rainbool_last_send_millis, 5 * 60 * 1000);
        }
        else if (delayCheckMillis(m_rain_last_recv_millis, rainTimeout)) // no change of rain gauge within the timeout
        {
            KoW90_SensorRain_.valueCompareTime(false, RainBoolKODPT, m_rainbool_last_send_millis, 5 * 60 * 1000); // signal inactive rain
        }
    }
    m_rain_last_recv_value = rain;

    // rain gauge
    uint32_t send_millis = ParamW90_SensorRainSendCycle_ * 60000;
    bool sendnow = false;
    if (send_millis)
    {
        sendnow = m_rain_last_send_millis == 0 || millis() - m_rain_last_send_millis > send_millis;
    }
    if (!sendnow)
    {
        sendnow = total != m_rain_last_send_value;
    }

    double rain_f = total * 0.01;
    if (sendnow)
    {
        KoW90_SensorRainGauge_.value(rain_f, RainKODPT);
        KoW90_SensorRainGauge4_.value(rain_f, Dpt(14,0));
        logDebugP("Send RainKO: %f", rain_f);
        m_rain_last_send_millis = millis();
        m_rain_last_send_value = total;
    }
    else
    {
        KoW90_SensorRainGauge_.valueNoSend(rain_f, RainKODPT);
        KoW90_SensorRainGauge4_.valueNoSend(rain_f, Dpt(14,0));
    }
}

void Sensorchannel::setRain(double setvalue)
{
    if (isnan(setvalue) || setvalue < 0)
        return;

    // current continuous rain gauge in 0.01 mm
    int32_t current;
    if (m_rain_last_recv_value >= 0)
        current = m_rain_last_recv_value + m_rain_offset_value;
    else if (m_rain_last_recv_value_restored >= 0) // no value received since start, use the value from flash
        current = m_rain_last_recv_value_restored + m_rain_offset_value;
    else
        current = round((double)KoW90_SensorRainGauge4_.value(Dpt(14,0)) * 100);

    int32_t delta = (int32_t)round(setvalue * 100) - current;
    logDebugP("set RainGauge to %f, old offset %d, new offset %d", setvalue, m_rain_offset_value, m_rain_offset_value + delta);
    m_rain_offset_value += delta;

    // keep the rain flow calculation consistent
    if (m_rainflow_lastvalue >= 0)
        m_rainflow_lastvalue += delta;

    // force sending of the new value
    m_rain_last_send_value = INT32_MIN;
    loop_rain(m_hwSensors->GetRain(_channelIndex));
}

void Sensorchannel::processInputKo(GroupObject &ko)
{
    logTraceP("processInputKo");

    switch (RelKO(ko.asap()))
    {
        case W90_KoSensorTempMinMaxReset_:
            resetMinMax(KoW90_SensorTemp_, KoW90_SensorTempMinValue_, KoW90_SensorTempMaxValue_, TempKODPT);
            break;
        case W90_KoSensorHumMinMaxReset_:
            resetMinMax(KoW90_SensorHum_, KoW90_SensorHumMinValue_, KoW90_SensorHumMaxValue_, HumKODPT);
            break;
        case W90_KoSensorAbsHumMinMaxReset_:
            resetMinMax(KoW90_SensorAbsHum_, KoW90_SensorAbsHumMinValue_, KoW90_SensorAbsHumMaxValue_, AbsHumKODPT);
            break;
        case W90_KoSensorDewPointMinMaxReset_:
            resetMinMax(KoW90_SensorDewPoint_, KoW90_SensorDewPointMinValue_, KoW90_SensorDewPointMaxValue_, DewPointKODPT);
            break;
        case W90_KoSensorPressMinMaxReset_:
            resetMinMax(KoW90_SensorPress_, KoW90_SensorPressMinValue_, KoW90_SensorPressMaxValue_, PressKODPT);
            break;
        case W90_KoSensorLightMinMaxReset_:
            resetMinMax(KoW90_SensorLight_, KoW90_SensorLightMinValue_, KoW90_SensorLightMaxValue_, LightKODPT);
            break;
        case W90_KoSensorUVIMinMaxReset_:
            resetMinMax(KoW90_SensorUVI_, KoW90_SensorUVIMinValue_, KoW90_SensorUVIMaxValue_, UviKODPT);
            break;
        case W90_KoSensorWindMinMaxReset_:
            resetMinMax(KoW90_SensorWind_, KoW90_SensorWindMinValue_, KoW90_SensorWindMaxValue_, WindKODPT);
            break;
        case W90_KoSensorGustMinMaxReset_:
            resetMinMax(KoW90_SensorGust_, KoW90_SensorGustMinValue_, KoW90_SensorGustMaxValue_, GustKODPT);
            break;
        case W90_KoSensorSetRainGauge4_:
            setRain(ko.value(Dpt(14,0)));
            break;
        case W90_KoSensorResetRainGauge_:
            if(ko.value(DPT_Enable))
                setRain(0);
            break;
    }
}

float Sensorchannel::CalcDewPoint(float relative_humidity, float temperature)
{
    float a, b;
    if (temperature >= 0)
    {
        a = 7.5;
        b = 237.3;
    }
    else
    {
        a = 7.6;
        b = 240.7;
    }

    // Magnusformel
    double sdd = 6.1078 * pow(10.0, ((a * temperature) / (b + temperature))); // Sättigungsdampfdruck
    double dd = (relative_humidity / 100.0) * sdd;                            // Dampfdruck
    double v = log10((dd / 6.1078) / log10(10));

    double value = (b * v) / (a - v); // Taupunkt
    return (float)value;
}

float Sensorchannel::CalcAbsHumidity(float relative_humidity, float temperature)
{
    // input in 1°C , 0-100% humidity
    // ouput in g/m³

    float value = (6.112 * pow(2.718, (17.67 * temperature) / (temperature + 243.5)) * relative_humidity * 2.1674) / (273.15 + temperature);
    return value;
}

float Sensorchannel::pressureToSeaLevel(float pressure, int16_t sensor_height)
{
    const float k = 2.25577e-5f;

    float x = k * sensor_height;

    // Approximation von (1 - x)^-5.255
    float factor = 1.0f
                 + 5.255f * x
                 + 16.42f * x * x;

    return pressure * factor;
}

const std::string Sensorchannel::name()
{
    return "Sensor";
}

void Sensorchannel::save()
{
    openknx.flash.writeFloat((float)KoW90_SensorTempMaxValue_.value(TempKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorTempMinValue_.value(TempKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorHumMaxValue_.value(HumKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorHumMinValue_.value(HumKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorAbsHumMaxValue_.value(AbsHumKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorAbsHumMinValue_.value(AbsHumKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorDewPointMaxValue_.value(DewPointKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorDewPointMinValue_.value(DewPointKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorPressMaxValue_.value(PressKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorPressMinValue_.value(PressKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorLightMaxValue_.value(LightKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorLightMinValue_.value(LightKODPT));

    openknx.flash.writeByte((uint8_t)KoW90_SensorUVIMaxValue_.value(UviKODPT));
    openknx.flash.writeByte((uint8_t)KoW90_SensorUVIMinValue_.value(UviKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorWindMaxValue_.value(WindKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorWindMinValue_.value(WindKODPT));

    openknx.flash.writeFloat((float)KoW90_SensorGustMaxValue_.value(GustKODPT));
    openknx.flash.writeFloat((float)KoW90_SensorGustMinValue_.value(GustKODPT));

    openknx.flash.writeInt(m_rain_offset_value);
    // if no value was received since start, keep the restored one
    openknx.flash.writeInt(m_rain_last_recv_value >= 0 ? m_rain_last_recv_value : m_rain_last_recv_value_restored);
}

void Sensorchannel::restore()
{
    KoW90_SensorTempMaxValue_.valueNoSend(openknx.flash.readFloat(), TempKODPT);
    KoW90_SensorTempMinValue_.valueNoSend(openknx.flash.readFloat(), TempKODPT);

    KoW90_SensorHumMaxValue_.valueNoSend(openknx.flash.readFloat(), HumKODPT);
    KoW90_SensorHumMinValue_.valueNoSend(openknx.flash.readFloat(), HumKODPT);

    KoW90_SensorAbsHumMaxValue_.valueNoSend(openknx.flash.readFloat(), AbsHumKODPT);
    KoW90_SensorAbsHumMinValue_.valueNoSend(openknx.flash.readFloat(), AbsHumKODPT);

    KoW90_SensorDewPointMaxValue_.valueNoSend(openknx.flash.readFloat(), DewPointKODPT);
    KoW90_SensorDewPointMinValue_.valueNoSend(openknx.flash.readFloat(), DewPointKODPT);

    KoW90_SensorPressMaxValue_.valueNoSend(openknx.flash.readFloat(), PressKODPT);
    KoW90_SensorPressMinValue_.valueNoSend(openknx.flash.readFloat(), PressKODPT);

    KoW90_SensorLightMaxValue_.valueNoSend(openknx.flash.readFloat(), LightKODPT);
    KoW90_SensorLightMinValue_.valueNoSend(openknx.flash.readFloat(), LightKODPT);

    KoW90_SensorUVIMaxValue_.valueNoSend(openknx.flash.readByte(), UviKODPT);
    KoW90_SensorUVIMinValue_.valueNoSend(openknx.flash.readByte(), UviKODPT);

    KoW90_SensorWindMaxValue_.valueNoSend(openknx.flash.readFloat(), WindKODPT);
    KoW90_SensorWindMinValue_.valueNoSend(openknx.flash.readFloat(), WindKODPT);

    KoW90_SensorGustMaxValue_.valueNoSend(openknx.flash.readFloat(), GustKODPT);
    KoW90_SensorGustMinValue_.valueNoSend(openknx.flash.readFloat(), GustKODPT);

    m_rain_offset_value = openknx.flash.readInt();
    m_rain_last_recv_value_restored = openknx.flash.readInt();
}
