#include "Duoji_board.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

//舵机板串口连接至USART3 初始化 38400 8-N-1
uint8_t Duoji_board_RxData;
uint8_t Duoji_board_RxFlag;

//初始化 默认启用接收中断
void Duoji_board_Init(void)
{
    USART3_Init(115200);
}

void Duoji_board_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void Duoji_board_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Duoji_board_SendByte(Array[i]);
	}
}

void Duoji_board_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Duoji_board_SendByte(String[i]);
	}
}

uint32_t Duoji_board_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Duoji_board_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Duoji_board_SendByte(Number / Duoji_board_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void Duoji_board_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Duoji_board_SendString(String);
}

uint8_t Duoji_board_GetRxFlag(void)
{
	if (Duoji_board_RxFlag == 1)
	{
		Duoji_board_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Duoji_board_GetRxData(void)
{
	return Duoji_board_RxData;
}

// void USART3_IRQHandler(void)
// {
// 	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
// 	{
// 		Duoji_board_RxData = USART_ReceiveData(USART3);
// 		Duoji_board_RxFlag = 1;
// 		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
// 	}
// }

