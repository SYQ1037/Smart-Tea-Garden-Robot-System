#include "Water.h"
#include <stdio.h>
#include <stdarg.h>

void Water_Init(void) 
	{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ��GPIOBʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // ����GPIOB_PIN_0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ��ʼ״̬Ϊ�͵�ƽ������������
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void Water_on(void)
{
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void Water_off(void)
{
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}
