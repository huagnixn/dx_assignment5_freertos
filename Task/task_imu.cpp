//
// task_imu.cpp
//
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"

extern "C" {
#include "bsp_dwt.h"
#include "BMI088driver.h"
#include "QuaternionEKF.h"
}
// 声明外部变量
extern float yaw, pitch, roll;

extern "C" void IMU_Task(void *argument)
{
    // 1. 初始化 DWT 计时器 (168MHz主频)
    DWT_Init(168);

    // 2. 初始化 BMI088 传感器
    // 传入 SPI1 句柄
    BMI088_Init(&hspi1, 1);

    // 3. 初始化四元数 EKF
    IMU_QuaternionEKF_Init(10, 0.001f, 1000000, 0.9996f, 0.001f);

    uint32_t last_time = DWT_GetTimeline_ms(); // 获取初始时间

    for (;;)
    {
        // 4. 计算 dt（单位：秒）
        // 去 bsp_dwt.h 看真实的函数名，通常是 DWT_GetDeltaT
        float dt = DWT_GetDeltaT(&last_time);

        // 5. 读取 BMI088 数据
        BMI088_Read(&BMI088);

        // 6. 更新姿态
        IMU_QuaternionEKF_Update(BMI088.Gyro[0], BMI088.Gyro[1], BMI088.Gyro[2],
                                 BMI088.Accel[0], BMI088.Accel[1], BMI088.Accel[2], dt);

        // 7. 运行周期 1ms
        osDelay(1);
    }
}