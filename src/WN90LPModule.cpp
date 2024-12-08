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
/*
    uint16_t asap = ko.asap();
    int16_t channelnumber = (asap - THP_KoOffset ) / THP_KoBlockSize;
    if(channelnumber < THP_ChannelCount)
        if(_Sensorchannels[channelnumber] != nullptr)
            _Sensorchannels[channelnumber]->processInputKo(ko);
*/
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
    return 0; //1 + (THP_ChannelCount * 2 * 5 * 4);
}

void WN90LPModule::readFlash(const uint8_t *iBuffer, const uint16_t iSize)
{/*
    if (iSize == 0) // first call - without data
        return;

    uint8_t lVersion = openknx.flash.readByte();
    if (lVersion != 1) // version unknown
    {
        logDebugP("Wrong version of flash data (%i)", lVersion);
        return;
    }

    logDebugP("Reading channel data from flash (%i)", THP_ChannelCount);
    for (uint8_t lIndex = 0; lIndex < THP_ChannelCount; lIndex++)
    {
        _Sensorchannels[lIndex]->restore();
    }*/
}

void WN90LPModule::writeFlash()
{
    /*
    openknx.flash.writeByte(1); // Version
    for (uint8_t lIndex = 0; lIndex < W90_ChannelCount; lIndex++)
    {
        _Sensorchannels[lIndex]->save();
    }
    */
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


/*

#include <ModbusMaster.h>


// instantiate ModbusMaster object
ModbusMaster node;


void preTransmission()
{
    digitalWrite(DIR_PIN, 1);
}

void postTransmission()
{
    digitalWrite(DIR_PIN, 0);
}


void setup()
{
  while(!Serial); // ait for USB serial

  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, 0);
  Serial.println("Set Pin 3 to Output and LOW");

  Serial1.begin(9600, SERIAL_8N1);
  Serial.println("Set Serial1 9600 8N1");

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node.begin(0x90, Serial1);
  Serial.println("Modbus Slave ID 0x90");

  Serial.println("Setup end");
}


void loop()
{
  uint8_t result;
  uint16_t data[6];  

  Serial.println("readHoldingRegisters(cmd 0x03) 0x0160, Length 0x0001");
  result = node.readHoldingRegisters(0x0160, 0x0001);

  Serial.print("Result: 0x");
  Serial.println(result, HEX);

  data[0] = node.getResponseBuffer(0);
  Serial.print("ResponseBuffer: ");
  Serial.println(data[0], HEX);

  Serial.println("Pause 2s");
  delay(2000);
}

*/