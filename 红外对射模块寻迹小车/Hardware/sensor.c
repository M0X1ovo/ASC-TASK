#include "stm32f10x.h"                  // Device header
#include "motor.h"

#define leftfront 1 
#define leftback 2
#define rightfront 3
#define rightback 4
  
void SensorInit()
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//开启AFIO的时钟，外部中断必须开启AFIO的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB14引脚初始化为上拉输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//将PB14引脚初始化为上拉输入
}

void directionjudge()
{
	uint16_t left1=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
	uint16_t left2=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
	uint16_t right1=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13);
	uint16_t right2=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14);
	if(left2==1&&right1==1&&left1==0&&right2==0)
	{
		Motor_SetPWM(1,50);     //直行
		Motor_SetPWM(2,50);
		Motor_SetPWM(3,50);
		Motor_SetPWM(4,50);
	}
	else if(left2==1&&right1==1&&left1==1&&right2==1)  //十字
	{
		Motor_SetPWM(1,50);
		Motor_SetPWM(2,50);
		Motor_SetPWM(3,50);
		Motor_SetPWM(4,50);
	}
	else if(left2==1&&right1==1&&left1==1&&right2==0)   //左直角弯
	{
		Motor_SetPWM(leftfront,10);
		Motor_SetPWM(leftback,10);
		Motor_SetPWM(rightfront,50);
		Motor_SetPWM(rightback,50);
	} 
	else if(left2==1&&right1==1&&left1==0&&right2==1)     //右直角弯  
	{
		Motor_SetPWM(leftfront,50);
		Motor_SetPWM(leftback,50);
		Motor_SetPWM(rightfront,10);
		Motor_SetPWM(rightback,10);
	}
	else if(left2==1&&right1==0&&left1==0&&right2==0)    //直线修正
	{
		Motor_SetPWM(leftfront,55);
		Motor_SetPWM(leftback,55);
		Motor_SetPWM(rightfront,45);
		Motor_SetPWM(rightback,45);
	}
	else if(left2==0&&right1==1&&left1==0&&right2==0)
	{
		Motor_SetPWM(leftfront,55);
		Motor_SetPWM(leftback,55);
		Motor_SetPWM(rightfront,45);
		Motor_SetPWM(rightback,45);
	}
	else if(left2==0&&right1==0&&left1==1&&right2==0)
	{
		Motor_SetPWM(leftfront,10);
		Motor_SetPWM(leftback,10);
		Motor_SetPWM(rightfront,50);
		Motor_SetPWM(rightback,50);		
	}
	else if(left2==0&&right1==0&&left1==0&&right2==1)
	{
		Motor_SetPWM(leftfront,50);
		Motor_SetPWM(leftback,50);
		Motor_SetPWM(rightfront,10);
		Motor_SetPWM(rightback,10);		
	}			
}


 

