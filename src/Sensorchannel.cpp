#include "Sensorchannel.h"
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
    KoW90_SensorHumMinValue_.valueNoSend((float)1000, HumKODPT);
    KoW90_SensorHumMaxValue_.valueNoSend((float)-1000, HumKODPT);
    KoW90_SensorAbsHumMinValue_.valueNoSend((float)1000, AbsHumKODPT);
    KoW90_SensorAbsHumMaxValue_.valueNoSend((float)-1000, AbsHumKODPT);
    KoW90_SensorDewPointMinValue_.valueNoSend((float)1000, DewPointKODPT);
    KoW90_SensorDewPointMaxValue_.valueNoSend((float)-1000, DewPointKODPT);
    KoW90_SensorPressMinValue_.valueNoSend((float)1000, PressKODPT);
    KoW90_SensorPressMaxValue_.valueNoSend((float)-1000, PressKODPT);

    
    // Debug

    logDebugP("Setup");
    logDebugP("ParamW90_Sensortype_                       : %i", ParamW90_Address_                          );
    
    logDebugP("Temperature Parameters:");
    logDebugP("ParamW90_SensorTemperatureSendChangeAmount_: %f", ParamW90_SensorTemperatureSendChangeAmount_);
    logDebugP("ParamW90_SensorTemperatureSendCycle_       : %i", ParamW90_SensorTemperatureSendCycle_       );
    logDebugP("ParamW90_SensorTemperatureAlign_           : %f", ParamW90_SensorTemperatureAlign_           );
    logDebugP("ParamW90_SensorTemperatureWarnL_           : %f", ParamW90_SensorTemperatureWarnL_           );
    logDebugP("ParamW90_SensorTemperatureWarnH_           : %f", ParamW90_SensorTemperatureWarnH_           );
    logDebugP("ParamW90_SensorTemperatureMinMax_          : %i", ParamW90_SensorTemperatureMinMax_          );
	/*logDebugP("Humidity Parameters:");
    logDebugP("ParamW90_SensorHumiditySendChangeAmount_   : %f", ParamW90_SensorHumiditySendChangeAmount_   );
    logDebugP("ParamW90_SensorHumiditySendCycle_          : %i", ParamW90_SensorHumiditySendCycle_          );
    logDebugP("ParamW90_SensorHumidityAlign_              : %f", ParamW90_SensorHumidityAlign_              );
    logDebugP("ParamW90_SensorHumidityWarnL_              : %f", ParamW90_SensorHumidityWarnL_              );
    logDebugP("ParamW90_SensorHumidityWarnH_              : %f", ParamW90_SensorHumidityWarnH_              );
    logDebugP("ParamW90_SensorHumidityMinMax_             : %i", ParamW90_SensorHumidityMinMax_             );
    logDebugP("ParamW90_SensorHumiditySend_               : %i", ParamW90_SensorHumiditySend_               );
    logDebugP("Absolute Humidity Parameters:");
    logDebugP("ParamW90_SensorAbsHumiditySendChangeAmount_: %f", ParamW90_SensorAbsHumiditySendChangeAmount_);
    logDebugP("ParamW90_SensorAbsHumiditySendCycle_       : %i", ParamW90_SensorAbsHumiditySendCycle_       );
    logDebugP("ParamW90_SensorAbsHumidityAlign_           : %f", ParamW90_SensorAbsHumidityAlign_           );
    logDebugP("ParamW90_SensorAbsHumidityWarnL_           : %f", ParamW90_SensorAbsHumidityWarnL_           );
    logDebugP("ParamW90_SensorAbsHumidityWarnH_           : %f", ParamW90_SensorAbsHumidityWarnH_           );
    logDebugP("ParamW90_SensorAbsHumidityMinMax_          : %i", ParamW90_SensorAbsHumidityMinMax_          );
    logDebugP("Dew Point Parameters:");
    logDebugP("ParamW90_SensorDewPointSendChangeAmount_   : %f", ParamW90_SensorDewPointSendChangeAmount_   );
    logDebugP("ParamW90_SensorDewPointSendCycle_          : %i", ParamW90_SensorDewPointSendCycle_          );
    logDebugP("ParamW90_SensorDewPointAlign_              : %f", ParamW90_SensorDewPointAlign_              );
    logDebugP("ParamW90_SensorDewPointWarnL_              : %f", ParamW90_SensorDewPointWarnL_              );
    logDebugP("ParamW90_SensorDewPointWarnH_              : %f", ParamW90_SensorDewPointWarnH_              );
    logDebugP("ParamW90_SensorDewPointMinMax_             : %i", ParamW90_SensorDewPointMinMax_             );
	logDebugP("Pressure Parameters:");
    logDebugP("ParamW90_SensorPressureSendChangeAmount_   : %f", ParamW90_SensorPressureSendChangeAmount_   );
    logDebugP("ParamW90_SensorPressureSendCycle_          : %i", ParamW90_SensorPressureSendCycle_          );
    logDebugP("ParamW90_SensorPressureAlign_              : %f", ParamW90_SensorPressureAlign_              );
    logDebugP("ParamW90_SensorPressureWarnL_              : %f", ParamW90_SensorPressureWarnL_              );
    logDebugP("ParamW90_SensorPressureWarnH_              : %f", ParamW90_SensorPressureWarnH_              );
    logDebugP("ParamW90_SensorPressureMinMax_             : %i", ParamW90_SensorPressureMinMax_             );
	logDebugP("Binary Input 0 Parameters:");
    logDebugP("ParamW90_Input0DebounceTime_               : %i", ParamW90_Input0DebounceTime_               );
    logDebugP("ParamW90_Input0ActionOpen_                 : %i", ParamW90_Input0ActionOpen_                 );
    logDebugP("ParamW90_Input0ActionClosed_               : %i", ParamW90_Input0ActionClosed_               );
    logDebugP("ParamW90_Input0SendCycle_                  : %i", ParamW90_Input0SendCycle_                  );
    logDebugP("Binary Input 1 Parameters:");
    logDebugP("ParamW90_Input1DebounceTime_               : %i", ParamW90_Input1DebounceTime_               );
    logDebugP("ParamW90_Input1ActionOpen_                 : %i", ParamW90_Input1ActionOpen_                 );
    logDebugP("ParamW90_Input1ActionClosed_               : %i", ParamW90_Input1ActionClosed_               );
    logDebugP("ParamW90_Input1SendCycle_                  : %i", ParamW90_Input1SendCycle_                  );
    logDebugP("Temperature2 Parameters:");
    logDebugP("ParamW90_SensorTemperature2SendChangeAmount: %f", ParamW90_SensorTemperature2SendChangeAmount_);
    logDebugP("ParamW90_SensorTemperature2SendCycle_      : %i", ParamW90_SensorTemperature2SendCycle_       );
    logDebugP("ParamW90_SensorTemperature2Align_          : %f", ParamW90_SensorTemperature2Align_           );
    logDebugP("ParamW90_SensorTemperature2WarnL_          : %f", ParamW90_SensorTemperature2WarnL_           );
    logDebugP("ParamW90_SensorTemperature2WarnH_          : %f", ParamW90_SensorTemperature2WarnH_           );
    logDebugP("ParamW90_SensorTemperature2MinMax_         : %i", ParamW90_SensorTemperature2MinMax_          );
    */
    logDebugP("-------------------------------------------");
}

