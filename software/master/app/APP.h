#ifndef APP_H
#define APP_H

#include "HalCtype.h"

void APPDegreeUpdate(void);
void APPGotPitch(int pitch);
uint32_t APPTime(void);
void APPInitialize(void);
void APPPoll(void);

#endif

