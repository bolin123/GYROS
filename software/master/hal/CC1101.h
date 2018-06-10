#ifndef CC1101_H
#define CC1101_H

#include "HalCtype.h"

typedef void (* CC1101Recv_cb)(uint8_t *data, uint8_t len);

void CC1101SendData(uint8_t *data, uint16_t len);
uint8_t CC1101ReadID(void);
void CC1101Initialize(CC1101Recv_cb callback);
void CC1101Poll(void);

#endif

