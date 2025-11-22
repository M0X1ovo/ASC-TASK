#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11|GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    PWM_Init();
}

/**
 * 函数名：Motor_SetPWM
 * 功  能：设置指定电机的PWM值和方向
 * 参  数：motor - 电机编号(1或2)
 *        PWM   - PWM值(-100~100，负值表示反转)
 * 说  明：通过控制方向引脚和PWM输出来控制电机转速和方向
 */
void Motor_SetPWM(uint8_t motor, int8_t PWM)
{
    if (motor == 1)  // 控制电机左前
    {
        if (PWM >= 0)  // 正转
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_4);   // AIN1 = 0
            GPIO_SetBits(GPIOB, GPIO_Pin_3);     // AIN2 = 1
            PWM_SetCompareleftfront(PWM);               // 设置PWM占空比
        }
        else  // 反转
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_3);     // AIN1 = 1
            GPIO_ResetBits(GPIOB, GPIO_Pin_4);   // AIN2 = 0
            PWM_SetCompareleftfront(-PWM);                // PWM值为正数
        }
    }
    else if (motor == 2)  // 控制电机左后
    {
        if (PWM >= 0)  // 正转
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_6);   // BIN1 = 0
            GPIO_SetBits(GPIOB, GPIO_Pin_5);     // BIN2 = 1
            PWM_SetCompareleftback(PWM);                 // 使用PWM通道2
        }
        else  // 反转
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_5);     // BIN1 = 1
            GPIO_ResetBits(GPIOB, GPIO_Pin_6);   // BIN2 = 0
            PWM_SetCompareleftback(-PWM);               // PWM值为正数
        }
    }
	    else if (motor == 3)  // 控制电机右前
    {
        if (PWM >= 0)  // 正转
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_11);   // BIN1 = 0
            GPIO_SetBits(GPIOB, GPIO_Pin_10);     // BIN2 = 1
            PWM_SetComparerightfront(PWM);                // 使用PWM通道2
        }
        else  // 反转
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_10);     // BIN1 = 1
            GPIO_ResetBits(GPIOB, GPIO_Pin_11);   // BIN2 = 0
            PWM_SetComparerightfront(-PWM);                // PWM值为正数
        }
    }
	    else if (motor == 4)  // 控制电机右后
    {
        if (PWM >= 0)  // 正转
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_10);   // BIN1 = 0
            GPIO_SetBits(GPIOA, GPIO_Pin_9);     // BIN2 = 1
            PWM_SetComparerightback(PWM);                // 使用PWM通道2
        }
        else  // 反转
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_9);     // BIN1 = 1
            GPIO_ResetBits(GPIOA, GPIO_Pin_10);   // BIN2 = 0
            PWM_SetComparerightback(-PWM);                 // PWM值为正数
        }
    }
}
