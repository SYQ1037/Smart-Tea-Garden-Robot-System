#include "sys.h"		    

u8 i=0,t=0;
u8 cnt=0;
u8 rs485buf[39];
u8 shidu[8]={0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};
u8 wendu[8]={0x01,0x03,0x00,0x01,0x00,0x06,0x94,0x08};	
u8 diandao[8]={0x01,0x03,0x00,0x02,0x00,0x03,0xA4,0x0B};
u8 PH[8]={0x01,0x03,0x00,0x03,0x00,0x01,0x74,0x0A};
u8 dan[8]={0x01,0x03,0x00,0x04,0x00,0x04,0x05,0xC8};
u8 lin[8]={0x01,0x03,0x00,0x05,0x00,0x07,0x14,0x09};
u8 jia[8]={0x01,0x03,0x00,0x06,0x00,0x05,0x65,0xC8};
#if 0
u8 guang[8]={0x01,0x03,0x00,0x07,0x00,0x02,0x75,0xCA};//盐度
u8 turang[8]={0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};//钾含量暂存值
#endif
//第一位：设备地址码
//第二位：功能码，即读取/写入
//第三四位：起始寄存器地址
//第五六位：读取寄存器的个数，0x00 0x01即读取一个寄存器，每个寄存器可以存储十六位数据，大多数数据可以存在一个寄存器中
//第七八位：CRC校验码


u8 shu;
int tem,hum,light,soil;
int ph,diandaovalue,danvalue,linvalue,jiavalue;

float real_tem;
float real_hum;
float real_ph;
float real_diandao;
float real_dan;
float real_lin;
float real_jia;

int tem_gao;
int tem_di;
int hum_gao;
int hum_di;
int ph_gao;
int ph_di;
int diandao_gao;
int diandao_di;




#ifdef EN_UART4_RX   	//如果使能了接收


//接收缓存区 	
u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 RS485_RX_CNT=0;   		  
  
void UART4_IRQHandler_DE(void)
{
	u8 res;	   

 	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET) //接收到数据
	{	 
	 			 
		res =USART_ReceiveData(UART4); 	//读取接收到的数据
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//记录接收到的值
			RS485_RX_CNT++;						//接收数据增加1 
		} 
	} 
	if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x0C)
	{
		tem_gao=RS485_RX_BUF[3];
		tem_di=RS485_RX_BUF[4];
		
		tem=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
				real_tem=tem/10.0f;
	}
	else if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x04)
	{
		hum_gao=RS485_RX_BUF[3];
		hum_di=RS485_RX_BUF[4];
		hum=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
		real_hum=hum/10.0f;
	}
	else if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x02)
	{
		ph_gao=RS485_RX_BUF[3];
		ph_di=RS485_RX_BUF[4];
		ph=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
		real_ph=ph/10.0f;
	}
	else if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x06)
	{
		diandao_gao=RS485_RX_BUF[3];
		diandao_di=RS485_RX_BUF[4];
		diandaovalue=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
		real_diandao=diandaovalue;
	}
	else if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x08)
	{
		danvalue=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
		real_dan=danvalue;
	}
	else if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x0E)
	{
		linvalue=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
		real_lin=linvalue;
	}
	else if(RS485_RX_BUF[0]==0x01&&RS485_RX_BUF[1]==0x03&&RS485_RX_BUF[2]==0x0A&&RS485_RX_BUF[3]==0x00)
	{
		jiavalue=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
		real_jia=jiavalue;
	}
	
USART_ClearITPendingBit(UART4,USART_IT_RXNE);   //清除中断标志	
} 
#endif										 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void RS485_Init(u32 bound)
{  
		GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //Enable the Usart clock //使能USART时钟
	
	
	//USART_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //C10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //C11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10复用为UART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11复用为UART4
	
	
	
 #ifdef EN_UART4_RX		  	//如果使能了接收
   //NVIC配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
  USART_Init(UART4, &USART_InitStructure);      //Initialize serial port 3 //初始化串口3
	

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
  USART_Cmd(UART4, ENABLE);                     //Enable serial port 3 //使能串口3 



 #endif

  RS485_TX_EN=0;			//默认为接收模式
 
}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			//设置为发送模式
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART4,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);		//等待发送结束
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//设置为接收模式	
}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//记录本次数据长度
		RS485_RX_CNT=0;		//清零
	}
}


void RS485_NUM_GET(void)
{
	if(shu)//接收到有数据
		{
//			if(shu>7)shu=7;//最大是5个数据.
 			for(i=0;i<shu;i++)      //LCD_ShowxNum(30+i*32,230,rs485buf[i],3,16,0X80);	//显示数据
		  if(rs485buf[0]==0x01&&rs485buf[1]==0x03&&rs485buf[2]==0x02&&rs485buf[3]==0x02)
      {
			hum=rs485buf[3]*256+rs485buf[4];
			}
			else if(rs485buf[0]==0x01&&rs485buf[1]==0x03&&rs485buf[2]==0x02&&rs485buf[3]==0x01)
      {
			tem=rs485buf[3]*256+rs485buf[4];
				real_tem=tem/10.0f;
			}
		}
	}		
void wen(void)
{
	RS485_Send_Data(wendu,8);
//	RS485_Receive_Data(rs485buf,&shu);
//	RS485_NUM_GET();
}
	
void shi(void)
{
	RS485_Send_Data(shidu,8);
//	RS485_Receive_Data(rs485buf,&shu);
//	RS485_NUM_GET();
}

void PH_value(void)
{
	RS485_Send_Data(PH,8);
}

void diaodao_value(void)
{
	RS485_Send_Data(diandao,8);	
}
	
void dan_value(void)
{
	RS485_Send_Data(dan,8);	
}

void lin_value(void)
{
	RS485_Send_Data(lin,8);	
}

void jia_value(void)
{
	RS485_Send_Data(jia,8);	
}
