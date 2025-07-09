#ifndef __GD_H
#define __GD_H	 
#include "sys.h"

#define GD1  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)	
#define GD2  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)

void GD_Init(void);//IO≥ı ºªØ
void EXTIX0_Init(void);

#endif
