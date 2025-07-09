#ifndef __AIR_H
#define __AIR_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
	
//void uart_init(u32 bound);
void uart4_init(u32 bound);
int USART2_IRQHandler(void);	//串口4中断服务程序

#endif


