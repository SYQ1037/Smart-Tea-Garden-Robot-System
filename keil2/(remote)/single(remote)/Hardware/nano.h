#ifndef __NANO_H
#define __NANO_H 
//S12SD�����ߴ�����ģ�����Ŷ��� GPIOA_5  ADC1ͨ��5

void nano_uart1_init(u32 bound);
void TIM7_INT_Init(uint16_t arr,uint16_t psc);
void uart_senddata(USART_TypeDef* USARTx, uint16_t Data);

#endif

