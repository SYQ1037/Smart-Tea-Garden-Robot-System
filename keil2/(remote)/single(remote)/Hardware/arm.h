#ifndef __ARM_H
#define __ARM_H 
//S12SD紫外线传感器模块引脚定义 GPIOA_5  ADC1通道5
#include "stdio.h"
#include "sys.h"

void Servo_Init(void);
void Servo_SetAngle1(float Angle);
void Servo_SetAngle2(float Angle);
void Servo_SetAngle3(float Angle);
void Servo_SetAngle4(float Angle);
void PWM_SetCompareT9C1(uint16_t Compare2);
void PWM_SetCompareT9C2(uint16_t Compare2);
void PWM_SetCompareT12C1(uint16_t Compare2);
void PWM_SetCompareT12C2(uint16_t Compare2);
void Servo_SetAngle5(float Angle);
void xia_servo(float angle);
void zhong_servo(float angle);


#endif

