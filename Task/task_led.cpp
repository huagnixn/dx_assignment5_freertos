//
// Created by WANG on 2026/11/8.
//
#include "main.h"
#include "cmsis_os.h"

//  extern "C"，让 C 语言写的 freertos.c 能找到这个函数
extern "C" void LED_Task(void *argument)
{
    // 初始化：先把红、绿、蓝三个灯全部熄灭（低电平熄灭）
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);

    for (;;)
    {
        // 1. 点亮红灯 (PH12 高电平)，熄灭蓝灯和绿灯 (低电平)
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
        osDelay(300); // 延时 300ms

        // 2. 点亮绿灯 (PH11 高电平)，熄灭蓝灯和红灯 (低电平)
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10 | GPIO_PIN_12, GPIO_PIN_RESET);
        osDelay(300);

        // 3. 点亮蓝灯 (PH10 高电平)，熄灭红灯和绿灯 (低电平)
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);
        osDelay(300);
    }
}