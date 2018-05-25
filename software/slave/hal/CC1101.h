#ifndef CC1101_H
#define CC1101_H

#include "HalCtype.h"

void CC1101SendData(uint8_t *data, uint16_t len);
uint8_t CC1101ReadID(void);
void CC1101Initialize(void);
void CC1101Poll(void);

#endif

