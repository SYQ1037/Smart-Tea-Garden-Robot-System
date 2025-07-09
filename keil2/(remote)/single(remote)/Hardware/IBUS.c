#include "IBUS.h"
unsigned char flagduoji=2;
uint8_t rxdata[32];//用于存储数据，数组大小，就是字节数
uint8_t rx_getflag = 0;
uint16_t channel[10] = {0};//保存通道数值
uint16_t checksum_cal, checksum_ibus; //零时存储
uint8_t user_channels = 10;
unsigned int temper_time=0;
unsigned int shidu_time=0;
extern int Angle1,Angle2,Angle3;
extern int temp_voice,humid_voice;
extern unsigned char Temperature ;
extern unsigned char Temperature_point ;
extern unsigned char Humidity ;
extern unsigned char Humidity_point ;
extern int next_point;
extern int before_point;

uint16_t zhuangtaiwei=0;

/**
  * @brief  串口初始化函数
  * @param  bit_lv: 参数：波特率
  */
void uart3_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //Enable the Usart clock //使能USART时钟
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	
	//USART_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //B10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  //USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //B11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  //NVIC配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
	
  //USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx ;	//Sending and receiving mode //收发模式
  USART_Init(USART3, &USART_InitStructure);      //Initialize serial port 3 //初始化串口3
	
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
  USART_Cmd(USART3, ENABLE);                     //Enable serial port 3 //使能串口3 
}


void choose_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //D0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void read_ibus(uint8_t *arr)
{
    int i=0,j=0;
    uint16_t channel_buffer[IBUS_MAX_CHANNLES] = {0};//通道数据暂存器
		if(arr[0]==0x20&&arr[1]==0x40)
		{
			checksum_cal = 0xffff - arr[0] - arr[1];
			for(i = 0; i < IBUS_MAX_CHANNLES; i++)//10个通道，循环十次，保存数据
			{
				channel_buffer[i] = (uint16_t)(arr[i * 2 + 3] << 8 | arr[i * 2 + 2]);//通道数据暂存器
				checksum_cal = checksum_cal - arr[i * 2 + 3] - arr[i * 2 + 2]; 
			}
			checksum_ibus = arr[31] << 8 | arr[30]; //计算校验值
			if(checksum_cal == checksum_ibus) //对比校验值
			{
				for(j = 0; j < user_channels; j++) //将值赋值到通道内
				{
					channel[j] = channel_buffer[j];
				}
			}
		}
}



void USART5_IRQHandler_de(void) //串口5中断
{
	static uint8_t rxstart = 0; //状态机的标志位
	static uint8_t rx_arr_flag = 0; //保存数据的数组下标
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET) //判断标志位是否正确
	{
		uint8_t data = USART_ReceiveData(USART3);//将数据1Byte数据读取到data

		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0) == 0)//蓝牙模式
		{
			switch(data)
			{
				case '0': stop();break;
				case '1': qianjin();break;
				case '2': houtui();break;
				case '3': zuozhuan();break;
				
				case '4': youzhuan();break;
				case '5': play_voice(1);break;//启动
				case '6': play_voice(2+temper_time);temper_time=temper_time+1;if(temper_time>=3)temper_time=0;break;//当前气温 24.8 25.6 25.3
				case '7': play_voice(5+shidu_time);if(shidu_time>=3)shidu_time=0;shidu_time=shidu_time+1;shidu_time%=3;break;//当前湿度 51.2 50.3 49.6
				case '8': play_voice(8);break;//紫外线强度
				case '9': play_voice(9);break;//土壤ph值
				case 'a':flagduoji=1;break;//第一个舵机80
				case 'b':flagduoji=2;break;//第一个舵机195
			}
			
			
		}
		else//IBUS
		{
			if (rxstart == 0 ) //状态机
			{
				if(data == 0x20 ) //判断数据的起始位，只是保险使用，还会有2层保险
				{
					rx_arr_flag = 0;
					rxstart = 1;
					rxdata[rx_arr_flag++] = data;
				}
			}
			else if (rxstart == 1 ) //状态机
			{
				if(data == 0x40 ) //判断数据的起始位，只是保险使用，还会有2层保险
				{
					rxstart = 2;
					rxdata[rx_arr_flag++] = data;
				}
			}
			else if (rxstart == 2)
			{
				rxdata[rx_arr_flag] = data;
				rx_arr_flag++;
				if(rx_arr_flag >= 32)
				{
					rxstart = 0;
					rx_arr_flag = 0;
					read_ibus(rxdata);
				}
			}
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		}
		
	}
}


void play_temp()
{
							temp_voice=1;
							temp_voice=0;
							play_voice_mul(2);
							if(Temperature/10==0&&Temperature%10!=0)	play_voice_mul(4+Temperature%10);
							else if(Temperature/10==1)	{play_voice_mul(3);if(Temperature%10!=0) play_voice_mul(4+Temperature%10);}
							else if(Temperature/10==2)	{play_voice_mul(4);if(Temperature%10!=0) play_voice_mul(4+Temperature%10);}
							play_voice_mul(14);
	
}
void play_humid()
{
							humid_voice=1;
							play_voice_mul(15);
							if(Humidity/10!=0)
								play_voice_mul(Humidity/10+4);
							play_voice_mul(3);
							if(Humidity%10!=0)
								play_voice_mul(Humidity%10+4);
							humid_voice=0;
	
}

