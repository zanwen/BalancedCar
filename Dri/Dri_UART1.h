#ifndef __DRI_UART1_H__
#define __DRI_UART1_H__

#include "stm32f10x.h"

void Dri_UART1_Init(void);

void Dri_UART1_ReceiveCallback(uint8_t *buf, uint8_t size);

void Dri_UART1_SendByte(uint8_t byte);

#endif /* __DRI_UART1_H__ */
