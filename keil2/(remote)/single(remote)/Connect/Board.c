#include "Board.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

//上位机串口连接至USART6 初始化 38400 8-N-1
uint8_t Board_RxData;
uint8_t Board_RxFlag;

//初始化 默认启用接收中断
void Board_Init(void)
{
    USART6_Init(115200);
}

void Board_SendByte(uint8_t Byte)
{
	USART_SendData(USART6, Byte);
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
}

void Board_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Board_SendByte(Array[i]);
	}
}

void Board_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Board_SendByte(String[i]);
	}
}

uint32_t Board_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Board_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Board_SendByte(Number / Board_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void Board_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Board_SendString(String);
}

uint8_t Board_GetRxFlag(void)
{
	if (Board_RxFlag == 1)
	{
		Board_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Board_GetRxData(void)
{
	return Board_RxData;
}

// void USART6_IRQHandler(void)
// {
// 	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
// 	{
// 		Board_RxData = USART_ReceiveData(USART6);
// 		Board_RxFlag = 1;
// 		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
// 	}
// }

