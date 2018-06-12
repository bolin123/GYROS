#include "APP.h"
#include "HalCommon.h"

void APPSelfAdjust(void)
{
    static uint32_t lastTime = 0;

    if(HalRunningTime() - lastTime > 3000)
    {
        HalGyroAdjust();
        lastTime = HalRunningTime();
    }
}

void APPInitialize(void)
{
    HalInitialize();
    printf("hal init\n");
}

void APPPoll(void)
{
    HalPoll();
}

