#ifndef HAL_H
#define HAL_H

#include "HalCtype.h"
#include "HalGPIO.h"
#include "HalTimer.h"
#include "HalUart.h"
#include "HalSpi.h"
#include "HalWait.h"
#include "HalFlash.h"

void HalInitialize(void);
void HalPoll(void);
void HalWarningEnable(bool enable);
void HalShutDownEnable(bool enable);
void HalGyroDataHandle(uint8_t *data, uint16_t len);

/**
 *  设置中断使能
 *  @param enable 使能状态
 */
void HalInterruptsSetEnable(bool enable);

/**
 *  获取中断使能状态
 *  @return 0关闭，1开启
 */
bool HalInterruptsGetEnable(void);

/** 重启 */
void HalReboot(void);

/** 系统运行时间，毫秒 */
uint32_t HalRunningTime(void);

/** 微秒延时 */
void HalDelayUs(uint16_t us);

#endif // HAL_H



