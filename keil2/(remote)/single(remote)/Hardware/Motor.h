#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f4xx.h"

typedef enum
{
    Motor_L,
    Motor_R
}Motor_Type;

void Motor_Init(void);
void Motor_SetOuput(Motor_Type Which, int Output); 
int  Motor_ReadOutput(Motor_Type Which);
void Action_JUSTgo(int8_t SpeedL,int8_t SpeedR);

#endif
