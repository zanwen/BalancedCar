#include "App_Car.h"
#include "Com_KalmanFilter.h"
#include "Com_PID.h"
#include "Dri_UART1.h"
#include "Dri_UART2.h"
#include "FreeRTOS.h"
#include "Int_TB6612.h"
#include "logger.h"
#include "queue.h"
#include "string.h"
#include "task.h"
#include <math.h>
#include <stdio.h>

#define PI 3.1415

// 临界值 * 0.6(工程经验系数，使得更平滑)
#define PID_BALANCE_KP             -540.0f // 低频振荡最大临界值 -900.0f
#define PID_BALANCE_KD             -7.8f   // 高频振荡最小临界值 -13.0f
#define PID_BALANCE_EXPECTED_VALUE 0.5f

#define PID_VELOCITY_KP             150.0f
#define PID_VELOCITY_KI             0.75f // 系数经验值 1/200
#define PID_VELOCITY_EXPECTED_VALUE 0.0f

#define PID_TURN_KP -10.0f

typedef enum { UP, DOWN, LEFT, RIGHT, STOP } ControlCmd;

static int16_t hallLeft;
static int16_t hallRight;
static float kalmanAngle;
static float angularVelocityY;
static float angularVelocityZ;

static uint8_t cnt      = 0;
static float batVoltage = 0;
static char batInfo[17];
static char hallLeftInfo[17];
static char hallRightInfo[17];
static char angleInfo[17];

extern float angle;

ControlCmd controlCmd = STOP;

void App_Car_Init(void) {
    Dri_UART1_Init();
    Dri_UART2_Init();
    Dri_ADC1_Init();

    Int_OLED_Init();
    Int_TB6612_Init();
    Int_HallEncoder_Init();
    Int_MPU6050_Init();

    LOG_DEBUG("App_Car_Init done");
}

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
    static int16_t accelX, accelY, accelZ;
    static int16_t gyroX, gyroY, gyroZ;

    Int_MPU6050_ReadAccel(&accelX, &accelY, &accelZ);
    Int_MPU6050_ReadGyro(&gyroX, &gyroY, &gyroZ);
    // 倾角tanθ=accelX/accelY, atan2()获取arctan(accelX/accelY)=θ，弧度转角度θ*180/Π
    // 通过重力在x轴和y轴上的加速度分量来计算小车倾角
    float accelAngle = (float)(atan2(accelX, accelZ) * 180 / PI);
    // y轴角速度换算 [-32768, 32767]  => [-2000°/s, 2000°/s] 比例尺为16.4
    angularVelocityY = (float)(-gyroY / 16.4);
    Com_KalmanFilter(accelAngle, angularVelocityY);

    kalmanAngle      = angle;
    angularVelocityZ = (float)(-gyroZ / 16.4);

    hallLeft  = Int_HallEncoder_ReadCounter(HALL_LEFT);
    hallRight = Int_HallEncoder_ReadCounter(HALL_RIGHT);
}

void App_Car_Control(void) {
    static int pwma, pwmb;
    static int balanceOut = 0, velocityOut = 0, turnOut = 0;
    static int cmdDisplacement = 0, cmdTurn = 0;
    balanceOut = Com_PID_Balance(PID_BALANCE_KP, PID_BALANCE_KD, PID_BALANCE_EXPECTED_VALUE,
                                 kalmanAngle, angularVelocityY);
    // 根据蓝牙命令调整小车需要移动的位移
    if (controlCmd == UP) {
        cmdDisplacement = 50;
    } else if (controlCmd == DOWN) {
        cmdDisplacement = -50;
    } else if (controlCmd == STOP) {
        cmdDisplacement = 0;
    }
    velocityOut = Com_PID_Velocity(PID_VELOCITY_KP, PID_VELOCITY_KI, PID_VELOCITY_EXPECTED_VALUE,
                                   hallLeft + hallRight, cmdDisplacement);

    // 根据蓝牙命令调整小车转向
    if (controlCmd == LEFT) {
        cmdTurn -= 10;
        if (cmdTurn <= -500)
            cmdTurn = -500; // 累积限幅
    } else if (controlCmd == RIGHT) {
        cmdTurn += 10;
        if (cmdTurn >= 500)
            cmdTurn = 500; // 累积限幅
    } else {
        cmdTurn = 0;
        turnOut = Com_PID_Turn(PID_TURN_KP, angularVelocityZ);
    }
    pwma = balanceOut + velocityOut + turnOut + cmdTurn;
    pwmb = balanceOut + velocityOut - turnOut - cmdTurn;
    Int_TB6612_SetPWM(pwma, pwmb);
}

// 处理从PC收到的数据
void Dri_UART1_ReceiveCallback(uint8_t *buf, uint8_t size) {
    // 转发给蓝牙
    Dri_UART2_SendBytes(buf, size);
    LOG_DEBUG("Send to Bluetooth: %s", buf);
}

// 处理从蓝牙收到的数据
void Dri_UART2_ReceiveCallback(uint8_t *buf, uint8_t size) {
    // 按键A 按下：DD  77  00  00  01  00  00  00
    // 按键B 按下：DD  77  00  00  00  01  00  00
    // 按键C 按下：DD  77  00  00  00  00  01  00
    // 按键D 按下：DD  77  00  00  00  00  00  01
    //      松开：DD  77  00  00  00  00  00  00
    if (size > 2 && buf[0] == 0xDD && buf[1] == 0x77) {
        // 通过蓝牙接收到的小车控制数据包
        if (buf[4] == 0x01) {
            controlCmd = UP;
        } else if (buf[5] == 0x01) {
            controlCmd = DOWN;
        } else if (buf[6] == 0x01) {
            controlCmd = LEFT;
        } else if (buf[7] == 0x01) {
            controlCmd = RIGHT;
        } else {
            controlCmd = STOP;
        }
        LOG_DEBUG("Received from Bluetooth, controlCmd = %d", controlCmd);
    } else {
        LOG_DEBUG("Received from Bluetooth: %s", buf)
    }
}
