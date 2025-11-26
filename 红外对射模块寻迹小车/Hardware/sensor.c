#include "stm32f10x.h"                  
#include "motor.h"
#include "sensor.h"

#define leftfront 1 
#define leftback 2
#define rightfront 3
#define rightback 4

// === 在这里定义全局变量（只此一处）===
SensorFilter_t sensor_left1, sensor_left2, sensor_right1, sensor_right2;


void SensorInit()
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//开启AFIO的时钟，外部中断必须开启AFIO的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//初始化为下拉输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//初始化为下拉输入
}

void filter_init(SensorFilter_t* filter)
{
    for(int i=0; i<FILTER_SAMPLES; i++) 
	{
        filter->history[i] = 0;  // 清空历史数据
    }
    filter->index = 0;           // 从第一个位置开始写入
}

void sensor_filters_init(void) 
{
    filter_init(&sensor_left1);
    filter_init(&sensor_left2);
    filter_init(&sensor_right1);
    filter_init(&sensor_right2);
}

uint16_t sensor_median_filter(SensorFilter_t* filter, uint16_t raw_value) 
{
    // 步骤1: 更新历史数据
    filter->history[filter->index] = raw_value;
    filter->index = (filter->index + 1) % FILTER_SAMPLES;
    
    // 步骤2: 复制数据用于排序
    uint16_t temp[FILTER_SAMPLES];
    for(int i=0; i<FILTER_SAMPLES; i++) {
        temp[i] = filter->history[i];
    }
    
    // 步骤3: 冒泡排序
    for(int i=0; i<FILTER_SAMPLES-1; i++) {
        for(int j=0; j<FILTER_SAMPLES-1-i; j++) {
            if(temp[j] > temp[j+1]) {
                uint16_t tmp = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = tmp;
            }
        }
    }
    
    // 步骤4: 返回中间值
    return temp[FILTER_SAMPLES/2];
}


void directionjudge()
{
	
    uint16_t left1 = sensor_median_filter(&sensor_left1, !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12));
    uint16_t left2 = sensor_median_filter(&sensor_left2, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13));
    uint16_t right1 = sensor_median_filter(&sensor_right1, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14));
    uint16_t right2 = sensor_median_filter(&sensor_right2, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15));
	
	if(left2==1&&right1==1&&left1==0&&right2==0)
	{
		Motor_SetPWM(1,80);     //直行
		Motor_SetPWM(2,80);
		Motor_SetPWM(3,80);
		Motor_SetPWM(4,80);
	}
	else if(left2==1&&right1==1&&left1==1&&right2==1)  //十字
	{
		Motor_SetPWM(1,80);
		Motor_SetPWM(2,80);
		Motor_SetPWM(3,80);
		Motor_SetPWM(4,80);
	}
	else if(left2==1&&right1==1&&left1==1&&right2==0)   //左直角弯
	{
		Motor_SetPWM(leftfront,10);
		Motor_SetPWM(leftback,10);
		Motor_SetPWM(rightfront,70);
		Motor_SetPWM(rightback,70);
	} 
	else if(left2==1&&right1==1&&left1==0&&right2==1)     //右直角弯  
	{
		Motor_SetPWM(leftfront,70);
		Motor_SetPWM(leftback,70);
		Motor_SetPWM(rightfront,10);
		Motor_SetPWM(rightback,10);
	}
	else if(left2==1&&right1==0&&left1==0&&right2==0)    //直线修正
	{
		Motor_SetPWM(leftfront,75);
		Motor_SetPWM(leftback,75);
		Motor_SetPWM(rightfront,65);
		Motor_SetPWM(rightback,65);
	}
	else if(left2==0&&right1==1&&left1==0&&right2==0)   //直线修正
	{
		Motor_SetPWM(leftfront,75);
		Motor_SetPWM(leftback,75);
		Motor_SetPWM(rightfront,65);
		Motor_SetPWM(rightback,65);
	}
	else if(left2==0&&right1==0&&left1==1&&right2==0)    //极左修正    
	{
		Motor_SetPWM(leftfront,10);
		Motor_SetPWM(leftback,10);
		Motor_SetPWM(rightfront,70);
		Motor_SetPWM(rightback,70);		
	}
	else if(left2==0&&right1==0&&left1==0&&right2==1)    //极右
	{
		Motor_SetPWM(leftfront,70);
		Motor_SetPWM(leftback,70);
		Motor_SetPWM(rightfront,10);
		Motor_SetPWM(rightback,10);		
	}
	else if(left2==0&&right1==1&&left1==0&&right2==1)    //右边俩黑
	{
		Motor_SetPWM(leftfront,40);
		Motor_SetPWM(leftback,40);
		Motor_SetPWM(rightfront,70);
		Motor_SetPWM(rightback,70);		
	}
	else if(left2==1&&right1==0&&left1==1&&right2==0)  //左边俩黑
	{
		Motor_SetPWM(leftfront,70);
		Motor_SetPWM(leftback,70);
		Motor_SetPWM(rightfront,40);
		Motor_SetPWM(rightback,40);		
	}		
}


 

