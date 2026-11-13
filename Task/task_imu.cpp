//
// task_imu.cpp
//
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"

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

/* 对外发布的姿态角，单位：度。其它任务用 extern 声明即可读取 */
float yaw   = 0.0f;
float pitch = 0.0f;
float roll  = 0.0f;

extern "C" void StartIMU_Task(void *argument)
{
    (void)argument;

    /* 传感器和 EKF 已在 MX_FREERTOS_Init() 里初始化完毕，这里只做周期驱动 */
    uint32_t last_cnt = 0;
    DWT_GetDeltaT(&last_cnt);   /* 先空跑一次，把计数器对齐到"现在"，丢掉这段垃圾 dt */

    for (;;)
    {
        float dt = DWT_GetDeltaT(&last_cnt);

        /* dt 合理性保护：首拍、调度抖动、断点调试会给出离谱值，直接跳过当拍 */
        if (dt > 0.0f && dt < 0.02f)
        {
            BMI088_Read(&BMI088);

            IMU_QuaternionEKF_Update(BMI088.Gyro[0], BMI088.Gyro[1], BMI088.Gyro[2],
                                     BMI088.Accel[0], BMI088.Accel[1], BMI088.Accel[2], dt);

            yaw   = QEKF_INS.Yaw;
            pitch = QEKF_INS.Pitch;
            roll  = QEKF_INS.Roll;
        }

        osDelay(1);
    }
}