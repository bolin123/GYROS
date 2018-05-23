#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "HalCtype.h"

#define HAL_GPIO_LEVEL_LOW  0
#define HAL_GPIO_LEVEL_HIGH 1

typedef enum
{
    HAL_IO_OUTPUT,
    HAL_IO_INPUT,
}HalGPIODirect_t;

void HalGPIOSetLevel(uint8_t io, uint8_t level);
uint8_t HalGPIOGetLevel(uint8_t io);
void HalGPIOConfig(uint8_t io, HalGPIODirect_t dir);
void HalGPIOInitialize(void);
void HalGPIOPoll(void);

#endif

