#include "Int_HallEncoder.h"
#include "Dri_TIM.h"
#include "logger.h"
#include "Com_Delay.h"

void Int_HallEncoder_Init(void) { Dri_TIM2_3_Init(); }

/**
 * @brief 获取霍尔编码器对应的计数值（霍尔编码器对接TIM编码器接口模式）
 * 正数表示正转（前进）、负数表示反转（后退）
 *
 * @param hallSensor
 * @return int16_t
 */
int16_t Int_HallEncoder_ReadCounter(HallSensor hallSensor) {
    int16_t value = 0;
    switch (hallSensor) {
        case HALL_LEFT:
            value     = (int16_t)TIM2->CNT;
            TIM2->CNT = 0;
            break;
        case HALL_RIGHT:
            value     = (int16_t)TIM3->CNT;
            TIM3->CNT = 0;
            break;
    }
    return value;
}

void Int_HallEncoder_Test(void) {
    LOG_DEBUG("left: %d, right: %d", Int_HallEncoder_ReadCounter(HALL_LEFT),
              Int_HallEncoder_ReadCounter(HALL_RIGHT));
    for_delay_ms(1000);
}
