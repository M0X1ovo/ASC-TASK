#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"
#include "Encoder.h"
#include "Motor.h"
#include "Serial.h"
#include "vofa.h"
#include <stdlib.h>
#include <string.h>
#include "OLED_Data.h"
#include "sensor.h"
#include "PWM.h"

/*------------------- 全局变量定义 -------------------*/
volatile uint8_t Current_Mode = 0; // 0: 速度环, 1: 位置环 (添加volatile)
uint8_t KeyNum;

// --- 新的、独立的串口接收系统 ---
#define UART_RX_BUFFER_SIZE 32
volatile char UART_RxBuffer_New[UART_RX_BUFFER_SIZE];
volatile uint8_t UART_RxIndex_New = 0;
volatile uint8_t UART_RxFlag_New = 0; // 接收完成标志

// 编码器
volatile int16_t Speed1 = 0; // (添加volatile)
int32_t Encoder1_Count = 0;

// --- 任务一：速度环PID变量 ---
volatile float Target_Speed = 0;  // 目标速度 (添加volatile)
volatile float Actual_Speed = 0;  // 实际速度 (添加volatile)
volatile float PID_Out_Speed = 0; // 速度环PID输出 (添加volatile)

// PID参数
float Kp_speed = 0.5f;
float Ki_speed = 0.1f;
float Kd_speed = 0.05f;

float Error_Speed_Now = 0;
float Error_Speed_Last = 0;

/*------------------- 函数声明 -------------------*/
void Process_UART_Command_Stable(char* cmd);
void Send_Data_To_PC(void);

/**
  * @brief  主函数
  */
int main(void)
{
    /*// 模块初始化
    OLED_Init();
    Key_Init();
    Timer_Init();
    Encoder1_Init();
    Motor_Init();
    Serial_Init(); // 只用它来初始化USART硬件，中断由我们自己的函数接管
    
    while (1)
    {
        // 1. 按键检测与模式切换
        KeyNum = Key_GetNum();
        if (KeyNum == 1)
        {
            Current_Mode = !Current_Mode;
            Target_Speed = 0;
            PID_Out_Speed = 0;
        }
        
        // 2. 【新的】处理串口命令
        if (UART_RxFlag_New)
        {
            if (Current_Mode == 0)
            {
                Process_UART_Command_Stable((char*)UART_RxBuffer_New);
            }
            UART_RxFlag_New = 0; // 清除标志位
            
            // 手动清空缓冲区
            for (uint16_t i = 0; i < UART_RX_BUFFER_SIZE; i++) {
                UART_RxBuffer_New[i] = 0;
            }
        }
        
        // 3. 更新OLED显示
        OLED_Clear();
        OLED_ShowString(1, 0, "Mode:");
        if (Current_Mode == 0) {
            OLED_ShowString(41, 0, "Speed");
        } else {
            OLED_ShowString(41, 0, "Position");
        }
        OLED_ShowString(1, 16, "T:");
        OLED_ShowSignedNum(17, 16, (int16_t)Target_Speed, 4);
        OLED_ShowString(65, 16, "A:");
        OLED_ShowSignedNum(81, 16, Speed1, 4);
        
        
        // 4. 定时发送数据到VOFA+
        Send_Data_To_PC();
        
        Delay_ms(10);
    }
	OLED_Init();
	OLED_Clear();
	SensorInit();*/
	while(1)
	{
		int state=Key_GetNum();
		if(state)
		{
			TIM_Cmd(TIM2, ENABLE);
		}
		else
		{
			PWM_Stop();
		}
		/*OLED_ShowString(1,1,"OLED OK");
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))
		{
			OLED_Clear();
			OLED_ShowString(1,1,"C13 ok high");
		}
		else if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14))
		{
			OLED_Clear();
			OLED_ShowString(1,1,"C14 ok low");
		}
		else if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15))
		{
			OLED_Clear();
			OLED_ShowString(1,1,"C15 ok high");
		}
		else if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12))
		{
			OLED_Clear();
			OLED_ShowString(1,1,"B12 ok low");
		}
		Delay_ms(1000);
	}*/
	}
}
/**
  * @brief  【新的】我们自己的串口中断服务函数，覆盖Serial.c里的
  */
/*void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t rx_data = USART_ReceiveData(USART1);
        
        if (rx_data == '\r') // 检测到回车
        {
            UART_RxBuffer_New[UART_RxIndex_New] = '\0'; // 添加字符串结束符
            UART_RxFlag_New = 1; // 设置标志位，通知主循环处理
            UART_RxIndex_New = 0; // 重置索引
        }
        else if (rx_data != '\n') // 忽略换行符
        {
            UART_RxBuffer_New[UART_RxIndex_New] = rx_data;
            UART_RxIndex_New++;
            if (UART_RxIndex_New >= UART_RX_BUFFER_SIZE) // 防止溢出
            {
                UART_RxIndex_New = 0;
            }
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}*/

/**
  * @brief  定时器1中断服务函数 (核心控制循环, 10ms执行一次)
  */
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		directionjudge();
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


