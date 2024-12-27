#include "Dri_TIM.h"

void Dri_TIM4_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // ===========GPIO复用推挽 mode=11 cnf=10
    GPIOB->CRH |= GPIO_CRH_MODE8;
    GPIOB->CRH |= GPIO_CRH_CNF8_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF8_0;
    GPIOB->CRH |= GPIO_CRH_MODE9;
    GPIOB->CRH |= GPIO_CRH_CNF9_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF9_0;

    // ===========定时器基本配置
    // 定时器溢出频率 72M / 7200 = 10kHz
    TIM4->PSC = 1 - 1;
    TIM4->ARR = 7200 - 1;
    // 计数方向为向上计数
    TIM4->CR1 &= ~TIM_CR1_DIR;

    // ===========定时器通道3配置
    // 默认占空比为0
    TIM4->CCR3 = 0;
    // 高电平有效
    TIM4->CCER &= ~TIM_CCER_CC3P;
    // 通道输入输出选择：比较输出
    TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
    // 比较输出模式：PWM1 110
    TIM4->CCMR2 |= TIM_CCMR2_OC3M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC3M_1;
    TIM4->CCMR2 &= ~TIM_CCMR2_OC3M_0;
    // 使能通道比较输出
    TIM4->CCER |= TIM_CCER_CC3E;

    // ===========定时器通道4配置
    // 默认占空比为0
    TIM4->CCR4 = 0;
    // 高电平有效
    TIM4->CCER &= ~TIM_CCER_CC4P;
    // 通道输入输出选择：比较输出
    TIM4->CCMR2 &= ~TIM_CCMR2_CC4S;
    // 比较输出模式：PWM1 110
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_1;
    TIM4->CCMR2 &= ~TIM_CCMR2_OC4M_0;
    // 使能通道比较输出
    TIM4->CCER |= TIM_CCER_CC4E;

    /* 使能定时器 */
    TIM4->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief 初始化 TIM2 TIM3 为编码器接口模式
 *
 */
void Dri_TIM2_3_Init(void) {
    RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN);
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN);

    // AFIO重映射
    AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;
    // 关闭JTAG 010
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_0;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_2;

    // ===========GPIO输入 mode=00 cnf=01
    GPIOA->CRH &= ~GPIO_CRH_MODE15;
    GPIOA->CRH &= ~GPIO_CRH_CNF15_1;
    GPIOA->CRH |= GPIO_CRH_CNF15_0;

    GPIOB->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
    GPIOB->CRL &= ~(GPIO_CRL_CNF3_1 | GPIO_CRL_CNF4_1 | GPIO_CRL_CNF5_1);
    GPIOB->CRL |= (GPIO_CRL_CNF3_0 | GPIO_CRL_CNF4_0 | GPIO_CRL_CNF5_0);

    // ===========定时器基本配置
    // 定时器溢出频率 72M / 7200 = 10kHz
    TIM2->PSC = 1 - 1;
    TIM2->ARR = 0xFFFF - 1;

    TIM3->PSC = 1 - 1;
    TIM3->ARR = 0xFFFF - 1;

    // ===========编码器模式配置
    TIM2->CCMR1 |= (TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S_1 | TIM_CCMR1_CC2S_1);
    TIM2->CCER &= ~TIM_CCER_CC1P; // 输入信号不反转，TI1FP1=TI1
    TIM2->CCER &= ~TIM_CCER_CC2P; // 输入信号不反转，TI2FP2=TI2
    TIM2->SMCR |= TIM_SMCR_SMS_0;
    TIM2->SMCR |= TIM_SMCR_SMS_1;
    TIM2->SMCR &= ~TIM_SMCR_SMS_2;

    TIM3->CCMR1 |= (TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);
    TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S_1 | TIM_CCMR1_CC2S_1);
    TIM3->CCER |= TIM_CCER_CC1P;
    TIM3->CCER &= ~TIM_CCER_CC2P;
    TIM3->SMCR |= TIM_SMCR_SMS_0;
    TIM3->SMCR |= TIM_SMCR_SMS_1;
    TIM3->SMCR &= ~TIM_SMCR_SMS_2;

    // 使能计数器
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
}
