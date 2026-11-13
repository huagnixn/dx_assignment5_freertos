//
// Created by WANG on 2026/11/9.
//
#include "main.h"
#include "cmsis_os.h"
#include "CAN_bsp.h"

// 1. 存放功率计数据
typedef struct {
    float chassis_power; // 底盘功率 (W)
    float buffer_energy; // 缓冲能量 (J)
} PowerData_t;

// 全局变量
PowerData_t g_power_data = {0};

// 2. 回调函数（当收到 0x31 报文时，这个函数会自动执行）
void PowerMeter_Callback(can_device_receive *dev, uint8_t *data) {
    // 解析底盘功率：第2字节(高8位) | 第1字节(低8位)，强转为有符号的 int16_t
    int16_t raw_power = (int16_t)((data[1] << 8) | data[0]);
    // 解析缓冲能量：第4字节(高8位) | 第3字节(低8位)
    int16_t raw_energy = (int16_t)((data[3] << 8) | data[2]);

    // 乘以单位系数 0.1
    g_power_data.chassis_power = raw_power * 0.1f;
    g_power_data.buffer_energy = raw_energy * 0.1f;
}

extern "C" void StartPower_Task(void *argument)
{
    // 3. 实例化接收对象
    // 参数依次为：CAN句柄，ID(0x31)，回调函数
    can_device_receive power_rx(&hcan1, 0x31, PowerMeter_Callback);

    for (;;)
    {
        osDelay(100);
    }
}