#include "APP.h"
#include "HalCommon.h"

void APPInitialize(void)
{
    HalInitialize();
    printf("hal init\n");
}

void APPPoll(void)
{
    HalPoll();
}