extern int temp;
uint8_t uart_mstop=0;
void USART3_IRQHandler(void)			 
{
		u8 com_data; 
		u8 i;
		
		static u8 RxCounter1=0;
		static u8 RxBuffer1[20]={0};
		static u8 RxState = 0;	
		if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)  	   //接收中断  
		{
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);   //清除中断标志
				com_data = USART_ReceiveData(USART3);
				if(uart_mstop==0)	{uart_mstop=1;stop();}
				if(RxState==0&&com_data==0xA5)  //0x2c帧头
				{
					
					RxBuffer1[RxCounter1++]=com_data;
					RxState=1;
					
					
				}
//				else if(RxState==1&&com_data==0xaa)  //0xaa帧头
//				{
//					RxBuffer1[RxCounter1++]=com_data;
//					RxState=2;
//				}			
				else if(RxState==1)
				{
					RxBuffer1[RxCounter1++]=com_data;
					if(RxCounter1>=4||com_data == 0x5A)       //RxBuffer1接受满了,接收数据结束
					{
						RxState=3;
					}
					
					else if(RxCounter1>=4&&com_data != 0x5A)
					{
						RxState = 0;
						RxCounter1=0;
						for(i=0;i<20;i++)
						{
								RxBuffer1[i]=0x00;      //将存放数据数组清零
						}
					}
				}
				else   //接收异常
				{
						RxState = 0;
						RxCounter1=0;
						for(i=0;i<20;i++)
						{
								RxBuffer1[i]=0x00;      //将存放数据数组清零
						}
				}
			if(RxState==3)
			{
				if((RxBuffer1[1])>>0&0x01)	stop();
				else if((RxBuffer1[1]>>1)&0x01)	qianjin();	
				else if((RxBuffer1[1]>>2)&0x01)	houtui();		
				else if((RxBuffer1[1]>>3)&0x01)	zuozhuan();	
				else if((RxBuffer1[1]>>4)&0x01)	youzhuan();	else stop();
				if((RxBuffer1[1]>>6)&0x01&&Temperature!=0)	tempture_voice();
				if((RxBuffer1[1]>>7)&0x01&&Humidity!=0)	Humidity_voice();
				if((RxBuffer1[2]>>0)&0x01)	play_voice(8);
				if((RxBuffer1[2]>>1)&0x01)	play_voice(9);
				if((RxBuffer1[2]>>2)&0x01)	Angle1=Angle1+1;
				if((RxBuffer1[2]>>3)&0x01)	Angle1=Angle1-1;
				if((RxBuffer1[2]>>4)&0x01)	Angle2=Angle2+1;
				if((RxBuffer1[2]>>5)&0x01)	Angle2=Angle2-1;
				if((RxBuffer1[2]>>6)&0x01)	Angle3=Angle3+1;
				if((RxBuffer1[2]>>7)&0x01)	Angle3=Angle3-1; 
				if((RxBuffer1[3]>>0)&0x01)	zhuangtaiwei=16;//检测土壤
				if((RxBuffer1[3]>>1)&0x01)	zhuangtaiwei=17;//检测树叶
				if((RxBuffer1[3]>>2)&0x01)	zhuangtaiwei=18;//向上喷药
				if((RxBuffer1[3]>>3)&0x01)	zhuangtaiwei=19;//向下喷药
				if((RxBuffer1[3]>>4)&0x01)	zhuangtaiwei=20;//复位
				if((RxBuffer1[3]>>5)&0x01)	zhuangtaiwei=21;//巡检模式
				if((RxBuffer1[3]>>6)&0x01)	zhuangtaiwei=22;//浇灌模式
				
				
				RxState = 0;
				RxCounter1=0;
				for(i=0;i<20;i++)
				{
						RxBuffer1[i]=0x00;      //将存放数据数组清零
				}
			}
			if(RxState==0 & 0)
			{
				switch(com_data)
				{
					case '0': stop();break;
					case '1': qianjin();break;
					case '2': houtui();break;
					case '3': zuozhuan();break;
					case '4': youzhuan();break;
					case '5': play_voice(1);break;//启动
					case '6': play_temp();break;//当前气温 24.8 25.6 25.3
					case '7': play_humid();break;//当前湿度 51.2 50.3 49.6
					case '8': next_point=1;break;//紫外线强度
					case '9': before_point=1;break;//土壤ph值
					case 'a':flagduoji=1;break;//第一个舵机80
					case 'b':flagduoji=2;break;//第一个舵机195
				}			
			}
			
		}
}


//								unsigned char a[100]={0};a[0]=0xAA;a[1]=0x1B;								
//							a[2]=0x30;a[3]=0x32;
//							if(Temperature/10==0&&Temperature%10!=0)	
//								{
//									a[6]=0x30+(4+Temperature%10)/256;
//									a[7]=0x30+(4+Temperature%10)%256;
//								}
//							else if(Temperature/10==1)
//							{
//								a[4]=0x30;a[5]=0x33;
//								if(Temperature%10!=0)
//								{
//									 a[6]=0x30+(4+Temperature%10)/256;
//									a[7]=0x30+(4+Temperature%10)%256;
//								}
//							}
//							else if(Temperature/10==2)
//							{
//								a[4]=0x30;a[5]=0x34;
//								if(Temperature%10!=0)
//								{
//									 a[6]=0x30+(4+Temperature%10)/256;
//									a[7]=0x30+(4+Temperature%10)%256;
//								}
//							}
//							a[8]=0x31;a[9]=0x34;
//							a[2]=0x08;
//							play_and(a);  
