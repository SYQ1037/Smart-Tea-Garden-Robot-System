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
u8 guang[8]={0x01,0x03,0x00,0x07,0x00,0x02,0x75,0xCA};//�ζ�
u8 turang[8]={0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};//�غ����ݴ�ֵ
#endif
//��һλ���豸��ַ��
//�ڶ�λ�������룬����ȡ/д��
//������λ����ʼ�Ĵ�����ַ
//������λ����ȡ�Ĵ����ĸ�����0x00 0x01����ȡһ���Ĵ�����ÿ���Ĵ������Դ洢ʮ��λ���ݣ���������ݿ��Դ���һ���Ĵ�����
//���߰�λ��CRCУ����


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




#ifdef EN_UART4_RX   	//���ʹ���˽���


//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT=0;   		  
  
void UART4_IRQHandler_DE(void)
{
	u8 res;	   

 	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET) //���յ�����
	{	 
	 			 
		res =USART_ReceiveData(UART4); 	//��ȡ���յ�������
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
			RS485_RX_CNT++;						//������������1 
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
	
USART_ClearITPendingBit(UART4,USART_IT_RXNE);   //����жϱ�־	
} 
#endif										 
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void RS485_Init(u32 bound)
{  
		GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //ʹ��GPIOʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //Enable the Usart clock //ʹ��USARTʱ��
	
	
	//USART_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //C10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //C11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10����ΪUART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11����ΪUART4
	
	
	
 #ifdef EN_UART4_RX		  	//���ʹ���˽���
   //NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound; //Port rate //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //һ��ֹͣ
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure);      //Initialize serial port 3 //��ʼ������3
	

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                     //Enable serial port 3 //ʹ�ܴ���3 



 #endif

  RS485_TX_EN=0;			//Ĭ��Ϊ����ģʽ
 
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			//����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART4,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);		//�ȴ����ͽ���
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
	}
}


void RS485_NUM_GET(void)
{
	if(shu)//���յ�������
		{
//			if(shu>7)shu=7;//�����5������.
 			for(i=0;i<shu;i++)      //LCD_ShowxNum(30+i*32,230,rs485buf[i],3,16,0X80);	//��ʾ����
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
