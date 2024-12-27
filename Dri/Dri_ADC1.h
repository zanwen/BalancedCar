#ifndef __DRI_ADC_H__
#define __DRI_ADC_H__

#include "stm32f10x.h"

void Dri_ADC1_Init(void);
void Dri_ADC1_StartConvert(void);
double Dri_ADC1_ReadVoltage(void);

#endif /* __DRI_ADC_H__ */
