#include "key.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 按键引脚A0配置为上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t KEY_Scan(void)
{
    static uint8_t key_up = 1; // 按键松开标志
    if(key_up && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)) { // 按键按下
        delay_ms(10); // 消抖
        key_up = 0;
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) {
            return 1; // 返回按键按下信号
        }
    } else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
        key_up = 1; // 按键已松开
    }
    return 0;
}