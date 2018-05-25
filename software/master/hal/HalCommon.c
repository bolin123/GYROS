#include "HalCommon.h"
#include "HalGPIO.h"
#include "HalTimer.h"
#include "HalUart.h"
#include "HalSpi.h"
#include "HalWait.h"
#include "CC1101.h"

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

static void debugUartInit(void)
{
    HalUartConfig_t uartConfig;
    uartConfig.baudrate = 115200;
    uartConfig.parity = 0;
    uartConfig.flowControl = 0;
    uartConfig.wordLength = USART_WordLength_8b;
    uartConfig.recvCb = NULL;
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

static void cc1101DataHandle(uint8_t *data, uint16_t len)
{
    uint16_t xyz[3];

    HalGPIOSetLevel(0x01, !HalGPIOGetLevel(0x01));
    memcpy((uint8_t *)xyz, data, sizeof(xyz));

    printf("x=%d, y=%d, z=%d\n", xyz[0], xyz[1], xyz[2]);
}

static void ledConfig(void)
{
    HalGPIOConfig(0x00, HAL_IO_OUTPUT);
    HalGPIOConfig(0x01, HAL_IO_OUTPUT);
    HalGPIOSetLevel(0x00, 0);
    HalGPIOSetLevel(0x01, 1);
}

void HalInitialize(void)
{
	periphClockInit();
    HalTimerInitialize();
    HalGPIOInitialize();
    HalUartInitialize();
    HalSpiInitialize();
    //HalIwdtInitialize();
    debugUartInit();
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

