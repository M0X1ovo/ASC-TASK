#include "uart.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "string.h"
#include "stdio.h"

char uart_rx_buffer[100];
uint8_t uart_rx_index = 0;

// 串口初始化
void UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
    
    // 配置TX引脚 (A9) 为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置RX引脚 (A10) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 串口配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    // 使能串口接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART1, ENABLE); // 使能串口
}

// 发送字符串
void UART_SendString(char* str)
{
    while(*str) {
        USART_SendData(USART1, *str++);        // 发送一个字符
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // 等待发送完成
    }
}

// 发送速度数据到上位机（VOFA+）
void UART_Send_Speed_Data(void)
{
    char buffer[50];
    // 格式化字符串：电机1实际速度,电机1目标速度,电机2实际速度
    sprintf(buffer, "%.2f,%.2f,%.2f\r\n", 
            (float)motor1.current_speed, 
            (float)motor1.target_speed,
            (float)motor2.current_speed);
    UART_SendString(buffer);
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        char received_char = USART_ReceiveData(USART1); // 读取接收到的字符
        
        // 如果接收到回车或换行，则解析命令
        if(received_char == '\r' || received_char == '\n') {
            uart_rx_buffer[uart_rx_index] = '\0'; // 字符串结束符
            UART_Command_Parser(uart_rx_buffer);  // 解析命令
            uart_rx_index = 0; // 重置索引
        } else if(uart_rx_index < sizeof(uart_rx_buffer)-1) {
            uart_rx_buffer[uart_rx_index++] = received_char; // 存储字符
        }
    }
}

// 命令解析
void UART_Command_Parser(char* data)
{
    int speed_value;
    // 解析格式：@speed% [速度值]
    if(sscanf(data, "@speed%% %d", &speed_value) == 1) {
        motor1.target_speed = speed_value; // 设置电机1目标速度
    }
}