#include "sys.h"
#include "stdio.h"
//ALIENTEK 探索者STM32F407开发板 实验0
//STM32F4工程模板-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
#define NANO_UART USART1


float pitch,roll,yaw; 								  			 //欧拉角(姿态角)
short aacx,aacy,aacz;													 //加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据
extern uint16_t channel[10];
int   Encoder_Left,Encoder_Right;         		 //左右编码器的脉冲计数
int Flag_Stop=0;                               //0电机不动，1电机动
int gyro[3],accel[3];
extern unsigned char Uart4_Buffer[20];
extern unsigned char recevie_falg ;
extern unsigned char flagduoji;
unsigned CO2_Date = 0;
unsigned char CH2O_Date = 0;//甲醛
unsigned char TVOC_Date = 0;//
unsigned char PM2_5 = 0;
unsigned char PM10 = 0;
unsigned char Temperature = 0;
unsigned char Temperature_point = 0;
unsigned char Humidity = 0;
unsigned char Humidity_point = 0;
unsigned char UV_Index=0;//紫外线强度
int temp_voice=0,humid_voice=0;
int 	Moto1=0,Moto2=0;												 //计算出来的最终赋给电机的PWM
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
	delay_init(168);      //初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	Motor_Init();
	delay_ms(100);//等待所有模块上电完成
	Servo_Init();
//    MPU6050_Init();
//    MPU6050_SetState(ENABLE);
//    OLED_Init();
	HC05_Init();		//蓝牙串口
	Board_Init();		//上位机串口
	Duoji_board_Init();			//舵机板串口
//		BH1750_Init();	//光照p强度传感器
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
		
		if(ActBuff[6]==1) {Speaker_Play(1);}		//收到
		else if(ActBuff[7]==1) {Speaker_Play(2);}		//正在检测土壤
		else if(ActBuff[8]==1) {Speaker_Play(3);}		//检测完毕
		else if(ActBuff[9]==1) {Speaker_Play(4);}		//正在检测茶叶病害
		else if(ActBuff[10]==1) {Speaker_Play(5);}		//茶白星喷药
		else if(ActBuff[11]==1) {Speaker_Play(6);}		//正在检测周围环境
		
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

//HC05接收处理函数
int UART4_IRQHandler(void)	//串口4中断服务程序
{	
		u8 com_data; 
		u8 i;
		static u8 RxCounter1=0;
		static u8 RxBuffer1[25]={0};
		static u8 RxState = 0;	
		if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)  	   //接收中断  
		{
			USART_ClearITPendingBit(UART4,USART_IT_RXNE);   //清除中断标志
			
			com_data = USART_ReceiveData(UART4);
			comdatest=com_data;
							testwin=1;

			if(RxState==0&&com_data==0xA5)  //0xA5帧头
			{
				RxBuffer1[RxCounter1++]=com_data;
				RxState=1;
			}
			else if(RxState==1)
			{
				RxBuffer1[RxCounter1++]=com_data;
				if(RxCounter1>21)       //RxBuffer1接受满了,接收数据结束
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
				for(i=0;i<25;i++)
				{
					ActBuff[i]=RxBuffer1[i];
				}

				
				
				RxState = 0;
				RxCounter1=0;
				for(i=0;i<25;i++)
				{
						RxBuffer1[i]=0x00;      //将存放数据数组清零
				}
			}
			else   //接收异常
			{
					RxState = 0;
					RxCounter1=0;
					for(i=0;i<25;i++)
					{
						//ActBuff[i]=0;
							RxBuffer1[i]=0x00;      //将存放数据数组清零
					}
			}		
		}
	return 0;	
}

