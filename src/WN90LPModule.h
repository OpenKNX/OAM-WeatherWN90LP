#pragma once
#include "OpenKNX.h"
#include "Sensorchannel.h"
#include "HWSensors.h"


#ifndef OPENKNX_DUALCORE
#pragma message "THPSensorModule needs OPENKNX_DUALCORE"
#endif


class WN90LPModule : public OpenKNX::Module
{

  public:
    WN90LPModule();
    void loop() override;
    #ifdef OPENKNX_DUALCORE
    void loop1() override;
    #endif
    void setup() override;
    const std::string name() override;
    const std::string version() override;
    void processBeforeRestart() override;
    void processBeforeTablesUnload() override;
    void savePower() override;
    bool restorePower() override;
    void readFlash(const uint8_t* iBuffer, const uint16_t iSize) override;
    void writeFlash() override;
    uint16_t flashSize() override;
    static WN90LPModule *instance();
    bool processCommand(const std::string cmd, bool diagnoseKo) override;

  private:
    static WN90LPModule *_instance;
    void processInputKo(GroupObject &ko);
    uint8_t _curLoopChannel = 0;
    Sensorchannel *_Sensorchannels[W90_ChannelCount];
    HWSensors _HWSensors = HWSensors();
};

extern WN90LPModule openknxWN90LPModule;