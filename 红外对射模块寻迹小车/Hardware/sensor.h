#ifndef __SENSOR_H
#define __SENSOR_H

#include "stm32f10x.h"

#define FILTER_SAMPLES 3

// 滤波数据结构体
typedef struct {
    uint16_t history[FILTER_SAMPLES];
    uint8_t index;
} SensorFilter_t;

// 声明外部变量
extern SensorFilter_t sensor_left1, sensor_left2, sensor_right1, sensor_right2;

// 函数声明
void SensorInit(void);
void directionjudge(void);
void filter_init(SensorFilter_t* filter);
void sensor_filters_init(void);
uint16_t sensor_median_filter(SensorFilter_t* filter, uint16_t raw_value);

#endif
