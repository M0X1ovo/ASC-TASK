#include "stm32f10x.h"                  // Device header
#include "pid.h"
#include "system.h"

void PID_Init(PIDController* pid, float kp, float ki, float kd)
{
    pid->kp = kp=1.5;
    pid->ki = ki=0.1;
    pid->kd = kd=0.08;
    pid->integral = 0;
    pid->last_error = 0;
    pid->output = 0;
}

float PID_Calculate(PIDController* pid, float target, float actual)
{
    float error = target - actual;
    
    // 比例项
    float proportional = pid->kp * error;
    
    // 积分项（带抗积分饱和）
    pid->integral += error;
    if (pid->integral > 1000) pid->integral = 1000;
    if (pid->integral < -1000) pid->integral = -1000;
    float integral = pid->ki * pid->integral;
    
    // 微分项
    float derivative = pid->kd * (error - pid->last_error);
    pid->last_error = error;
    
    // 总和
    pid->output = proportional + integral + derivative;
    
    // 输出限幅
    if (pid->output > 1000) pid->output = 1000;
    if (pid->output < -1000) pid->output = -1000;
    
    return pid->output;
}
