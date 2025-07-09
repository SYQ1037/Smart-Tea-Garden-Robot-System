#include "PCA9685.h"
#include "IIC.h"
#include "delay.h"
#include <math.h>
//#include "led.h"
#include "usart.h"
#include "sys.h"	
 
void PCA9685_Init_270(float hz,u16 angle)
{
	u32 off = 0;
	IIC_Init();
	PCA9685_Write(PCA_Model,0x00);
	PCA9685_setFreq(hz);
    off = (u32)(103+angle*1.52);  //270度舵机，每转动一度=1.52   0度起始位置：103
	PCA9685_setPWM(0,0,off);
	PCA9685_setPWM(1,0,off);
	PCA9685_setPWM(2,0,off);
	PCA9685_setPWM(3,0,off);
	PCA9685_setPWM(4,0,off);
	delay_ms(100);
	
}
void Beng_init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
 
  //GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6| GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//设置成上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}	
 void PCA9685_Init_180(float hz,u16 angle)
{
	u32 off = 0;
	IIC_Init();
	PCA9685_Write(PCA_Model,0x00);
	PCA9685_setFreq(hz);
    off = (u32)(103+angle*2.28);  //180度舵机，每转动一度=2.28   0度起始位置：103
	PCA9685_setPWM(5,0,off);
	PCA9685_setPWM(14,0,off);
	PCA9685_setPWM(15,0,off);
	delay_ms(100);
	
}
void PCA9685_Write(u8 addr,u8 data)
{
	IIC_Start();
	
	IIC_Send_Byte(PCA_Addr);
	IIC_NAck();
	
	IIC_Send_Byte(addr);
	IIC_NAck();
	
	IIC_Send_Byte(data);
	IIC_NAck();
	
	IIC_Stop();
	
	
}
 
u8 PCA9685_Read(u8 addr)
{
	u8 data;
	
	IIC_Start();
	
	IIC_Send_Byte(PCA_Addr);
	IIC_NAck();
	
	IIC_Send_Byte(addr);
	IIC_NAck();
	
	IIC_Stop();
	
	delay_us(10);
 
	
	IIC_Start();
 
	IIC_Send_Byte(PCA_Addr|0x01);
	IIC_NAck();
	
	data = IIC_Read_Byte(0);
	
	IIC_Stop();
	
	return data;
	
}
 
void PCA9685_setPWM(u8 num,u32 on,u32 off)
{
	IIC_Start();
	
	IIC_Send_Byte(PCA_Addr);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(LED0_ON_L+4*num);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(on&0xFF);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(on>>8);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(off&0xFF);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(off>>8);
	IIC_Wait_Ack();
	
	IIC_Stop();
	
}
 
void PCA9685_setFreq(float freq)
{
	u8 prescale,oldmode,newmode;
	
	double prescaleval;
	
	freq *= 0.98; // 经验值可微调
	prescaleval = 25000000;//芯片自带
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;
	prescale = floor(prescaleval+0.5f);
	oldmode = PCA9685_Read(PCA_Model);
	
	newmode = (oldmode&0x7F)|0x10;
	PCA9685_Write(PCA_Model,newmode);
	PCA9685_Write(PCA_Pre,prescale);
	PCA9685_Write(PCA_Model,oldmode);
	delay_ms(5);
	PCA9685_Write(PCA_Model,oldmode|0xa1);
	
	
}
 
void setAngle_270(u8 num,u16 angle)
{
	u32 off = 0;                
	off = (u32)(103+angle*1.52);  //270度舵机，每转动一度=1.52   0度起始位置：103
	PCA9685_setPWM(num,0,off);
}
void setAngle_180(u8 num,u16 angle)
{
	u32 off = 0;                
	off = (u32)(103+angle*2.28);  //180度舵机，每转动一度=2.28   0度起始位置：103
	PCA9685_setPWM(num,0,off);
}

 void JIAN_CE()
 {
		delay_ms(1000);   
		setAngle_270(0,45);//小于135向右	大于135向左	
		delay_ms(500);
		setAngle_270(3,80);//小于135向后	大于135向前
		delay_ms(500);
		setAngle_270(2,80);//小于135向前	大于135向后	
		delay_ms(500);
		setAngle_270(1,200);//小于135向前	大于135向后
		play_voice(22);//播报：正在检测土壤状况，请稍后	 
		delay_ms(5000);
//		setAngle_180(5,120);// 0 -> 180 = 松到紧
				
 }
 void RECOVER_DA()
 {
		setAngle_270(0,45);//小于135向右	大于135向左	
	 delay_ms(100);
		setAngle_270(1,165);//小于135向后	大于135向前
	 delay_ms(100);
		setAngle_270(2,105);//小于135向前	大于135向后	
	 delay_ms(100);
		setAngle_270(3,105);//小于135向前	大于135向后	
	 delay_ms(100);
		setAngle_270(4,135);//小于135顺时针	大于135逆时针
	 delay_ms(100);
		setAngle_180(5,30);// 0 -> 180 = 松到紧
 }
 void RECOVER_XIAO()
 {
		setAngle_180(14,90);
	 delay_ms(100);
		setAngle_180(15,90);
 }
 void PEN_DI()
 {
	 setAngle_270(0,45);
	 delay_ms(100);
	setAngle_270(1,180);
	 delay_ms(100);
	 setAngle_270(3,80);
	 delay_ms(100);
	 setAngle_270(2,80);
	 delay_ms(100);
	setAngle_270(4,45);
	delay_ms(1000);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,1);
	delay_ms(3000);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,0);
 }
 void PEN_KONG()
 {
	 setAngle_270(0,45);
	 delay_ms(100);
	setAngle_270(1,145);
	 delay_ms(100);
	setAngle_270(2,110);
	 delay_ms(100);
	setAngle_270(3,110);
	delay_ms(100);
	setAngle_270(4,225);
	delay_ms(1000);
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,1);
	delay_ms(3000);
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,0);
 }
 
 void leaf_check(void)
 {

	 setAngle_180(15,125);
	 setAngle_180(14,65);
	 
 }