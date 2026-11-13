# FreeRTOS 工程模板阅读笔记

## 一、 之前不熟悉的 C/C++ 功能

1. **类 (Class) **：
    - **定义**：C++ 的类是将“数据（成员变量）”和“操作数据的函数（成员函数）”封装在一起的自定义数据类型。
    - **语法结构**：
      ```cpp
      class 类名 { 访问说明符: 成员声明; };
      2. **函数指针与回调函数 (Callback)**:
2. **函数指针**：
   - **定义**：将函数地址赋给指针变量。
   - **语法结构**：
     ```cpp
     // 1. 直接声明
     void (*pFunc)(int);
     // 2. typedef 格式
     typedef void (*CallbackType)(int data);
     
## 二、 模板中关于 FreeRTOS 的使用
1.`freertos.c` 中创建了 CAN 发送和电机控制等高优先级任务。使用了 `__weak` 弱函数机制，MX 只生成空任务框架，具体的死循环业务写在上层 `.cpp` 中，实现了硬件配置与业务代码的分离。

2.针对 CAN 仅有的 3 个发送邮箱，模板创建了初始值为 3 的计数信号量。发送任务在取队列前先 `Acquire` 获取“车位”，等硬件中断触发发送完成后在回调中 `Release`。完美避免了发送过快导致的邮箱溢出。

3.任务中使用了 `osDelay` 替代裸机的 `HAL_Delay`，触发调度器切换任务，不浪费 CPU 资源。
## 三、 相比自己之前的代码，模板的优势
1. **CAN 接收机制**
   - **我的代码**：初始化时过滤器`FilterMask`全0，所有 CAN 报文全进中断。在 `HAL_CAN_RxFifo0MsgPendingCallback` 里用 `if` 编码判断 `StdId == 0x1FF`。
   - **模板代码**：ID列表模式，只有相匹配的ID才触发中断。中断里通过索引查表找到对象，执行绑定的回调函数（`dev->callback`）。如果加设备，底层中断代码不用改。

2. **CAN 发送机制**
   - **我的代码**：在 `BSP_CAN_SendStd` 里直接调用 `HAL_CAN_AddTxMessage`，如果三个发送邮箱满了（`FreeLevel == 0`）就直接 `return 2` 丢弃数据。
   - **模板代码**：发送数据只是压入 FreeRTOS 消息队列（`osMessageQueuePut`），立刻返回不阻塞,有效杜绝了总线拥堵导致的丢包。

3. **代码架构**
   - **我的代码**：用全局结构体数组 `Motor_Data_t motor[8]` 存数据，写函数通过传指针（`PID_Calc(PID_t *pid, ...)`）。如果电机有变化的话，需要修改很多全局变量。
   - **模板代码**：使用了`class`封装。每个电机都是一个独立的对象，拥有自己的状态和方法。如果要添加新电机，直接实例化一个对象就可以了。
## 四、 如何使用这个模板

1.在 `Task` 文件夹下新建文件。例如新建 `task_led.cpp`，在里面写上逻辑代码。

2.修改根目录 `CMakeLists.txt`。找到 `target_sources`，把 `Task/task_led.cpp` 加进去。

3.在 `Core/Src/freertos.c` 中注册任务。在 `MX_FREERTOS_Init()` 函数中，调用 `osThreadNew(LED_Task, NULL, &LED_Task_attributes)`。

