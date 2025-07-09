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

//CO2,��ȩ��TVOC��PM2.5��PM10���¶ȣ�ʪ����������
void uart4_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //ʹ��GPIOʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //Enable the Usart clock //ʹ��USARTʱ��
	
	//USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //C11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10����ΪUART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11����ΪUART4
	
//	//USART_TX  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //C10
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//����
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
//  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  //USART Initialization Settings ��ʼ������
	USART_InitStructure.USART_BaudRate = bound; //Port rate //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //һ��ֹͣ
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure);      //Initialize serial port 3 //��ʼ������3
	
  //NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                     //Enable serial port 3 //ʹ�ܴ���3 
}

int USART2_IRQHandler(void)	//����4�жϷ������
{
	/*
	  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //������յ�����
  {
      USART_ClearITPendingBit(UART4,USART_IT_RXNE); //����жϱ�־λ
      Uart4_Buffer[Uart4_Rx] = USART_ReceiveData(UART4); //�����ݴ��뻺����
      Uart4_Rx++; //������ָ���1
      //Uart4_Rx &= 0xFF; //��ָֹ��Խ��
  }
  if(USART_GetFlagStatus(UART4,USART_FLAG_ORE) == SET)//��������������
  {
      USART_ClearFlag(UART4,USART_FLAG_ORE); //�����������־λ
      USART_ReceiveData(UART4); //��ȡ���ݼĴ����������SR�Ĵ����е�ORE��־λ
  }
  if(Uart4_Buffer[Uart4_Rx-1] == 0x3C) //������յ�������Ϊ0x3C
  {
      Uart4_Tx = Uart4_Rx-1; 
		
  }
  if((Uart4_Buffer[Uart4_Tx] == 0x3C)&&(Uart4_Buffer[Uart4_Tx+1] == 0x02)) //������յ�������Ϊ0x3C��0x02
  {	   
		
      j++; //��������1
      if(j>16) //�������������16
      {
				
				CO2_Date=1;
          j = 0; //����������
          recevie_falg = 1; //���ձ�־λ��1 
          CO2_Date = (Uart4_Buffer[Uart4_Tx+2]<<8)+Uart4_Buffer[Uart4_Tx+3];   //CO2Ũ��ֵ
          CH2O_Date = (Uart4_Buffer[Uart4_Tx+4]<<8)+Uart4_Buffer[Uart4_Tx+5]; //��ȩŨ��ֵ
          TVOC_Date = (Uart4_Buffer[Uart4_Tx+6]<<8)+Uart4_Buffer[Uart4_Tx+7]; //TVOCŨ��ֵ
          PM2_5 = (Uart4_Buffer[Uart4_Tx+8]<<8)+Uart4_Buffer[Uart4_Tx+9]; //PM2.5Ũ��ֵ
          PM10 = (Uart4_Buffer[Uart4_Tx+10]<<8)+Uart4_Buffer[Uart4_Tx+11]; //PM10Ũ��ֵ
          Temperature = Uart4_Buffer[12]&0x7F; //�¶�ֵ
          Humidity = Uart4_Buffer[14]; //ʪ��ֵ
          Uart4_Sta = 1; //��־λ��1����ʾ������� 
					OLED_ShowNum(0,0,Temperature*100,5,12);
					OLED_ShowNum(0,10,Humidity*100,5,12);
					OLED_ShowNum(0,20,CO2_Date,5,12);
		OLED_Refresh();
					if(Humidity>10)	OLED_ShowNum(0,30,100,5,12);
      }
  }
  if((Uart4_Sta)||(Uart4_Rx>=150))//���������ɻ��߻�����ָ����ڵ���20����������С��
  {
      Uart4_Rx = 0; //������ָ�����㣬�Ա��´�ʹ�� 
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
		if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)  	   //�����ж�  
		{
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);   //����жϱ�־
				com_data = USART_ReceiveData(USART2);
				if(RxState==0&&com_data==0x3c)  //0x2c֡ͷ
				{
					RxBuffer1[RxCounter1++]=com_data;
					RxState=1;
					
				}
				else if(RxState==1&&com_data==0x02)  //0xaa֡ͷ
				{
					RxBuffer1[RxCounter1++]=com_data;
					RxState=2;
					
				}			
				else if(RxState==2)
				{
					RxBuffer1[RxCounter1++]=com_data;
					if(RxCounter1>16)       //RxBuffer1��������,�������ݽ���
					{
						RxState=3;
					}
//					else if(RxCounter1==11&&com_data != 0x5A)
//					{
//						RxState = 0;
//						RxCounter1=0;
//						for(i=0;i<20;i++)
//						{
//								RxBuffer1[i]=0x00;      //�����������������
//						}
//					}
				}
				else if(RxState==3)
				{
					air_concentration_h=RxBuffer1[6];
					air_concentration_l=RxBuffer1[7];
					Temperature = RxBuffer1[12]&0x7F; //�¶�ֵ
					Temperature_point = RxBuffer1[13]; //�¶�ֵ
					Humidity = RxBuffer1[14]; //ʪ��ֵ
					Humidity_point = RxBuffer1[15]; //ʪ��ֵ
					
					RxState = 0;
					RxCounter1=0;
					for(i=0;i<20;i++)
					{
							RxBuffer1[i]=0x00;      //�����������������
					}
				}
					else   //�����쳣
					{
							RxState = 0;
							RxCounter1=0;
							for(i=0;i<20;i++)
							{
									RxBuffer1[i]=0x00;      //�����������������
							}
					}		
		}
	return 0;	
}

//	CO2_Date=1;

