#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "stm32f10x.h"

// 系统模式定义
typedef enum {
    MODE_SPEED_CONTROL = 0,  // 速度控制模式
    MODE_FOLLOW_CONTROL      // 跟随控制模式
} SystemMode;

// 系统状态
extern SystemMode current_mode;

#endif
