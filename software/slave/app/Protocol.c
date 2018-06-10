#include "Protocol.h"
#include "CC1101.h"
#include "APP.h"

#define PROTOCOL_HEAD_CODE 0xA5


typedef struct Protocol_st
{
    uint8_t head;
    uint8_t len; //data len
    uint8_t cmd;
    uint8_t data[];
}Protocol_t;

static uint8_t g_buff[64];
static uint8_t g_count = 0;

static uint8_t checkSum(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint8_t sum = 0;

    for(i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}

static void praseFrame(Protocol_t *frame)
{
    switch(frame->cmd)
    {
        case PROTOCOL_CMD_ACK:
            break;
        case PROTOCOL_CMD_DATA:
            break;
        case PROTOCOL_CMD_SELFCHECK:
            APPSelfAdjust();
            break;
        default:
            break;
    }
}

void ProtocolDataRecv(uint8_t *data, uint8_t len)
{
    uint8_t i;
    static uint8_t datalen = 0;

    for(i = 0; i < len; i++)
    {
        g_buff[g_count++] = data[i];
        if(g_count == 1)
        {
            if(data[i] != PROTOCOL_HEAD_CODE)
            {
                g_count = 0;
            }
        }
        else if(g_count == 2)
        {
            datalen = data[i];
            if(datalen > sizeof(g_buff) - sizeof(Protocol_t))
            {
                g_count = 0;
                datalen = 0;
            }
        }
        else if(g_count == datalen + sizeof(Protocol_t) + 1)//check
        {
            if(g_buff[datalen + sizeof(Protocol_t)] == checkSum(g_buff, datalen + sizeof(Protocol_t)))
            {
                praseFrame((Protocol_t *)g_buff);
            }
            datalen = 0;
            g_count = 0;
        }
    }
}

void ProtocolSendData(ProtocolCmd_t cmd, uint8_t *data, uint8_t len)
{
    uint8_t buff[64];
    Protocol_t *protocol = (Protocol_t *)buff;

    protocol->head = PROTOCOL_HEAD_CODE;
    protocol->cmd = cmd;
    protocol->len = len;
    memcpy(protocol->data, data, len);
    buff[sizeof(Protocol_t) + len] = checkSum(buff, sizeof(Protocol_t) + len);
    CC1101SendData(buff, sizeof(Protocol_t) + len + 1);
}

void ProtocolInitialize(void)
{
}

void ProtocolPoll(void)
{
}

