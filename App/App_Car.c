#include "App_Car.h"
#include "string.h"
#include <stdio.h>

static int16_t hallLeft;
static int16_t hallRight;
static float kalmanAngle;

static uint8_t cnt      = 0;
static float batVoltage = 0;
static char batInfo[17];
static char hallLeftInfo[17];
static char hallRightInfo[17];
static char angleInfo[17];

void App_Car_OLED(void) {
    if (cnt++ == 20) {
        batVoltage = Dri_ADC1_ReadVoltage();
        cnt        = 0;
    }
    sprintf(batInfo, "Bat: %4.1fV", batVoltage);
    sprintf(hallLeftInfo, "Left: %6d", hallLeft);
    sprintf(hallRightInfo, "Right: %6d", hallRight);
    sprintf(angleInfo, "Angle: %4.1f", kalmanAngle);
    Int_OLED_ShowString(0, 0, (u8 *)batInfo, 16, 1);
    Int_OLED_ShowString(0, 16, (u8 *)hallLeftInfo, 16, 1);
    Int_OLED_ShowString(0, 32, (u8 *)hallRightInfo, 16, 1);
    Int_OLED_ShowString(0, 48, (u8 *)angleInfo, 16, 1);
    Int_OLED_Refresh();
}

void App_Car_ReadSensor(void) {
    kalmanAngle = Int_MPU6050_ReadAngle();
    hallLeft    = Int_HallEncoder_ReadCounter(HALL_LEFT);
    hallRight   = Int_HallEncoder_ReadCounter(HALL_RIGHT);
}
