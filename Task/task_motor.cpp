//
// task_motor.cpp
//
#include "main.h"
#include "cmsis_os.h"
#include <math.h>
#include "DJI_motor_driver.h"

#define PI 3.1415926535f

extern "C" void Motor_Task(void *argument)
{
    // 1. 实例化 3508 电机 (接在 CAN1，ID 为 1)
    DJI_motor motor_3508(&hcan1, nullptr, 1, DJI_M3508, 0);

    // 2. PID参数
    motor_3508.posPid.param.p = 0.8f;
    motor_3508.posPid.param.max_out = 0.5f;

    // 3. 正弦波参数设定
    float time_s = 0;
    const float dt = 0.001f;       // 1ms 周期
    const float amplitude = 10.0f; // 振幅：正负 10 圈
    const float frequency = 0.5f;  // 频率：0.5Hz

    for (;;)
    {
        // 4. 计算当前时刻的正弦目标位置
        float target_rounds = amplitude * sinf(2.0f * PI * frequency * time_s);

        // 5. 位置环控制
        motor_3508.motor_set_rounds(target_rounds);

        // 6. 时间累加与系统延时
        time_s += dt;
        osDelay(1);
    }
}