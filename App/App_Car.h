#ifndef __APP_CAR_H__
#define __APP_CAR_H__

#include "Dri_ADC1.h"
#include "Int_HallEncoder.h"
#include "Int_MPU6050.h"
#include "Int_OLED.h"
#include "stm32f10x.h"

void App_Car_Init(void);

void App_Car_OLED(void);
void App_Car_ReadSensor(void);

void App_Car_Control(void);

#endif /* __APP_CAR_H__ */
