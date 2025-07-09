#include "Speaker.h"
#include "delay.h"

//IO触发的语音播报

//初始化语音播报
void Speaker_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG| RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStruct.GPIO_Pin = IO0_GPIO_Pin|IO1_GPIO_Pin|IO2_GPIO_Pin|IO3_GPIO_Pin|IO4_GPIO_Pin|IO5_GPIO_Pin|IO6_GPIO_Pin;
 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
 	GPIO_Init(IO0_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = IO7_GPIO_Pin;
 	GPIO_Init(IO7_GPIO, &GPIO_InitStruct);
	
    //初始化

	GPIO_WriteBit(IO0_GPIO,IO0_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO1_GPIO,IO1_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO2_GPIO,IO2_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO3_GPIO,IO3_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO4_GPIO,IO4_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO5_GPIO,IO5_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO6_GPIO,IO6_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO7_GPIO,IO7_GPIO_Pin,(BitAction)1);
}

/// @brief 语音播报
/// @param Which_Song 

// 1 收到
// 2 正在检测土壤
// 3 检测完毕
// 4 正在检测茶叶病害
// 5 检测到可能发生茶白星病，即将进行喷药
// 6 正在检测周围环境
void Speaker_Play(SongType Which_Song)//index  1-255
{
	Which_Song & 0x01  ?  GPIO_WriteBit(IO0_GPIO,IO0_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO0_GPIO,IO0_GPIO_Pin,(BitAction)1) ;//& 0000 0001
	Which_Song & 0x02  ?  GPIO_WriteBit(IO1_GPIO,IO1_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO1_GPIO,IO1_GPIO_Pin,(BitAction)1) ;//& 0000 0010
	Which_Song & 0x04  ?  GPIO_WriteBit(IO2_GPIO,IO2_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO2_GPIO,IO2_GPIO_Pin,(BitAction)1) ;//& 0000 0100
	Which_Song & 0x08  ?  GPIO_WriteBit(IO3_GPIO,IO3_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO3_GPIO,IO3_GPIO_Pin,(BitAction)1) ;//& 0000 1000
	Which_Song & 0x10  ?  GPIO_WriteBit(IO4_GPIO,IO4_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO4_GPIO,IO4_GPIO_Pin,(BitAction)1) ;//& 0001 0000
	Which_Song & 0x20  ?  GPIO_WriteBit(IO5_GPIO,IO5_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO5_GPIO,IO5_GPIO_Pin,(BitAction)1) ;//& 0010 0000
	Which_Song & 0x40  ?  GPIO_WriteBit(IO6_GPIO,IO6_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO6_GPIO,IO6_GPIO_Pin,(BitAction)1) ;//& 0100 0000
	Which_Song & 0x80  ?  GPIO_WriteBit(IO7_GPIO,IO7_GPIO_Pin,(BitAction)0) : GPIO_WriteBit(IO7_GPIO,IO7_GPIO_Pin,(BitAction)1) ;//& 1000 0000
	
	delay_ms(200);
	GPIO_WriteBit(IO0_GPIO,IO0_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO1_GPIO,IO1_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO2_GPIO,IO2_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO3_GPIO,IO3_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO4_GPIO,IO4_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO5_GPIO,IO5_GPIO_Pin,(BitAction)1);
	GPIO_WriteBit(IO6_GPIO,IO6_GPIO_Pin,(BitAction)1);
}