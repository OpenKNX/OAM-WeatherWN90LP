#include "OpenKNX.h"
#include "Logic.h"
#include "FileTransferModule.h"
#include "WN90LPModule.h"

/* ToDo
- Status für Station erreichbar, Timeout Parameter
- Was tun mit den Werten wenn Station nicht erreichbar (0xffff oder letzter Wert oder 0)
- höhenkorrektur luftdruck


*/


void setup()
{
    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);


    openknx.addModule(1, openknxLogic);
    openknx.addModule(2, openknxWN90LPModule);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.setup();
}

uint32_t lastmillis_1 = 0;
void loop()
{
    openknx.loop();
    if(delayCheckMillis(lastmillis_1, 5000))
    {
        //logDebug("", "Loop 5s");


        lastmillis_1 = millis();
    }
}

#ifdef OPENKNX_DUALCORE
void setup1()
{
    openknx.setup1();
}
#endif


#ifdef OPENKNX_DUALCORE
void loop1()
{
    openknx.loop1();
}
#endif







