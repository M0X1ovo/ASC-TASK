#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PWM1 - PA2 (TIM2_CH3)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PWM2 - PA3 (TIM2_CH4)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_InternalClockConfig(TIM2);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);  // PA2
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);  // PA3
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 1. 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

    // 2. 配置GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;  // PA9, PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 配置时基单元
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // 4. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; // 互补输出禁用
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲状态

    // 通道2配置
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);         // 使能预装载

    // 通道3配置
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // 5. 启动定时器
    TIM_ARRPreloadConfig(TIM1, ENABLE);                       // 使能ARR预装载
    TIM_CtrlPWMOutputs(TIM1, ENABLE);                         // 高级定时器必须使能主输出
    TIM_Cmd(TIM1, ENABLE);                                    // 启动定时器
	
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare3(TIM2, Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
    TIM_SetCompare4(TIM2, Compare);
}
