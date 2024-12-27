#ifndef __INT_HALLENCODER_H__
#define __INT_HALLENCODER_H__

#include "stm32f10x.h"

typedef enum { HALL_LEFT, HALL_RIGHT } HallSensor;

void Int_HallEncoder_Init(void);

int16_t Int_HallEncoder_ReadCounter(HallSensor hallSensor);

void Int_HallEncoder_Test(void);

#endif /* __INT_HALLENCODER_H__ */
