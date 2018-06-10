#include "HalCommon.h"
#include "HalGPIO.h"
#include "HalTimer.h"
#include "HalUart.h"
#include "HalSpi.h"
#include "HalWait.h"
#include "CC1101.h"
#include "Protocol.h"

#define HAL_DEBUG_UART_PORT 1 //uart2

static uint32_t volatile g_timerCount = 0;
static bool g_intEnable = true;

//redirect "printf()"
int fputc(int ch, FILE *f)
{
	//HalUartWrite(HAL_DEBUG_UART_PORT, (const uint8_t *)&ch, 1);
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

static uint8_t g_buff[64];
static uint8_t g_xyz[6];

static uint8_t check(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint8_t result = data[0];

    for(i = 1; i < len; i++)
    {
        result = result ^ data[i];
    }
    return result;
}

static void poseDataRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;
    static uint8_t dCount = 0;
    static uint8_t msgLength = 1;

    for(i = 0; i < len; i++)
    {
        g_buff[dCount++] = data[i];

        if(dCount == 1)
        {
            if(data[i] != 0xA7)
            {
                dCount = 0;
            }
        }
        else if(dCount == 2)
        {
            if(data[i] != 0x7A)
            {
                dCount = 0;
            }
        }
        else if(dCount == 3)
        {
            if(data[i] != 0x72)
            {
                dCount = 0;
            }
        }
        else if(dCount == 4)
        {
            msgLength = data[i];
            if(msgLength > (sizeof(g_buff) - 4))
            {
                dCount = 0;
            }
        }
        else if(dCount == msgLength + 4)
        {
            if(g_buff[4] == check(&g_buff[5], msgLength - 1))
            {
                memcpy(g_xyz, &g_buff[5], sizeof(g_xyz));
            }
            dCount = 0;
            msgLength = 1;
        }
#if 0
        if(dCount == 1)
        {
            if(data[i] != 0xAA)
            {
                dCount = 0;
            }
        }
        else if(dCount == 8)
        {
            if(data[i] == 0x55)
            {
                memcpy(g_xyz, &g_buff[1], sizeof(g_xyz));
            }
            dCount = 0;
        }
#endif
    }
}

static void debugUartInit(void)
{
    HalUartConfig_t uartConfig;
    uartConfig.baudrate = 115200;
    uartConfig.parity = 0;
    uartConfig.flowControl = 0;
    uartConfig.wordLength = USART_WordLength_8b;
    uartConfig.recvCb = poseDataRecv;
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

extern void ProtocolDataRecv(uint8_t *data, uint8_t len);
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
    CC1101Initialize(ProtocolDataRecv);
//    MPU6500Initialize();
//    ledConfig();

    printf("cc1101 id = %d\n", CC1101ReadID());
}

void HalGyroAdjust(void)
{
    uint8_t data[4] = {0xd6, 0x6d, 0x10, 0x10};
    HalUartWrite(HAL_DEBUG_UART_PORT, (const uint8_t *)data, sizeof(data));
}

static void testReadGyro(void)
{
    static uint32_t lastTime = 0;
    //int16_t yaw, pitch, roll;
    //float y, p, r;

    if(HalRunningTime() > 10000 && HalRunningTime() - lastTime > 300)
    {
        HalInterruptsSetEnable(false);
        ProtocolSendData(PROTOCOL_CMD_DATA, g_xyz, sizeof(g_xyz));
        #if 0
        //CC1101SendData(g_xyz, sizeof(g_xyz));
        yaw = (g_xyz[1] << 8) + g_xyz[0];
        pitch = (g_xyz[3] << 8) + g_xyz[2];
        roll = (g_xyz[5] << 8) + g_xyz[4];
        y = yaw / 100.0;
        p = pitch / 100.0;
        r = roll /100.0;
        
#endif
        HalInterruptsSetEnable(true);
        //printf("yaw=%0.1f, pitch=%0.1f, roll=%0.1f\n", y, p, r);
        lastTime = HalRunningTime();
    }
}

void HalPoll(void)
{
    HalGPIOPoll();
    HalSpiPoll();
    HalUartPoll();
    //HalIwdtFeed();
    CC1101Poll();
    testReadGyro();
}

