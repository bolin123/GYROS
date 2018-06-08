#ifndef SYS_H
#define SYS_H

#include "HalCtype.h"
#include "HalCommon.h"

#define HalPrintf(...) printf(__VA_ARGS__)
#define SysPrintf HalPrintf
#define SysLog(...) SysPrintf("%s: ", __FUNCTION__); SysPrintf(__VA_ARGS__); SysPrintf("\n");

void SysDegreeArgSave(uint8_t warning, uint8_t shutdown);
void SysDegreeArgLoad(uint8_t degree[2]);
void SysInitialize(void);
void SysPoll(void);
#endif


