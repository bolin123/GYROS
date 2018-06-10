#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "HalCtype.h"

typedef enum
{
    PROTOCOL_CMD_ACK = 0,
    PROTOCOL_CMD_DATA,
    PROTOCOL_CMD_SELFCHECK,
}ProtocolCmd_t;

typedef void (*ProtocolEvent_cb)(ProtocolCmd_t cmd, void *args);

void ProtocolSendData(ProtocolCmd_t cmd, uint8_t *data, uint8_t len);
void ProtocolDataRecv(uint8_t *data, uint8_t len);
void ProtocolPoll(void);
void ProtocolInitialize(ProtocolEvent_cb eventcb);
#endif
