#include "pid.h"
#include "math.h"

// PID初始化
void PID_Init(void)
{
    // 速度环PID参数
    speed_pid1.Kp = 0.8f;
    speed_pid1.Ki = 0.2f; 
    speed_pid1.Kd = 0.05f;
    speed_pid1.integral = 0;
    speed_pid1.last_error = 0;
    speed_pid1.prev_error = 0;
    
    speed_pid2 = speed_pid1; // 电机2使用相同参数
    
    // 位置环PID参数
    position_pid.Kp = 1.5f;
    position_pid.Ki = 0.1f;
    position_pid.Kd = 0.02f;
    position_pid.integral = 0;
    position_pid.last_error = 0;
    position_pid.prev_error = 0;
}

// 增量式PID
int16_t Incremental_PID(PID_TypeDef* pid, float target, float current)
{
    pid->last_error = pid->error;          // 保存上一次误差
    pid->error = target - current;         // 计算当前误差
    
    // 增量式PID公式：
    // Δu = Kp*(e(k)-e(k-1)) + Ki*e(k) + Kd*(e(k)-2e(k-1)+e(k-2))
    float increment = pid->Kp * (pid->error - pid->last_error) 
                    + pid->Ki * pid->error
                    + pid->Kd * (pid->error - 2*pid->last_error + pid->prev_error);
    
    pid->prev_error = pid->last_error;     // 保存上上次误差
    pid->output += increment;              // 累加输出
    
    // 输出限幅，防止积分饱和
    if(pid->output > 1000) pid->output = 1000;
    if(pid->output < -1000) pid->output = -1000;
    
    return (int16_t)pid->output;
}