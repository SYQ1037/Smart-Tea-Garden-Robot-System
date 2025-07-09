#ifndef __ZIWAIXIAN_H
#define __ZIWAIXIAN_H 
//S12SD紫外线传感器模块引脚定义 GPIOA_5  ADC1通道5
#include "stdio.h"
#include "sys.h"
void  Adc_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
#endif

