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
#include "Dri_UART2.h"

int main() {
    NVIC_SetPriorityGrouping(3);
    FreeRTOS_Start();

    while (1) {
        // Int_HallEncoder_Test();
        // Int_MPU6050_ReadAngle();
        //  Dri_ADC1_ReadVoltage();
        //  for_delay_ms(1000);
    }
}
