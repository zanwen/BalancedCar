#ifndef __INT_TB6612_H__
#define __INT_TB6612_H__

#include "stm32f10x.h"

#define AIN1(x) ((x) ? (GPIOB->ODR |= GPIO_ODR_ODR14) : (GPIOB->ODR &= ~GPIO_ODR_ODR14))
#define AIN2(x) ((x) ? (GPIOB->ODR |= GPIO_ODR_ODR15) : (GPIOB->ODR &= ~GPIO_ODR_ODR15))
#define BIN1(x) ((x) ? (GPIOB->ODR |= GPIO_ODR_ODR13) : (GPIOB->ODR &= ~GPIO_ODR_ODR13))
#define BIN2(x) ((x) ? (GPIOB->ODR |= GPIO_ODR_ODR12) : (GPIOB->ODR &= ~GPIO_ODR_ODR12))

typedef enum { MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_STOP } MotorState;

void Int_TB6612_Init(void);

void Int_TB6612_SetPWM(int pwma, int pwmb);

#endif /* __INT_TB6612_H__ */
