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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

extern void AtCmdRecv(uint8_t *data, uint16_t len);
static void debugUartInit(void)
{
    HalUartConfig_t uartConfig;
    uartConfig.baudrate = 115200;
    uartConfig.parity = 0;
    uartConfig.flowControl = 0;
    uartConfig.wordLength = USART_WordLength_8b;
    uartConfig.recvCb = AtCmdRecv;
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

void HalInitialize(void)
{
	periphClockInit();
    HalTimerInitialize();
    HalGPIOInitialize();
    HalUartInitialize();
    debugUartInit();
    //HalIwdtInitialize();


}

void HalPoll(void)
{
    HalGPIOPoll();
    HalUartPoll();
    //HalIwdtFeed();
}

