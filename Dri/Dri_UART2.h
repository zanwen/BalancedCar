#ifndef __DRI_UART2_H__
#define __DRI_UART2_H__

#include "stm32f10x.h"

void Dri_UART2_Init(void);

void Dri_UART2_ReceiveCallback(uint8_t *buf, uint8_t size);

void Dri_UART2_SendByte(uint8_t byte);
void Dri_UART2_SendBytes(uint8_t *bytes, uint16_t size);

#endif /* __DRI_UART2_H__ */