void Sensorchannel::loop()
{
    float temperature = m_hwSensors->GetTemperature(_channelIndex);
    float humidity = m_hwSensors->GetHumidity(_channelIndex);
    float abshumidity = CalcAbsHumidity(humidity, temperature);
    float dewpoint = CalcDewPoint(humidity, temperature);
    float pressure = m_hwSensors->GetPressure(_channelIndex);

    loop_temperature(temperature);
    loop_humidity(humidity);
    loop_abshumidity(abshumidity);
    loop_dewpoint(dewpoint);
    loop_pressure(pressure);
}


void Sensorchannel::loop_temperature(float temperature)
{
    if(!isnan(temperature))
    {
        uint8_t send_cycle = ParamW90_SensorTemperatureSendCycle_;
        uint32_t send_millis = send_cycle * 60000;
        bool sendnow = false;
        if(send_cycle)
        {
            sendnow = millis() - m_temperature_last_send_millis > send_millis || m_temperature_last_send_millis == 0;
        }
        if(!sendnow)
        {
            float SendTresh = ParamW90_SensorTemperatureSendChangeAmount_;
            if(SendTresh != 0)
            {
                float current_temp_diff = temperature - m_temperature_last_send_value;
                sendnow = current_temp_diff >= SendTresh || 0 - current_temp_diff >= SendTresh;
                if(sendnow)
                {
                    logDebugP("Send TempKO Diff: %f", current_temp_diff);
                }
            }
        }
        
        if(sendnow)
        {
            KoW90_SensorTemp_.value(temperature + ParamW90_SensorTemperatureAlign_, TempKODPT);
            logDebugP("Send TempKO: %f", temperature + ParamW90_SensorTemperatureAlign_);
            m_temperature_last_send_millis = millis();
            m_temperature_last_send_value = temperature;
        }
        else
        {
            KoW90_SensorTemp_.valueNoSend(temperature + ParamW90_SensorTemperatureAlign_, TempKODPT);
        }

        if(ParamW90_SensorTemperatureMinMax_)   // Min Max values enabled
        {
            if(temperature + ParamW90_SensorTemperatureAlign_ > (float)KoW90_SensorTempMaxValue_.value(TempKODPT))
            {
                KoW90_SensorTempMaxValue_.valueNoSend(temperature, TempKODPT);
            }
            if(temperature + ParamW90_SensorTemperatureAlign_ < (float)KoW90_SensorTempMinValue_.value(TempKODPT))
            {
                KoW90_SensorTempMinValue_.valueNoSend(temperature, TempKODPT);
            }
        }

        if(!(ParamW90_SensorTemperatureWarnL_ == 0 && ParamW90_SensorTemperatureWarnH_ == 0))   // not both are 0 (=> feature disabled)
        {
            bool AlarmH = temperature + ParamW90_SensorTemperatureAlign_ > ParamW90_SensorTemperatureWarnH_;
            if( (bool)KoW90_SensorTempAlarmH_.value(Dpt(1,5)) != AlarmH ||                                          // alarm value has changed
                (AlarmH && millis() - m_temperature_alarmH_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorTempAlarmH_.value(AlarmH, Dpt(1,5));
                m_temperature_alarmH_last_send_millis = millis();
            }

            bool AlarmL = temperature + ParamW90_SensorTemperatureAlign_ < ParamW90_SensorTemperatureWarnL_;
            if( (bool)KoW90_SensorTempAlarmL_.value(Dpt(1,5)) != AlarmL ||                                          // alarm value has changed
                (AlarmL && millis() - m_temperature_alarmL_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorTempAlarmL_.value(AlarmL, Dpt(1,5));
                m_temperature_alarmL_last_send_millis = millis();
            }
        }
    }
}

void Sensorchannel::loop_humidity(float humidity)
{
    if(!isnan(humidity))
    {
        uint8_t send_cycle = ParamW90_SensorHumiditySendCycle_;
        uint32_t send_millis = send_cycle * 60000;
        bool sendnow = false;
        if(send_cycle)
        {
            sendnow = millis() - m_humidity_last_send_millis > send_millis || m_humidity_last_send_millis == 0;
        }
        if(!sendnow)
        {
            float SendTresh = ParamW90_SensorHumiditySendChangeAmount_;
            if(SendTresh != 0)
            {
                float current_diff = humidity - m_humidity_last_send_value;
                sendnow = current_diff >= SendTresh || 0 - current_diff >= SendTresh;
            }
        }
        
        if(sendnow)
        {
            KoW90_SensorHum_.value(humidity + ParamW90_SensorHumidityAlign_, HumKODPT);
            m_humidity_last_send_millis = millis();
            m_humidity_last_send_value = humidity;
        }
        else
        {
            KoW90_SensorHum_.valueNoSend(humidity + ParamW90_SensorHumidityAlign_, HumKODPT);
        }

        if(ParamW90_SensorHumidityMinMax_)   // Min Max values enabled
        {
            if(humidity + ParamW90_SensorHumidityAlign_ > (float)KoW90_SensorHumMaxValue_.value(HumKODPT))
            {
                KoW90_SensorHumMaxValue_.valueNoSend(humidity, HumKODPT);
            }
            if(humidity + ParamW90_SensorHumidityAlign_ < (float)KoW90_SensorHumMinValue_.value(HumKODPT))
            {
                KoW90_SensorHumMinValue_.valueNoSend(humidity, HumKODPT);
            }
        }

        if(!(ParamW90_SensorHumidityWarnL_ == 0 && ParamW90_SensorHumidityWarnH_ == 0))   // not both are 0 (=> feature disabled)
        {
            bool AlarmH = humidity + ParamW90_SensorHumidityAlign_ > ParamW90_SensorHumidityWarnH_;
            if( (bool)KoW90_SensorHumAlarmH_.value(Dpt(1,5)) != AlarmH ||                                          // alarm value has changed
                (AlarmH && millis() - m_humidity_alarmH_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorHumAlarmH_.value(AlarmH, Dpt(1,5));
                m_humidity_alarmH_last_send_millis = millis();
            }

            bool AlarmL = humidity + ParamW90_SensorHumidityAlign_ < ParamW90_SensorHumidityWarnL_;
            if( (bool)KoW90_SensorHumAlarmL_.value(Dpt(1,5)) != AlarmL ||                                          // alarm value has changed
                (AlarmL && millis() - m_humidity_alarmL_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorHumAlarmL_.value(AlarmL, Dpt(1,5));
                m_humidity_alarmL_last_send_millis = millis();
            }
        }
    }
}

void Sensorchannel::loop_abshumidity(float abshumidity) 
{
    if(!isnan(abshumidity))
    {
        uint8_t send_cycle = ParamW90_SensorAbsHumiditySendCycle_;
        uint32_t send_millis = send_cycle * 60000;
        bool sendnow = false;
        if(send_cycle)
        {
            sendnow = millis() - m_abshumidity_last_send_millis > send_millis || m_abshumidity_last_send_millis == 0;
        }
        if(!sendnow)
        {
            float SendTresh = ParamW90_SensorAbsHumiditySendChangeAmount_;
            if(SendTresh != 0)
            {
                float current_diff = abshumidity - m_abshumidity_last_send_value;
                sendnow = current_diff >= SendTresh || 0 - current_diff >= SendTresh;
            }
        }
        
        if(sendnow)
        {
            KoW90_SensorAbsHum_.value(abshumidity + ParamW90_SensorAbsHumidityAlign_, AbsHumKODPT);
            m_abshumidity_last_send_millis = millis();
            m_abshumidity_last_send_value = abshumidity;
        }
        else
        {
            KoW90_SensorAbsHum_.valueNoSend(abshumidity + ParamW90_SensorAbsHumidityAlign_, AbsHumKODPT);
        }

        if(ParamW90_SensorAbsHumidityMinMax_)   // Min Max values enabled
        {
            if(abshumidity + ParamW90_SensorAbsHumidityAlign_ > (float)KoW90_SensorAbsHumMaxValue_.value(AbsHumKODPT))
            {
                KoW90_SensorAbsHumMaxValue_.valueNoSend(abshumidity, AbsHumKODPT);
            }
            if(abshumidity + ParamW90_SensorAbsHumidityAlign_ < (float)KoW90_SensorAbsHumMinValue_.value(AbsHumKODPT))
            {
                KoW90_SensorAbsHumMinValue_.valueNoSend(abshumidity, AbsHumKODPT);
            }
        }

        if(!(ParamW90_SensorAbsHumidityWarnL_ == 0 && ParamW90_SensorAbsHumidityWarnH_ == 0))   // not both are 0 (=> feature disabled)
        {
            bool AlarmH = abshumidity + ParamW90_SensorAbsHumidityAlign_ > ParamW90_SensorAbsHumidityWarnH_;
            if( (bool)KoW90_SensorAbsHumAlarmH_.value(Dpt(1,5)) != AlarmH ||                                          // alarm value has changed
                (AlarmH && millis() - m_abshumidity_alarmH_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorAbsHumAlarmH_.value(AlarmH, Dpt(1,5));
                m_abshumidity_alarmH_last_send_millis = millis();
            }

            bool AlarmL = abshumidity + ParamW90_SensorAbsHumidityAlign_ < ParamW90_SensorAbsHumidityWarnL_;
            if( (bool)KoW90_SensorAbsHumAlarmL_.value(Dpt(1,5)) != AlarmL ||                                          // alarm value has changed
                (AlarmL && millis() - m_abshumidity_alarmL_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorAbsHumAlarmL_.value(AlarmL, Dpt(1,5));
                m_abshumidity_alarmL_last_send_millis = millis();
            }
        }
    }
}

void Sensorchannel::loop_dewpoint(float dewpoint)
{
    if(!isnan(dewpoint))
    {
        uint8_t send_cycle = ParamW90_SensorDewPointSendCycle_;
        uint32_t send_millis = send_cycle * 60000;
        bool sendnow = false;
        if(send_cycle)
        {
            sendnow = millis() - m_dewpoint_last_send_millis > send_millis || m_dewpoint_last_send_millis == 0;
        }
        if(!sendnow)
        {
            float SendTresh = ParamW90_SensorDewPointSendChangeAmount_;
            if(SendTresh != 0)
            {
                float current_diff = dewpoint - m_dewpoint_last_send_value;
                sendnow = current_diff >= SendTresh || 0 - current_diff >= SendTresh;
            }
        }
        
        if(sendnow)
        {
            KoW90_SensorDewPoint_.value(dewpoint + ParamW90_SensorDewPointAlign_, DewPointKODPT);
            m_dewpoint_last_send_millis = millis();
            m_dewpoint_last_send_value = dewpoint;
        }
        else
        {
            KoW90_SensorDewPoint_.valueNoSend(dewpoint + ParamW90_SensorDewPointAlign_, DewPointKODPT);
        }

        if(ParamW90_SensorDewPointMinMax_)   // Min Max values enabled
        {
            if(dewpoint + ParamW90_SensorDewPointAlign_ > (float)KoW90_SensorDewPointMaxValue_.value(DewPointKODPT))
            {
                KoW90_SensorDewPointMaxValue_.valueNoSend(dewpoint, DewPointKODPT);
            }
            if(dewpoint + ParamW90_SensorDewPointAlign_ < (float)KoW90_SensorDewPointMinValue_.value(DewPointKODPT))
            {
                KoW90_SensorDewPointMinValue_.valueNoSend(dewpoint, DewPointKODPT);
            }
        }

        if(!(ParamW90_SensorDewPointWarnL_ == 0 && ParamW90_SensorDewPointWarnH_ == 0))   // not both are 0 (=> feature disabled)
        {
            bool AlarmH = dewpoint + ParamW90_SensorDewPointAlign_ > ParamW90_SensorDewPointWarnH_;
            if( (bool)KoW90_SensorDewPointAlarmH_.value(Dpt(1,5)) != AlarmH ||                                          // alarm value has changed
                (AlarmH && millis() - m_dewpoint_alarmH_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorDewPointAlarmH_.value(AlarmH, Dpt(1,5));
                m_dewpoint_alarmH_last_send_millis = millis();
            }

            bool AlarmL = dewpoint + ParamW90_SensorDewPointAlign_ < ParamW90_SensorDewPointWarnL_;
            if( (bool)KoW90_SensorDewPointAlarmL_.value(Dpt(1,5)) != AlarmL ||                                          // alarm value has changed
                (AlarmL && millis() - m_dewpoint_alarmL_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorDewPointAlarmL_.value(AlarmL, Dpt(1,5));
                m_dewpoint_alarmL_last_send_millis = millis();
            }
        }
    }
}

void Sensorchannel::loop_pressure(float pressure)
{
    uint32_t send_cycle = ParamW90_SensorPressureSendCycle_;
    uint32_t send_millis = send_cycle * 60000;
    bool sendnow = false;
    //logDebugP("loop_pressure");
    if(!isnan(pressure) )
    {
        //logDebugP("loop_pressure 2");
        if(send_cycle)
        {
            sendnow = millis() - m_pressure_last_send_millis > send_millis || m_pressure_last_send_millis == 0;
        }
        if(!sendnow)
        {
            float SendTresh = ParamW90_SensorPressureSendChangeAmount_;
            if(SendTresh != 0)
            {
                float current_diff = pressure - m_pressure_last_send_value;
                sendnow = current_diff >= SendTresh || 0 - current_diff >= SendTresh;
            }
        }
        
        if(sendnow)
        {
            logDebugP("loop_pressure %f ", pressure + ParamW90_SensorPressureAlign_);
            KoW90_SensorPress_.value(pressure + ParamW90_SensorPressureAlign_, PressKODPT);
            m_pressure_last_send_millis = millis();
            m_pressure_last_send_value = pressure;
        }
        else
        {
            KoW90_SensorPress_.valueNoSend(pressure + ParamW90_SensorPressureAlign_, PressKODPT);
        }

        if(ParamW90_SensorPressureMinMax_)   // Min Max values enabled
        {
            if(pressure + ParamW90_SensorPressureAlign_ > (float)KoW90_SensorPressMaxValue_.value(PressKODPT))
            {
                KoW90_SensorPressMaxValue_.valueNoSend(pressure, PressKODPT);
            }
            if(pressure + ParamW90_SensorPressureAlign_ < (float)KoW90_SensorPressMinValue_.value(PressKODPT))
            {
                KoW90_SensorPressMinValue_.valueNoSend(pressure, PressKODPT);
            }
        }

        if(!(ParamW90_SensorPressureWarnL_ == 0 && ParamW90_SensorPressureWarnH_ == 0))   // not both are 0 (=> feature disabled)
        {
            bool AlarmH = pressure + ParamW90_SensorPressureAlign_ > ParamW90_SensorPressureWarnH_;
            if( (bool)KoW90_SensorPressAlarmH_.value(Dpt(1,5)) != AlarmH ||                                          // alarm value has changed
                (AlarmH && millis() - m_pressure_alarmH_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorPressAlarmH_.value(AlarmH, Dpt(1,5));
                m_pressure_alarmH_last_send_millis = millis();
            }

            bool AlarmL = pressure + ParamW90_SensorPressureAlign_ < ParamW90_SensorPressureWarnL_;
            if( (bool)KoW90_SensorPressAlarmL_.value(Dpt(1,5)) != AlarmL ||                                          // alarm value has changed
                (AlarmL && millis() - m_pressure_alarmL_last_send_millis > send_millis))     // alarm is true and has not been sent for send_millis
            {
                KoW90_SensorPressAlarmL_.value(AlarmL, Dpt(1,5));
                m_pressure_alarmL_last_send_millis = millis();
            }
        }
    }
}

void Sensorchannel::processInputKo(GroupObject& ko)
{
    logTraceP("processInputKo");

    float setvalue = 0;

    switch(RelKO(ko.asap()))
    {
        case W90_KoSensorTempMinMaxReset_:
            setvalue = KoW90_SensorTemp_.value(TempKODPT);
            KoW90_SensorTempMaxValue_.valueNoSend(setvalue, TempKODPT);
            KoW90_SensorTempMinValue_.valueNoSend(setvalue, TempKODPT);
        break;
        case W90_KoSensorHumMinMaxReset_:
            setvalue = knx.getGroupObject(AbsKO(W90_KoSensorHum_)).value(HumKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorHumMinValue_)).valueNoSend(setvalue, HumKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorHumMaxValue_)).valueNoSend(setvalue, HumKODPT);
        break;
        case W90_KoSensorAbsHumMinMaxReset_:
            setvalue = knx.getGroupObject(AbsKO(W90_KoSensorAbsHum_)).value(AbsHumKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorAbsHumMinValue_)).valueNoSend(setvalue, AbsHumKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorAbsHumMaxValue_)).valueNoSend(setvalue, AbsHumKODPT);
        break;
        case W90_KoSensorDewPointMinMaxReset_:
            setvalue = knx.getGroupObject(AbsKO(W90_KoSensorDewPoint_)).value(DewPointKODPT);
            knx.getGroupObject(AbsKO( W90_KoSensorDewPointMinValue_)).valueNoSend(setvalue, DewPointKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorDewPointMaxValue_)).valueNoSend(setvalue, DewPointKODPT);
        break;
        case W90_KoSensorPressMinMaxReset_:
            setvalue = knx.getGroupObject(AbsKO(W90_KoSensorPress_)).value(PressKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorPressMinValue_)).valueNoSend(setvalue, PressKODPT);
            knx.getGroupObject(AbsKO(W90_KoSensorPressMaxValue_)).valueNoSend(setvalue, PressKODPT);
        break;
    }
}

float Sensorchannel::CalcDewPoint(float relative_humidity, float temperature)
{
   float a,b;
    if( temperature >= 0 )
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
    double sdd = 6.1078 * pow(10.0, ( (a * temperature) / (b + temperature) ) ); //Sättigungsdampfdruck
    double dd = (relative_humidity / 100.0) * sdd; //Dampfdruck
    double v = log10 ( (dd / 6.1078) / log10(10));

    double value = (b * v) / (a - v); //Taupunkt 
    return (float)value;
}

float Sensorchannel::CalcAbsHumidity(float relative_humidity, float temperature)
{
    //input in 1°C , 0-100% humidity
    //ouput in g/m³

    float value = (6.112 * pow(2.718, (17.67 * temperature)/(temperature+243.5)) * relative_humidity * 2.1674) / (273.15 + temperature);
    return value;
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
}

