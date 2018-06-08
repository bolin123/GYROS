#include "Sys.h"
#include "APP.h"

#define SYS_DEFAULT_WARNING_DEGREE  5
#define SYS_DEFAULT_SHUTDOWN_DEGREE 10

void SysDegreeArgSave(uint8_t warning, uint8_t shutdown)
{
    uint8_t degree[2];

	SysLog("warning = %d, shutdown = %d", warning, shutdown);
    degree[0] = warning;
    degree[1] = shutdown;
    HalFlashErase(HAL_FLASH_ANGLE_ADDR);
    HalFlashWrite(HAL_FLASH_ANGLE_ADDR, degree, sizeof(degree));
}

void SysDegreeArgLoad(uint8_t degree[2])
{
    uint8_t data[2];

    HalFlashRead(HAL_FLASH_ANGLE_ADDR, data, sizeof(data));

    if(data[0] == 0xff || data[1] == 0xff)
    {
        data[0] = SYS_DEFAULT_WARNING_DEGREE;
        data[1] = SYS_DEFAULT_SHUTDOWN_DEGREE;
    }
    memcpy(degree, data, sizeof(data));
    SysLog("warning = %d, shutdown = %d", data[0], data[1]);
}

void SysInitialize(void)
{
    HalInitialize();
    APPInitialize();
}

void SysPoll(void)
{
    APPPoll();
    HalPoll();
}

