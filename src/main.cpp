#include "OpenKNX.h"
#include "Logic.h"
#include "FileTransferModule.h"



void setup()
{
    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);


    openknx.addModule(1, openknxLogic);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.setup();
}

#ifdef OPENKNX_DUALCORE
void setup1()
{
    openknx.setup1();
}
#endif

void loop()
{
    openknx.loop();
}

#ifdef OPENKNX_DUALCORE
void loop1()
{
    openknx.loop1();
}
#endif