#include "sys.h"
#include "stdio.h"
//ALIENTEK ̽����STM32F407������ ʵ��0
//STM32F4����ģ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK
#define NANO_UART USART1


float pitch,roll,yaw; 								  			 //ŷ����(��̬��)
short aacx,aacy,aacz;													 //���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
extern uint16_t channel[10];
int   Encoder_Left,Encoder_Right;         		 //���ұ��������������
int Flag_Stop=0;                               //0���������1�����
int gyro[3],accel[3];
extern unsigned char Uart4_Buffer[20];
extern unsigned char recevie_falg ;
extern unsigned char flagduoji;
unsigned CO2_Date = 0;
unsigned char CH2O_Date = 0;//��ȩ
unsigned char TVOC_Date = 0;//
unsigned char PM2_5 = 0;
unsigned char PM10 = 0;
unsigned char Temperature = 0;
unsigned char Temperature_point = 0;
unsigned char Humidity = 0;
unsigned char Humidity_point = 0;
unsigned char UV_Index=0;//������ǿ��
int temp_voice=0,humid_voice=0;
int 	Moto1=0,Moto2=0;												 //������������ո��������PWM
int Angle1=120,Angle2=120,Angle3=130;
int humidness;
int air_concentration_h=0;
int air_concentration_l=0;
int next_point=0;
int before_point=0;
int Battry_Volt=0;
int action_mode=0;
int charge_status=0;
int plat_state=0;
int temp;
uint8_t actmode=0;

int te_count=0;
int te_count1=0;
int te_count3=0;
float temper;
float humidn;

extern float real_tem,real_hum,real_ph,real_diandao,real_dan,real_lin,real_jia;

u8 ActBuff[25]={0};
u8 testwin;
u8 comdatest;

void initialise(void);

int main(void)
{
//	SystemInit();
	initialise();
	delay_init(168);      //��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	Motor_Init();
	delay_ms(100);//�ȴ�����ģ���ϵ����
	Servo_Init();
//    MPU6050_Init();
//    MPU6050_SetState(ENABLE);
//    OLED_Init();
	HC05_Init();		//��������
	Board_Init();		//��λ������
	Duoji_board_Init();			//����崮��
//		BH1750_Init();	//����pǿ�ȴ�����
//		test1_Init();
	Speaker_Init();
	Water_Init();
	delay_ms(500);
	//Speaker_Play(6);
		//delay_ms(500);
	//while(1);
	while(1)
	{
		if(ActBuff[1]==1) {Action_JUSTgo(0,0);}
		else if(ActBuff[2]==1) {Action_JUSTgo(10,10);}
		else if(ActBuff[3]==1) {Action_JUSTgo(-10,-10);}
		else if(ActBuff[4]==1) {Action_JUSTgo(0,10);}
		else if(ActBuff[5]==1) {Action_JUSTgo(10,0);}
		
		if(ActBuff[6]==1) {Speaker_Play(1);}		//�յ�
		else if(ActBuff[7]==1) {Speaker_Play(2);}		//���ڼ������
		else if(ActBuff[8]==1) {Speaker_Play(3);}		//������
		else if(ActBuff[9]==1) {Speaker_Play(4);}		//���ڼ���Ҷ����
		else if(ActBuff[10]==1) {Speaker_Play(5);}		//�������ҩ
		else if(ActBuff[11]==1) {Speaker_Play(6);}		//���ڼ����Χ����
		
		if(ActBuff[12]==1) {Action_IronArm_Start();}
		else if(ActBuff[13]==1) {Action_IronArm_End();}
		
		if(ActBuff[14]==1) {Duoji_board_Printf("$AAA!");}
		
		if(ActBuff[15]==1) {Board_Printf("1\r\n");}
		else if(ActBuff[16]==1) {Board_Printf("2\r\n");}
	
		if(ActBuff[17]==1) {Water_on();}
		else if(ActBuff[18]==1) {Water_off();}
	
	
	
	}

}
void initialise(void) 
{
	volatile uint32_t _counter = 0;
	SysTick_Config(SystemCoreClock / 1000);
}

//HC05���մ�����
int UART4_IRQHandler(void)	//����4�жϷ������
{	
		u8 com_data; 
		u8 i;
		static u8 RxCounter1=0;
		static u8 RxBuffer1[25]={0};
		static u8 RxState = 0;	
		if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)  	   //�����ж�  
		{
			USART_ClearITPendingBit(UART4,USART_IT_RXNE);   //����жϱ�־
			
			com_data = USART_ReceiveData(UART4);
			comdatest=com_data;
							testwin=1;

			if(RxState==0&&com_data==0xA5)  //0xA5֡ͷ
			{
				RxBuffer1[RxCounter1++]=com_data;
				RxState=1;
			}
			else if(RxState==1)
			{
				RxBuffer1[RxCounter1++]=com_data;
				if(RxCounter1>21)       //RxBuffer1��������,�������ݽ���
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
				for(i=0;i<25;i++)
				{
					ActBuff[i]=RxBuffer1[i];
				}

				
				
				RxState = 0;
				RxCounter1=0;
				for(i=0;i<25;i++)
				{
						RxBuffer1[i]=0x00;      //�����������������
				}
			}
			else   //�����쳣
			{
					RxState = 0;
					RxCounter1=0;
					for(i=0;i<25;i++)
					{
						//ActBuff[i]=0;
							RxBuffer1[i]=0x00;      //�����������������
					}
			}		
		}
	return 0;	
}

