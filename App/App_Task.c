#include "App_Task.h"

#define START_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE
#define START_TASK_PRIORITY    1

#define DISPLAY_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE
#define DISPLAY_TASK_PRIORITY    1

#define SENSOR_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE
#define SENSOR_TASK_PRIORITY    2

TaskHandle_t startTask;
TaskHandle_t displayTask;
TaskHandle_t sensorTask;

void App_Task_Start(void *pvParmas);
void App_Task_Display(void *pvParmas);
void App_Task_ReadSensor(void *pvParmas);

void FreeRTOS_Start(void) {
    xTaskCreate(App_Task_Start, "startTask", START_TASK_STACK_DEPTH, NULL, START_TASK_PRIORITY, &startTask);
    vTaskStartScheduler();
}

void App_Task_Start(void *pvParmas) {
    portENTER_CRITICAL();
    xTaskCreate(App_Task_Display, "displayTask", DISPLAY_TASK_STACK_DEPTH, NULL, DISPLAY_TASK_PRIORITY, &displayTask);
    xTaskCreate(App_Task_ReadSensor, "sensorTask", SENSOR_TASK_STACK_DEPTH, NULL, SENSOR_TASK_PRIORITY, &sensorTask);
    vTaskDelete(NULL);
    portEXIT_CRITICAL();
}

void App_Task_Display(void *pvParmas) {
    Int_OLED_Clear();
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        App_Car_OLED();
        xTaskDelayUntil(&pxPreviousWakeTime, 50);
    }
}

void App_Task_ReadSensor(void *pvParmas) {
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        App_Car_ReadSensor();
        // 每10ms读取一次传感器数据，因为配置了MPU采样率为100（1s / 100 = 10ms）
        xTaskDelayUntil(&pxPreviousWakeTime, 10);
    }
}

extern void xPortSysTickHandler(void);
void SysTick_Handler(void) {
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}
