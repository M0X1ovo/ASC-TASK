#include "motor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

Motor_TypeDef motor1, motor2;
PID_TypeDef speed_pid1, speed_pid2, position_pid;

// 电机初始化
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
    
    // 初始化电机1控制引脚（B12、B13）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_Pp;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化电机2控制引脚（B14、B15）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化PWM引脚（A2、A3）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置TIM2为PWM输出，频率1kHz
    TIM_TimeBaseStructure.TIM_Period = 999;           // 自动重装载值，1000-1
    TIM_TimeBaseStructure.TIM_Prescaler = 71;         // 预分频器，72MHz/(71+1)=1MHz，1MHz/1000=1kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // PWM通道配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 初始化通道3（A2）和通道4（A3）
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比为0
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    
    // 使能预装载
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    TIM_ARRPreloadConfig(TIM2, ENABLE); // 使能自动重装载预装载
    TIM_Cmd(TIM2, ENABLE);              // 启动TIM2
    
    // 初始化电机结构体
    motor1.IN1_PORT = GPIOB; motor1.IN1_PIN = GPIO_Pin_12;
    motor1.IN2_PORT = GPIOB; motor1.IN2_PIN = GPIO_Pin_13;
    motor1.PWM_TIM = TIM2; motor1.PWM_CH = TIM_Channel_3; // 使用TIM2的通道3
    
    motor2.IN1_PORT = GPIOB; motor2.IN1_PIN = GPIO_Pin_14;
    motor2.IN2_PORT = GPIOB; motor2.IN2_PIN = GPIO_Pin_15;  
    motor2.PWM_TIM = TIM2; motor2.PWM_CH = TIM_Channel_4; // 使用TIM2的通道4
}

// 设置电机速度
void Motor_Set_Speed(Motor_TypeDef* motor, int16_t speed)
{
    // 限制速度范围，PWM占空比最大为1000（因为Period=999）
    if(speed > 1000) speed = 1000;
    if(speed < -1000) speed = -1000;
    
    // 设置方向
    if(speed > 0) {
        // 正转：IN1=1, IN2=0
        GPIO_SetBits(motor->IN1_PORT, motor->IN1_PIN);
        GPIO_ResetBits(motor->IN2_PORT, motor->IN2_PIN);
    } else if(speed < 0) {
        // 反转：IN1=0, IN2=1
        GPIO_ResetBits(motor->IN1_PORT, motor->IN1_PIN);
        GPIO_SetBits(motor->IN2_PORT, motor->IN2_PIN);
        speed = -speed; // 取绝对值，因为PWM为正数
    } else {
        // 停止：IN1=0, IN2=0
        GPIO_ResetBits(motor->IN1_PORT, motor->IN1_PIN);
        GPIO_ResetBits(motor->IN2_PORT, motor->IN2_PIN);
    }
    
    // 设置PWM占空比
    if(motor->PWM_CH == TIM_Channel_3) {
        TIM_SetCompare3(motor->PWM_TIM, speed);
    } else if(motor->PWM_CH == TIM_Channel_4) {
        TIM_SetCompare4(motor->PWM_TIM, speed);
    }
}