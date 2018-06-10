#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "HalCommon.h"

typedef enum
{
    PROTOCOL_CMD_ACK = 0,
    PROTOCOL_CMD_DATA,
    PROTOCOL_CMD_SELFCHECK,
}ProtocolCmd_t;

void ProtocolSendData(ProtocolCmd_t cmd, uint8_t *data, uint8_t len);
void ProtocolDataRecv(uint8_t *data, uint8_t len);
void ProtocolPoll(void);
void ProtocolInitialize(void);
#endif
