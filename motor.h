#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

// 电机结构体
typedef struct {
    GPIO_TypeDef* IN1_PORT;  // 控制引脚1的端口，如B12
    uint16_t IN1_PIN;        // 控制引脚1的引脚号
    GPIO_TypeDef* IN2_PORT;  // 控制引脚2的端口，如B13
    uint16_t IN2_PIN;        // 控制引脚2的引脚号
    TIM_TypeDef* PWM_TIM;    // 使用的定时器（PWM生成），如TIM2
    uint16_t PWM_CH;         // 定时器的通道，如TIM_Channel_3
    TIM_TypeDef* ENCODER_TIM; // 编码器定时器，如TIM3
    int32_t encoder_count;   // 编码器计数值
    int16_t target_speed;    // 目标速度
    int16_t current_speed;   // 当前速度
} Motor_TypeDef;

// PID结构体
typedef struct {
    float Kp;               // 比例系数
    float Ki;               // 积分系数
    float Kd;               // 微分系数
    float error;            // 当前误差
    float last_error;       // 上一次误差
    float prev_error;       // 上上次误差（用于增量式PID）
    float integral;         // 积分项
    float output;           // 输出值
} PID_TypeDef;

// 函数声明
void Motor_Init(void);      // 电机初始化
void Motor_Set_Speed(Motor_TypeDef* motor, int16_t speed); // 设置电机速度
void Encoder_Init(void);    // 编码器初始化
int32_t Encoder_Read(TIM_TypeDef* TIMx); // 读取编码器值
void PID_Init(void);        // PID初始化
int16_t Incremental_PID(PID_TypeDef* pid, float target, float current); // 增量式PID计算

extern Motor_TypeDef motor1, motor2;  // 声明两个电机全局变量
extern PID_TypeDef speed_pid1, speed_pid2, position_pid; // 声明PID全局变量

#endif