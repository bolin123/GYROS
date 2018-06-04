#ifndef MPUI2C_H
#define MPUI2C_H

#include "HalCtype.h"

int8_t MPUI2CReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int MPUI2CWriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);
bool MPUI2CWriteByte(unsigned char slaveAddr,unsigned char regAddr,unsigned char regData);
unsigned char MPUI2CReadByte(unsigned char slaveAddr, unsigned char regAddr);
void MPUI2CInitialize(void);
void MPUI2CPoll(void);
#endif

