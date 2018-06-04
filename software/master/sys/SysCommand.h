#ifndef SYS_COMMAND_H
#define SYS_COMMAND_H

#include "Sys.h"

void SysCommandDataRecv(uint8_t *data, uint16_t len);
void SysCommandInitialize(void);
void SysCommandPoll(void);

#endif

