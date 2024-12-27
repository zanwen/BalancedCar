#ifndef __DRI_Dri_I2C_H__
#define __DRI_Dri_I2C_H__

#include "stm32f10x.h"

void Dri_I2C_Init(void);

void Dri_I2C_Start(void);

void Dri_I2C_EnableAutoAck(void);
void Dri_I2C_DisableAutoAck(void);

void Dri_I2C_SendByte(uint8_t data);
void Dri_I2C_SendAddr(uint8_t data);

uint8_t Dri_I2C_ReceiveByte(void);

void Dri_I2C_Stop4Send(void);
void Dri_I2C_EnableStop(void);
void Dri_I2C_Test(void);


#endif /* __DRI_Dri_I2C_H__ */
