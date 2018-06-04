#include "HalCommon.h"
#include "CC1101.h"
//#include "Sys.h"

#define HAL_DEBUG_UART_PORT 1 //uart2

static uint32_t volatile g_timerCount = 0;
static bool g_intEnable = true;

//redirect "printf()"
int fputc(int ch, FILE *f)
{
	HalUartWrite(HAL_DEBUG_UART_PORT, (const uint8_t *)&ch, 1);
	return ch;
}

static void periphClockInit(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

extern void SysCommandDataRecv(uint8_t *data, uint16_t len);
static void debugUartInit(void)
{
    HalUartConfig_t uartConfig;
    uartConfig.baudrate = 115200;
    uartConfig.parity = 0;
    uartConfig.flowControl = 0;
    uartConfig.wordLength = USART_WordLength_8b;
    uartConfig.recvCb = SysCommandDataRecv;
    HalUartConfig(HAL_DEBUG_UART_PORT, &uartConfig);
}

void HalTimerPast1ms(void)
{
    g_timerCount++;
}

bool HalInterruptsGetEnable(void)
{
    return g_intEnable;
}

void HalInterruptsSetEnable(bool enable)
{
    g_intEnable = enable;
    if(enable)
    {
        __enable_irq();
    }
    else
    {
        __disable_irq();
    }
}

void HalReboot(void)
{
    static bool reboot = false;
    if(!reboot)
    {
        reboot = true;
		//__set_FAULTMASK(1);
		NVIC_SystemReset();
    }
}

uint32_t HalRunningTime(void)
{
    return g_timerCount;
}

void HalWarningEnable(bool enable)
{
    static bool warning = false;

    if(warning != enable)
    {
        printf("%s, enable = %d\n", __FUNCTION__, enable);
        HalGPIOSetLevel(0x1e, enable); //beep
        HalGPIOSetLevel(0x00, !enable); //warning led
        warning = enable;
    }
}

void HalShutDownEnable(bool enable)
{
    static bool shutdown = false;

    if(shutdown != enable)
    {
        printf("%s, enable = %d\n", __FUNCTION__, enable);
        HalGPIOSetLevel(0x1c, enable);
        HalGPIOSetLevel(0x1d, enable);
        shutdown = enable;
    }
}

static void switchIOConfig(void)
{
    HalGPIOConfig(0x1c, HAL_IO_OUTPUT);
    HalGPIOConfig(0x1d, HAL_IO_OUTPUT);
    HalGPIOConfig(0x1e, HAL_IO_OUTPUT);

    HalGPIOSetLevel(0x1c, 0);
    HalGPIOSetLevel(0x1d, 0);
    HalGPIOSetLevel(0x1e, 0);
    HalGPIOSetLevel(0x00, 1);
}

extern void APPGotPitch(int pitch);
static void cc1101DataHandle(uint8_t *data, uint16_t len)
{
    uint8_t euler[6];
    int16_t yaw, pitch, roll;
    float y, p, r;

    HalGPIOSetLevel(0x01, !HalGPIOGetLevel(0x01));
    memcpy(euler, data, sizeof(euler));

    yaw = (data[0] << 8) + data[1];
    pitch = (data[2] << 8) + data[3];
    roll = (data[4] << 8) + data[5];

    APPGotPitch(abs(pitch));
#if 0
    if(pitch < 8000 && pitch > -8000)
    {
        switchContrl(true);
    }
#endif
    y = yaw / 100.0;
    p = pitch / 100.0;
    r = roll /100.0;

    printf("yaw=%0.1f, pitch=%0.1f, roll=%0.1f\n", y, p, r);

}

static void ledConfig(void)
{
    HalGPIOConfig(0x00, HAL_IO_OUTPUT);
    HalGPIOConfig(0x01, HAL_IO_OUTPUT);
    HalGPIOSetLevel(0x00, 1);
    HalGPIOSetLevel(0x01, 1);
}

void HalInitialize(void)
{
	periphClockInit();
    HalTimerInitialize();
    HalGPIOInitialize();
    HalUartInitialize();
    HalSpiInitialize();
    debugUartInit();
    //HalIwdtInitialize();
    switchIOConfig();
    printf("CC1101Initialize\n");
    CC1101Initialize(cc1101DataHandle);
    ledConfig();

    printf("cc1101 id = %d\n", CC1101ReadID());
}

void HalPoll(void)
{
    HalGPIOPoll();
    HalSpiPoll();
    HalUartPoll();
    //HalIwdtFeed();
    CC1101Poll();
}

