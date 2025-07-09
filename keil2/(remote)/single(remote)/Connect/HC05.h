#ifndef _HC05_H_
#define _HC05_H_

#include "stm32f4xx.h"

void HC05_Init(void);
void HC05_SendByte(uint8_t Byte);
void HC05_SendArray(uint8_t *Array, uint16_t Length);
void HC05_SendString(char *String);
void HC05_SendNumber(uint32_t Number, uint8_t Length);
void HC05_Printf(char *format, ...);

uint8_t HC05_GetRxFlag(void);
uint8_t HC05_GetRxData(void);

#endif
