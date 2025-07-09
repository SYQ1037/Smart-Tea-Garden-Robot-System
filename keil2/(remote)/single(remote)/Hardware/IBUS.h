#ifndef _IBUS_H
#define _IBUS_H

#include "stdio.h"
#include "sys.h"



#define IBUS_UART_INSTANCE		(USART3)
#define IBUS_USER_CHANNELS		10			// Use 6 channels
/* User configuration */

#define IBUS_LENGTH				0x20	// 32 bytes
#define IBUS_COMMAND40			0x40	// Command to set servo or motor speed is always 0x40
#define IBUS_MAX_CHANNLES		14

void uart3_init(uint32_t bit_lv);
void USART3_IRQHandler(void);
void choose_init(void);

extern uint16_t zhuangtaiwei;

#endif

