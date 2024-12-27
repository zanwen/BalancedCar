#include "App_Task.h"
#include "Com_Delay.h"
#include "Dri_ADC1.h"
#include "Dri_UART1.h"
#include "Int_HallEncoder.h"
#include "Int_MPU6050.h"
#include "Int_OLED.h"
#include "Int_TB6612.h"
#include "freertos.h"
#include "logger.h"
#include "task.h"

int main() {
    NVIC_SetPriorityGrouping(3);
    Dri_UART1_Init();
    Dri_ADC1_Init();
    
    Int_OLED_Init();
    Int_TB6612_Init();
    // Int_TB6612_SetPWM(0, 3600);
    Int_HallEncoder_Init();
    Int_MPU6050_Init();
    
    LOG_DEBUG("main started, %d, %d", sizeof(int), sizeof(long));

    FreeRTOS_Start();

    while (1) {
        // Int_HallEncoder_Test();
        // Int_MPU6050_ReadAngle();
        //  Dri_ADC1_ReadVoltage();
        //  for_delay_ms(1000);
    }
}
