#ifndef _SERVO_SG90_H
#define _SERVO_SG90_H

#include "stm32f4xx.h"
#include "sys.h"

typedef enum
{
    Servo_Soil_first,
    Servo_Soil_second
}ServoWhichType;

typedef enum 
{
    Servo_Soil_First_Start = 25,
    Servo_Soil_First_Middle = 10,
    Servo_Soil_First_End = 20,
    Servo_Soil_Second_Start = 5,
    Servo_Soil_Second_Middle,
    Servo_Soil_Second_End = 18
}ServoStateType;//¶æ»ú×´Ì¬


void Servo_Init(void);
void Servo_SetDuty(ServoWhichType Which,uint16_t Duty);
float Servo_ReadOutput(ServoWhichType Which);
void Action_IronArm_Start(void);
void Action_IronArm_End(void);

#endif
