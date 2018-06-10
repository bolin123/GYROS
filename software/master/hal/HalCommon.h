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
 *  �����ж�ʹ��
 *  @param enable ʹ��״̬
 */
void HalInterruptsSetEnable(bool enable);

/**
 *  ��ȡ�ж�ʹ��״̬
 *  @return 0�رգ�1����
 */
bool HalInterruptsGetEnable(void);

/** ���� */
void HalReboot(void);

/** ϵͳ����ʱ�䣬���� */
uint32_t HalRunningTime(void);

/** ΢����ʱ */
void HalDelayUs(uint16_t us);

#endif // HAL_H



