#include "air.h"
unsigned char Uart4_Rx = 0;
unsigned char Uart4_Tx = 0;
unsigned char Uart4_Len = 0;
unsigned char Uart4_Sta = 0;
unsigned char Uart4_Buffer[200] = {0};
unsigned char recevie_falg = 0;

extern unsigned char Data[16];
extern unsigned char CO2_Date;
extern unsigned char CH2O_Date;
extern unsigned char TVOC_Date;
extern unsigned char PM2_5 ;
extern unsigned char PM10;
extern unsigned char Temperature ;
extern unsigned char Temperature_point ;
extern unsigned char Humidity ;
extern unsigned char Humidity_point ;

//CO2,甲醛，TVOC，PM2.5，PM10，温度，湿度七组数据
void uart4_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //Enable the Usart clock //使能USART时钟
	
	//USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //C11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10复用为UART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11复用为UART4
	
//	//USART_TX  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //C10
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
//  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  //USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
  USART_Init(UART4, &USART_InitStructure);      //Initialize serial port 3 //初始化串口3
	
  //NVIC配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
  USART_Cmd(UART4, ENABLE);                     //Enable serial port 3 //使能串口3 
}

int USART2_IRQHandler(void)	//串口4中断服务程序
{
	/*
	  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //如果接收到数据
  {
      USART_ClearITPendingBit(UART4,USART_IT_RXNE); //清除中断标志位
      Uart4_Buffer[Uart4_Rx] = USART_ReceiveData(UART4); //将数据存入缓冲区
      Uart4_Rx++; //缓冲区指针加1
      //Uart4_Rx &= 0xFF; //防止指针越界
  }
  if(USART_GetFlagStatus(UART4,USART_FLAG_ORE) == SET)//如果发生溢出错误
  {
      USART_ClearFlag(UART4,USART_FLAG_ORE); //清除溢出错误标志位
      USART_ReceiveData(UART4); //读取数据寄存器，以清除SR寄存器中的ORE标志位
  }
  if(Uart4_Buffer[Uart4_Rx-1] == 0x3C) //如果接收到的数据为0x3C
  {
      Uart4_Tx = Uart4_Rx-1; 
		
  }
  if((Uart4_Buffer[Uart4_Tx] == 0x3C)&&(Uart4_Buffer[Uart4_Tx+1] == 0x02)) //如果接收到的数据为0x3C和0x02
  {	   
		
      j++; //计数器加1
      if(j>16) //如果计数器大于16
      {
				
				CO2_Date=1;
          j = 0; //计数器清零
          recevie_falg = 1; //接收标志位置1 
          CO2_Date = (Uart4_Buffer[Uart4_Tx+2]<<8)+Uart4_Buffer[Uart4_Tx+3];   //CO2浓度值
          CH2O_Date = (Uart4_Buffer[Uart4_Tx+4]<<8)+Uart4_Buffer[Uart4_Tx+5]; //甲醛浓度值
          TVOC_Date = (Uart4_Buffer[Uart4_Tx+6]<<8)+Uart4_Buffer[Uart4_Tx+7]; //TVOC浓度值
          PM2_5 = (Uart4_Buffer[Uart4_Tx+8]<<8)+Uart4_Buffer[Uart4_Tx+9]; //PM2.5浓度值
          PM10 = (Uart4_Buffer[Uart4_Tx+10]<<8)+Uart4_Buffer[Uart4_Tx+11]; //PM10浓度值
          Temperature = Uart4_Buffer[12]&0x7F; //温度值
          Humidity = Uart4_Buffer[14]; //湿度值
          Uart4_Sta = 1; //标志位置1，表示接收完成 
					OLED_ShowNum(0,0,Temperature*100,5,12);
					OLED_ShowNum(0,10,Humidity*100,5,12);
					OLED_ShowNum(0,20,CO2_Date,5,12);
		OLED_Refresh();
					if(Humidity>10)	OLED_ShowNum(0,30,100,5,12);
      }
  }
  if((Uart4_Sta)||(Uart4_Rx>=150))//如果接收完成或者缓冲区指针大于等于20（缓冲区大小）
  {
      Uart4_Rx = 0; //缓冲区指针清零，以便下次使用 
      Uart4_Tx = 0;
      Uart4_Len = 0;
      Uart4_Sta = 0; 
		
      for(i=0; i<200; i++)
      Uart4_Buffer[i] = 0;
  }
*/
	
	u8 com_data; 
		u8 i;
		static u8 RxCounter1=0;
		static u16 RxBuffer1[20]={0};
		static u8 RxState = 0;	
		if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)  	   //接收中断  
		{
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);   //清除中断标志
				com_data = USART_ReceiveData(USART2);
				if(RxState==0&&com_data==0x3c)  //0x2c帧头
				{
					RxBuffer1[RxCounter1++]=com_data;
					RxState=1;
					
				}
				else if(RxState==1&&com_data==0x02)  //0xaa帧头
				{
					RxBuffer1[RxCounter1++]=com_data;
					RxState=2;
					
				}			
				else if(RxState==2)
				{
					RxBuffer1[RxCounter1++]=com_data;
					if(RxCounter1>16)       //RxBuffer1接受满了,接收数据结束
					{
						RxState=3;
					}
//					else if(RxCounter1==11&&com_data != 0x5A)
//					{
//						RxState = 0;
//						RxCounter1=0;
//						for(i=0;i<20;i++)
//						{
//								RxBuffer1[i]=0x00;      //将存放数据数组清零
//						}
//					}
				}
				else if(RxState==3)
				{
					air_concentration_h=RxBuffer1[6];
					air_concentration_l=RxBuffer1[7];
					Temperature = RxBuffer1[12]&0x7F; //温度值
					Temperature_point = RxBuffer1[13]; //温度值
					Humidity = RxBuffer1[14]; //湿度值
					Humidity_point = RxBuffer1[15]; //湿度值
					
					RxState = 0;
					RxCounter1=0;
					for(i=0;i<20;i++)
					{
							RxBuffer1[i]=0x00;      //将存放数据数组清零
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
		}
	return 0;	
}

//	CO2_Date=1;

