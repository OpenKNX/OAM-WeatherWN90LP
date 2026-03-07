#include "FileTransferModule.h"
#include "Logic.h"
#include "OpenKNX.h"
#include "WN90LPModule.h"

#include "GpioBinaryInputModule.h"
#include "SensorDevices.h"
#include "SensorModule.h"
#include "VirtualButtonModule.h"

/* ToDo
- IMPL - Status für Station erreichbar
- IMPL - höhenkorrektur luftdruck
- RainFlow
- IMPL - Rain
- IMPL - RainGauge Overflow
- DONE - StatusLED
- DONE - fix UVI
- DONE - 2MB Flashlayout
- DONE - Common 1.7.2
*/

void setup()
{
    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);

    openknx.addModule(1, openknxLogic);
    openknx.addModule(2, openknxWN90LPModule);
    openknx.addModule(3, openknxGpioBinaryInputModule);
    openknx.addModule(4, openknxVirtualButtonModule);
    openknx.addModule(5, openknxSensorDevicesModule);
    openknx.addModule(6, openknxSensorModule);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.setup();
}

uint32_t lastmillis_1 = 0;
void loop()
{
    openknx.loop();
    if (delayCheckMillis(lastmillis_1, 5000))
    {
        // logDebug("", "Loop 5s");

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
