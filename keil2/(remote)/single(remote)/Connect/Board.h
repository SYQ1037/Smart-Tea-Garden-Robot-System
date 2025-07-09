#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32f4xx.h"

void Board_Init(void);
void Board_SendByte(uint8_t Byte);
void Board_SendArray(uint8_t *Array, uint16_t Length);
void Board_SendString(char *String);
void Board_SendNumber(uint32_t Number, uint8_t Length);
void Board_Printf(char *format, ...);

uint8_t Board_GetRxFlag(void);
uint8_t Board_GetRxData(void);

#endif
