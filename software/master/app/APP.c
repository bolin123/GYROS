#include "APP.h"
#include "Sys.h"
#include "SysButton.h"
#include "Protocol.h"

#define BUTTON_0_PIN 0x0f
#define BUTTON_1_PIN 0x13
#define BUTTON_2_PIN 0x14
#define BUTTON_3_PIN 0x15

static SysButton_t g_button[4];
static uint8_t g_degree[2];
static int g_lastPitch;

static bool isWarningClear(void)
{
    SysLog("lastPitch = %d", g_lastPitch);
    if(g_lastPitch < g_degree[0] * 100)
    {
        return true;
    }
    return false;
}

static SysButtonState_t getButtonState(SysButton_t *button)
{
    if(&g_button[0] == button)
    {
        if(HalGPIOGetLevel(BUTTON_0_PIN) == 0)
        {
            return SYS_BUTTON_STATE_PRESSED;
        }
    }

    return SYS_BUTTON_STATE_RELEASED;
}

static uint8_t buttonHandle(SysButton_t *button, uint32_t pressTime, SysButtonState_t state)
{
    if(&g_button[0] == button && SYS_BUTTON_STATE_RELEASED == state)
    {
        if(pressTime > 20)
        {
            if(isWarningClear())
            {
                HalShutDownEnable(false);
                HalWarningEnable(false);
            }
            return 1;
        }
    }
    return 0;
}

static void buttonInit(void)
{
    HalGPIOConfig(BUTTON_0_PIN, HAL_IO_INPUT);
    HalGPIOConfig(BUTTON_1_PIN, HAL_IO_INPUT);
    HalGPIOConfig(BUTTON_2_PIN, HAL_IO_INPUT);
    HalGPIOConfig(BUTTON_3_PIN, HAL_IO_INPUT);

    SysButtonInitialize();

    SysButtonRegister(&g_button[0], buttonHandle, getButtonState);
    SysButtonRegister(&g_button[1], buttonHandle, getButtonState);
    SysButtonRegister(&g_button[2], buttonHandle, getButtonState);
    SysButtonRegister(&g_button[3], buttonHandle, getButtonState);
}

void APPGotPitch(int pitch)
{
    int realDegree = 9000 - pitch;

    if(realDegree > g_degree[0] * 100) //warning
    {
        HalWarningEnable(true);
    }

    if(realDegree > g_degree[1] * 100)
    {
        HalShutDownEnable(true);
    }
    g_lastPitch = realDegree;
}

static void protocolEventHandle(ProtocolCmd_t cmd, void *args)
{
    switch(cmd)
    {
        case PROTOCOL_CMD_ACK:
            break;
        case PROTOCOL_CMD_DATA:
            HalGyroDataHandle((uint8_t *)args, 6);
            break;
        case PROTOCOL_CMD_SELFCHECK:
            break;
        default:
            break;
    }
}

uint32_t APPTime(void)
{
    return HalRunningTime();
}

void APPSlaveAjust(void)
{
    SysLog("");
    ProtocolSendData(PROTOCOL_CMD_SELFCHECK, NULL, 0);
}

void APPDegreeUpdate(void)
{
    SysDegreeArgLoad(g_degree);
}

void APPInitialize(void)
{
    buttonInit();
    APPDegreeUpdate();
    ProtocolInitialize(protocolEventHandle);
}

void APPPoll(void)
{
    SysButtonPoll();
}

