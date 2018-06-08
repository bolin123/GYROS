#include "HalCommon.h"
#include "VTStaticQueue.h"

#define HAL_GPRS_UART_PORT 0

static VTSQueueDef(uint8_t, g_recvQueue, 1024);
static uint16_t g_count = 0;
static char g_buff[512];

static void gprsDataRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        if(VTSQueueHasSpace(g_recvQueue))
        {
            VTSQueuePush(g_recvQueue, data[i]);
        }
    }
}

static void gprsPrintPoll(void)
{
    uint8_t byte;

    while(VTSQueueCount(g_recvQueue))
    {
        HalInterruptsSetEnable(false);
        byte = VTSQueueFront(g_recvQueue);
        VTSQueuePop(g_recvQueue);
        HalInterruptsSetEnable(true);

        g_buff[g_count++] = byte;

        if(byte == '\n' || byte == '>')
        {
            g_buff[g_count] = '\0';
            printf("%s", g_buff);
            g_count = 0;
        }
    }
}

static void atcmdUartInit(void)
{
    HalUartConfig_t config;

    config.baudrate = 9600;
    config.flowControl = 0;
    config.parity = 0;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = gprsDataRecv;
    HalUartConfig(HAL_GPRS_UART_PORT, &config);
}

static uint8_t g_cmdBuff[255];
static uint8_t g_cmdCount = 0;
void AtCmdRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;
    for(i = 0; i < len; i++)
    {
        g_cmdBuff[g_cmdCount++] = data[i];
        if(data[i] == '\n')
        {
            HalUartWrite(HAL_GPRS_UART_PORT, g_cmdBuff, g_cmdCount);
            g_cmdCount = 0;
        }
    }
}

#define HAL_GPRS_POWER_PIN 0x00

static void wakeup(void)
{
    HalGPIOConfig(HAL_GPRS_POWER_PIN, HAL_IO_OUTPUT);
    HalGPIOSetLevel(HAL_GPRS_POWER_PIN, 0);
    HalWaitCondition(true, 1500);
    HalGPIOSetLevel(HAL_GPRS_POWER_PIN, 1);
    printf("wakeup\n");
}

void APPPoll(void)
{
    gprsPrintPoll();
    HalPoll();
}

void APPInitialize(void)
{
    HalInitialize();
    atcmdUartInit();
    wakeup();
}

