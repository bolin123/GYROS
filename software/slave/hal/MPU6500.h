#ifndef MPU6500_H
#define MPU6500_H

#include "HalCtype.h"

void MPU6500GyroRead(uint16_t xyz[3]);
void MPU6500Initialize(void);
void MPU6500Poll(void);
#endif

