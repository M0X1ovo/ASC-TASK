#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

// 电机结构体
typedef struct {
    GPIO_TypeDef* IN1_PORT;
    uint16_t IN1_PIN;
    GPIO_TypeDef* IN2_PORT; 
    uint16_t IN2_PIN;
    TIM_TypeDef* PWM_TIM;
    uint16_t PWM_CH;
    TIM_TypeDef* ENCODER_TIM;
    int32_t encoder_count;
    int16_t target_speed;
    int16_t current_speed;
} Motor_TypeDef;

// PID结构体
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float error;
    float last_error;
    float prev_error;
    float integral;
    float output;
} PID_TypeDef;

// 函数声明
void Motor_Init(void);
void Motor_Set_Speed(Motor_TypeDef* motor, int16_t speed);
void Encoder_Init(void);
int32_t Encoder_Read(TIM_TypeDef* TIMx);
void PID_Init(void);
int16_t Incremental_PID(PID_TypeDef* pid, float target, float current);

extern Motor_TypeDef motor1, motor2;
extern PID_TypeDef speed_pid1, speed_pid2, position_pid;

#endif