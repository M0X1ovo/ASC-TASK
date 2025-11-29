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

// === 添加PWM限制函数 ===
int limit_pwm(int pwm) {
	if(pwm < 10) return 10;
	if(pwm > 100) return 100;
	return pwm;
}

void directionjudge()
{
	static uint8_t index=0;
	static uint8_t lastindex=0;
	static uint8_t llastindex=0;
	
    // 使用静态数组保存历史数据
    static uint16_t left1[3] = {0,0,0};
    static uint16_t left2[3] = {0,0,0};
    static uint16_t right1[3] = {0,0,0};
    static uint16_t right2[3] = {0,0,0};
	
	static uint8_t proportion=0;
	
	    // === 新增：数据存储降频 ===
    static uint8_t data_store_counter = 0;
    data_store_counter++;

	
    // 每次中断都读取当前传感器数据（保证实时性）
    uint16_t current_left1 = sensor_median_filter(&sensor_left1, !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12));
    uint16_t current_left2 = sensor_median_filter(&sensor_left2, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13));
    uint16_t current_right1 = sensor_median_filter(&sensor_right1, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14));
    uint16_t current_right2 = sensor_median_filter(&sensor_right2, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15));
	
	
    // === 关键：每5次中断存储一次数据 ===
    if(data_store_counter >= 160) 
		{
			data_store_counter = 0;
			
			// 存储数据到历史数组
			left1[index] = current_left1;
			left2[index] = current_left2;
			right1[index] = current_right1;
			right2[index] = current_right2;
			
			if(left1[index]==left1[lastindex]&&left2[index]==left2[lastindex]&&right1[index]==right1[lastindex]&&right2[index]==right2[lastindex]&&left1[index]==left1[llastindex]&&left2[index]==left2[llastindex]&&right1[index]==right1[llastindex]&&right2[index]==right2[llastindex])
			{
				if(proportion<5)
				{
					proportion+=2;
				}
				

			}
			else if(left1[index]==left1[lastindex]&&left2[index]==left2[lastindex]&&right1[index]==right1[lastindex]&&right2[index]==right2[lastindex])
			{
				if(proportion<4)
				{
					proportion+=1;
				}
				
			}
			else
			{
				proportion=0;
			}
			// 更新索引
			llastindex=lastindex;
			lastindex=index;
			index = (index + 1) % 3;		
		}

	if(current_left2==1&&current_right1==1&&current_left1==0&&current_right2==0)
	{
		Motor_SetPWM(1,68+proportion*6);     //直行
		Motor_SetPWM(2,68+proportion*6);
		Motor_SetPWM(3,68+proportion*6);
		Motor_SetPWM(4,68+proportion*6);
	}
	else if(current_left2==1&&current_right1==1&&current_left1==1&&current_right2==1)  //十字
	{
		Motor_SetPWM(1,68+proportion*6);
		Motor_SetPWM(2,68+proportion*6);
		Motor_SetPWM(3,68+proportion*6);
		Motor_SetPWM(4,68+proportion*6);
	}
	else if(current_left2==1&&current_right1==1&&current_left1==1&&current_right2==0)   //左直角弯
	{
		Motor_SetPWM(leftfront,6-proportion*1);
		Motor_SetPWM(leftback,6-proportion*1);
		Motor_SetPWM(rightfront,100-proportion*2);
		Motor_SetPWM(rightback,100-proportion*2);
	} 
	else if(current_left2==1&&current_right1==1&&current_left1==0&&current_right2==1)     //右直角弯  
	{
		Motor_SetPWM(leftfront,100-proportion*2);
		Motor_SetPWM(leftback,100-proportion*2);
		Motor_SetPWM(rightfront,6-proportion*1);
		Motor_SetPWM(rightback,6-proportion*1);
	}
	else if(current_left2==1&&current_right1==0&&current_left1==0&&current_right2==0)    //直线修正
	{
		Motor_SetPWM(leftfront,80-proportion*2);
		Motor_SetPWM(leftback,80-proportion*2);
		Motor_SetPWM(rightfront,55+proportion*2);
		Motor_SetPWM(rightback,55+proportion*2);
	}
	else if(current_left2==0&&current_right1==1&&current_left1==0&&current_right2==0)   //直线修正
	{
		Motor_SetPWM(leftfront,80-proportion*2);
		Motor_SetPWM(leftback,80-proportion*2);
		Motor_SetPWM(rightfront,55+proportion*2);
		Motor_SetPWM(rightback,55+proportion*2);
	}
	else if(current_left2==0&&current_right1==0&&current_left1==1&&current_right2==0)    //极左修正    
	{
		Motor_SetPWM(leftfront,12-proportion*2);
		Motor_SetPWM(leftback,12-proportion*2);
		Motor_SetPWM(rightfront,100-proportion*2);
		Motor_SetPWM(rightback,100-proportion*2);		
	}
	else if(current_left2==0&&current_right1==0&&current_left1==0&&current_right2==1)    //极右
	{
		Motor_SetPWM(leftfront,100-proportion*2);
		Motor_SetPWM(leftback,100-proportion*2);
		Motor_SetPWM(rightfront,12-proportion*2);
		Motor_SetPWM(rightback,12-proportion*2);		
	}
	else if(current_left2==0&&current_right1==1&&current_left1==0&&current_right2==1)    //右边俩黑
	{
		Motor_SetPWM(leftfront,18);
		Motor_SetPWM(leftback,18);
		Motor_SetPWM(rightfront,90-proportion*2);
		Motor_SetPWM(rightback,90-proportion*2);		
	}
	else if(current_left2==1&&current_right1==0&&current_left1==1&&current_right2==0)  //左边俩黑
	{
		Motor_SetPWM(leftfront,90-proportion*2);
		Motor_SetPWM(leftback,90-proportion*2);
		Motor_SetPWM(rightfront,18);
		Motor_SetPWM(rightback,18);		
	}



}

