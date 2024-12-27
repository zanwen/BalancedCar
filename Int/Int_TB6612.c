#include "Int_TB6612.h"
#include "Dri_TIM.h"
#include <stdlib.h>

void Int_TB6612_Init(void) {
    Dri_TIM4_Init();

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH |= (GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE14 | GPIO_CRH_MODE15);
    GPIOB->CRH &= ~(GPIO_CRH_CNF12 | GPIO_CRH_CNF13 | GPIO_CRH_CNF14 | GPIO_CRH_CNF15);

    AIN1(0);
    AIN2(0);
    BIN1(0);
    BIN2(0);
}

static void Int_TB6612_MotorA(MotorState state) {
    switch (state) {
        case MOTOR_FORWARD:
            AIN1(0);
            AIN2(1);
            break;
        case MOTOR_BACKWARD:
            AIN1(1);
            AIN2(0);
            break;
        case MOTOR_STOP:
            AIN1(1);
            AIN2(1);
            break;
    }
}

static void Int_TB6612_MotorB(MotorState state) {
    switch (state) {
        case MOTOR_FORWARD:
            BIN1(0);
            BIN2(1);
            break;
        case MOTOR_BACKWARD:
            BIN1(1);
            BIN2(0);
            break;
        case MOTOR_STOP:
            BIN1(1);
            BIN2(1);
            break;
    }
}

/**
 * @brief 设置电机占空比，0~7199
 *
 * @param pwma 左轮
 * @param pwmb 右轮
 */
void Int_TB6612_SetPWM(int pwma, int pwmb) {
    if (pwma > 0) {
        Int_TB6612_MotorA(MOTOR_FORWARD);
    } else if (pwma < 0) {
        Int_TB6612_MotorA(MOTOR_BACKWARD);
    } else {
        Int_TB6612_MotorA(MOTOR_STOP);
    }
    TIM4->CCR4 = abs(pwma);

    if (pwmb > 0) {
        Int_TB6612_MotorB(MOTOR_FORWARD);
    } else if (pwmb < 0) {
        Int_TB6612_MotorB(MOTOR_BACKWARD);
    } else {
        Int_TB6612_MotorB(MOTOR_STOP);
    }
    TIM4->CCR3 = abs(pwmb);
}
