#include "sys.h"


#define MLX_UART UART5
const uint16_t camColors[256] = {       
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,
0x3810,0x3010,0x3010,0x3010,0x2810,0x2810,0x2810,0x2810,  
0x2010,0x2010,0x2010,0x1810,0x1810,0x1811,0x1811,0x1011,
0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,0x0011,
0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,
0x00B2,0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,
0x0152,0x0172,0x0192,0x0192,0x01B2,0x01D2,0x01F3,0x01F3,
0x0213,0x0233,0x0253,0x0253,0x0273,0x0293,0x02B3,0x02D3,
0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,0x0394,
0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,
0x0474,0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,
0x0554,0x0554,0x0574,0x0574,0x0573,0x0573,0x0573,0x0572,
0x0572,0x0572,0x0571,0x0591,0x0591,0x0590,0x0590,0x058F,
0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,0x05AD,
0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,
0x05C9,0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,
0x05E6,0x05E6,0x05E5,0x05E5,0x0604,0x0604,0x0604,0x0603,
0x0603,0x0602,0x0602,0x0601,0x0621,0x0621,0x0620,0x0620,
0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,0x1E40,
0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,
0x3E60,0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,
0x5E80,0x5E80,0x6680,0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,
0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,0x8EC0,0x96C0,0x96C0,
0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,0xBEE0,
0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,
0xDEE0,0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,
0xE600,0xE5E0,0xE5C0,0xE5A0,0xE580,0xE560,0xE540,0xE520,
0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,0xE460,0xEC40,0xEC20,
0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,0xEB20,
0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,
0xF200,0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,
0xF0E0,0xF0C0,0xF0A0,0xF080,0xF060,0xF040,0xF020,
0x0000,0xffff
};


void set_bound(uint32_t bound)
{
	USART_SendData(MLX_UART, 0xA5);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	USART_SendData(MLX_UART, 0x15);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	switch(bound)
	{
		case 9600:USART_SendData(MLX_UART, 0x01);break;
		case 115200:USART_SendData(MLX_UART, 0x02);break;
		case 460800:USART_SendData(MLX_UART, 0x03);break;
		default:	return;
	}
	if(bound==9600)
	{
		USART_SendData(MLX_UART, 0x01);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xBB);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
	else if(bound==115200)
	{
		USART_SendData(MLX_UART, 0x02);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xBC);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
	else if(bound==460800)
	{
		USART_SendData(MLX_UART, 0x03);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xBD);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
		
		
}
void set_refresh(float Freq)
{
	USART_SendData(MLX_UART, 0xA5);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	USART_SendData(MLX_UART, 0x25);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	if(Freq==0.5)
	{
		USART_SendData(MLX_UART, 0x00);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xCA);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
	else if(Freq==1)
	{
		USART_SendData(MLX_UART, 0x01);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xCB);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
	else if(Freq==2)
	{
		USART_SendData(MLX_UART, 0x02);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xCC);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
	else if(Freq==4)
	{
		USART_SendData(MLX_UART, 0x03);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xCD);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
	else if(Freq==8)
	{
		USART_SendData(MLX_UART, 0x04);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
		USART_SendData(MLX_UART, 0xCE);
		while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	}
}
void save_command(void)
{
	USART_SendData(MLX_UART, 0xA5);
	while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	USART_SendData(MLX_UART, 0x65);
	while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	USART_SendData(MLX_UART, 0x01);
	while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
	USART_SendData(MLX_UART, 0x0B);
	while(USART_GetFlagStatus(MLX_UART, USART_FLAG_TXE) == RESET);
}



volatile uint8_t scale_data[769];
volatile uint8_t pailie1_data[769];
volatile uint8_t pailie2_data[769];
//model代表归一化方式   1：相对温度   0：绝对温度
void data_processing(volatile uint16_t *raw_data,uint16_t max,uint16_t min,uint8_t model,volatile uint16_t *out_pic)
{
	uint16_t i=0,j=0,Col=0,Row=0;
	uint16_t abso_high_temp=10000,abso_low_temp=100;//绝对温度设定值，最高温 最低温
	float scale;
	if(model==1)
	{
		for(i=0;i<769;i++)//归一化到 0-255
		{
			scale=(float)(*(raw_data+i)-min)/(max-min);
			*(scale_data+i)=(uint8_t)(scale*255);
		}
	}
	else if (model==0)
	{
		for(i=0;i<769;i++)//归一化到 0-255
		{
			if(*(raw_data+i)>abso_high_temp)
				scale=1;
			else if(*(raw_data+i)<abso_low_temp)
				scale=0;
			else
				scale=(float)(*(raw_data+i)-abso_low_temp)/(abso_high_temp-abso_low_temp);

			*(scale_data+i)=(uint8_t)(scale*255);
		}
	}
		
	
	
	for(Row=0;Row<24;Row++)
	{
		for(Col=0;Col<32;Col++)
		{
			pailie1_data[Col+Row*32]=scale_data[32*(Row+1)-1-Col];
			out_pic[Col+Row*32]=camColors[scale_data[32*(Row+1)-1-Col]];
		}
		
//		for(Col=0;Col<32;Col++)
//		{
//			out_pic[Col+Row*32]=camColors[scale_data[32*Row+Col]];//左右镜像后
//		}

	}
//	for(j=0;j<769;j++)
//	{
//		pailie2_data[j]=pailie1_data[j];
//	}
	
}


volatile uint16_t temperature_data[769];
volatile uint16_t MAX_temperature;
volatile uint16_t MIN_temperature;
volatile uint8_t  Data_get_ready;
volatile uint16_t hongwai_pic[769];
void uart5_init(uint32_t bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//使能USART6时钟              RCC_APB2Periph_USART6
 
	//串口2对应引脚复用映射      PC12是TX PD2是RX
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOA3复用为USART2
	
	//USART6端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

   //USART6 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART5, &USART_InitStructure); //初始化串口
	
	USART_Cmd(UART5, ENABLE);  //使能串口
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;       //串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		    //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
} 

void UART5_IRQHandler(void)
{
	uint8_t Res;
	static uint16_t t=0,max=0,min=20000;
	static uint8_t  temp=0;

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		
		Res =USART_ReceiveData(UART5);//(USART2->DR);	//读取接收到的数据
		
		if(t==0)
		{
			if(Res==0x5A)	t++;
				else 		t=0;
			
		}
		else if(t==1)
		{
			if(Res==0x5A)	t++;
				else 		t=0;
			
		}
		else if(t==2)
		{
			t++;
		}
		else if(t==3)
		{
			t++;
		}
		else if(t>=4&&t<=1539)
		{
			
			if(t%2==0)temp=Res;
			else if(t%2==1)
			{
				temperature_data[(t-4)/2]=temp+Res*256;
				if(temperature_data[(t-4)/2]>max)max=temperature_data[(t-4)/2];
				if(temperature_data[(t-4)/2]<min)min=temperature_data[(t-4)/2];
			}
			
			t++;
		}
		else if(t==1540||t==1541)
		{
			MAX_temperature=max;
			MIN_temperature=min;
			if(t%2==0)temp=Res;
			else if(t%2==1)temperature_data[(t-4)/2]=temp+Res*256;
			
			t++;
		}
		else 
		{
			max=0;
			min=20000;
			t=0;
			data_processing(temperature_data,MAX_temperature,MIN_temperature,1,hongwai_pic);
			Data_get_ready=1;
		}	 
		
	}
	USART_ClearITPendingBit(UART5,USART_IT_RXNE);   //清除中断标志
}  

