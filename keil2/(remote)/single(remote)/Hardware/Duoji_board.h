#ifndef _DUOJI_BOARD_H_
#define _DUOJI_BOARD_H_

#include "stm32f4xx.h"

void Duoji_board_Init(void);
void Duoji_board_SendByte(uint8_t Byte);
void Duoji_board_SendArray(uint8_t *Array, uint16_t Length);
void Duoji_board_SendString(char *String);
void Duoji_board_SendNumber(uint32_t Number, uint8_t Length);
void Duoji_board_Printf(char *format, ...);

uint8_t Duoji_board_GetRxFlag(void);
uint8_t Duoji_board_GetRxData(void);

#endif
