//
// Created by WANG on 2026/11/9.
//
#include "main.h"
#include "cmsis_os.h"
#include <string.h>

// USART1 接收 DT7 数据
extern UART_HandleTypeDef huart1;

// DBUS 一帧数据18个字节
#define DBUS_BUFFER_SIZE 18
uint8_t dbus_rx_buffer[DBUS_BUFFER_SIZE];

// 遥控器数据结构体
typedef struct {
    int16_t ch[4];   // 4个摇杆通道，中位0，范围 -660 ~ +660
    uint8_t s1;      // 左侧拨杆：上=1，中=3，下=2
    uint8_t s2;      // 右侧拨杆
    uint8_t is_online; // 是否在线
} RC_Data_t;

RC_Data_t rc_data = {0};

// 解析函数
void DBUS_Decode(const uint8_t *buf, RC_Data_t *rc) {
    // 校验和判断：如果校验不通过，说明数据有误，直接返回
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 17; i++) {
        checksum += buf[i];
    }
    if (checksum != buf[17]) {
        rc->is_online = 0; // 数据错误/离线
        return;
    }
    rc->is_online = 1;

    // 提取 11 位通道数据，并减去 1024，将范围映射到 -660 ~ +660
    rc->ch[0] = ((buf[0] | (buf[1] << 8)) & 0x07FF) - 1024;
    rc->ch[1] = (((buf[1] >> 3) | (buf[2] << 5)) & 0x07FF) - 1024;
    rc->ch[2] = (((buf[2] >> 6) | (buf[3] << 2) | (buf[4] << 10)) & 0x07FF) - 1024;
    rc->ch[3] = (((buf[4] >> 1) | (buf[5] << 7)) & 0x07FF) - 1024;

    // 解析拨杆开关 (S1 和 S2)
    // S1 在第 6 个字节 (buf[5]) 的第 4,5 位
    // S2 在第 6 个字节 (buf[5]) 的第 6,7 位
    rc->s1 = (buf[5] >> 4) & 0x03;
    rc->s2 = (buf[5] >> 6) & 0x03;
}

// 任务入口
extern "C" void RC_Task(void *argument)
{
    // 启动串口 DMA 接收
    HAL_UART_Receive_DMA(&huart1, dbus_rx_buffer, DBUS_BUFFER_SIZE);

    for (;;)
    {
        // 由于 DT7 每 14ms 发送一帧，每 10ms 去解析一次
        DBUS_Decode(dbus_rx_buffer, &rc_data);

        osDelay(10);
    }
}