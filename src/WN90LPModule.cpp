#include "WN90LPModule.h"

WN90LPModule *WN90LPModule::_instance = nullptr;

WN90LPModule::WN90LPModule()
{
    WN90LPModule::_instance = this;
}

WN90LPModule *WN90LPModule::instance()
{
    return WN90LPModule::_instance;
}

const std::string WN90LPModule::name()
{
    return "WN90LPModule";
}

const std::string WN90LPModule::version()
{
    return "";
}

void WN90LPModule::setup()
{
    logTraceP("setup");

    uint8_t addrs[W90_ChannelCount];
    for(int i=0;i<W90_ChannelCount;i++)
        addrs[i] = knx.paramByte(W90_ParamBlockOffset+W90_ParamBlockSize*i+W90_Address_);

    _HWSensors.Setup(addrs);

    for (uint8_t i = 0; i < W90_ChannelCount; i++)
    {
        _Sensorchannels[i] = new Sensorchannel();
        _Sensorchannels[i]->Setup(i, &_HWSensors);
    }
}



void WN90LPModule::loop()
{   
    // do nothing while StartupDelay
    if(!openknx.afterStartupDelay())
        return;

    // do nothing when not parameterized
    if (!knx.configured())
        return;

    uint8_t loopedChannels = 0;
    
    while(openknx.freeLoopTime() && loopedChannels < W90_ChannelCount)
    {
        _Sensorchannels[_curLoopChannel]->loop();
        loopedChannels++;
        _curLoopChannel++;
        _curLoopChannel = _curLoopChannel % W90_ChannelCount;
    }
}

void WN90LPModule::loop1()
{
    _HWSensors.Loop();
}

void WN90LPModule::processInputKo(GroupObject &ko)
{
    // there are no global KO, so all ko belong to a channel

    uint16_t asap = ko.asap();
    int16_t channelnumber = (asap - W90_KoOffset ) / W90_KoBlockSize;
    if(channelnumber < W90_ChannelCount)
        if(_Sensorchannels[channelnumber] != nullptr)
            _Sensorchannels[channelnumber]->processInputKo(ko);

}

void WN90LPModule::processBeforeRestart()
{
    
}

void WN90LPModule::processBeforeTablesUnload()
{
    
}

void WN90LPModule::savePower()
{
    
}

bool WN90LPModule::restorePower()
{
    return true;
}

uint16_t WN90LPModule::flashSize()
{
    // Version + Data (Channel * Inputs * (Dpt + Value))
    return 1 + (W90_ChannelCount * 2 * (8 * 4 + 1));
}

void WN90LPModule::readFlash(const uint8_t *iBuffer, const uint16_t iSize)
{
    if (iSize == 0) // first call - without data
        return;

    uint8_t lVersion = openknx.flash.readByte();
    if (lVersion != W90_FLASH_VERSION) // version unknown
    {
        logDebugP("Wrong version of flash data (%i)", lVersion);
        return;
    }

    logDebugP("Reading channel data from flash (%i)", W90_ChannelCount);
    for (uint8_t lIndex = 0; lIndex < W90_ChannelCount; lIndex++)
    {
        _Sensorchannels[lIndex]->restore();
    }
}

void WN90LPModule::writeFlash()
{
    openknx.flash.writeByte(W90_FLASH_VERSION); // Version
    for (uint8_t lIndex = 0; lIndex < W90_ChannelCount; lIndex++)
    {
        _Sensorchannels[lIndex]->save();
    }
}

bool WN90LPModule::processCommand(const std::string cmd, bool diagnoseKo)
{
    if(diagnoseKo)
        return false;
    
    if(cmd == "wn90lp")
    {
        for (uint8_t lIndex = 0; lIndex < W90_ChannelCount; lIndex++)
        {
            //openknx.logger.log(_Sensorchannels[lIndex]->getValueString());            
        }
        return true;
    }
    return false;
}

WN90LPModule openknxWN90LPModule;