#ifndef __PCA9685_H
#define __PCA9685_H
#include "sys.h"
#define PCA_Addr 0x80
#define PCA_Model 0x00
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09
#define PCA_Pre 0xFE
 
void PCA9685_Init_270(float hz,u16 angle);

void PCA9685_Init_180(float hz,u16 angle);

void Beng_init();
 
void PCA9685_Write(u8 addr,u8 data);
 
u8 PCA9685_Read(u8 addr);
 
void PCA9685_setPWM(u8 num,u32 on,u32 off);
 
void PCA9685_setFreq(float freq);
 
void setAngle_270(u8 num,u16 angle);

void setAngle_180(u8 num,u16 angle);

void JIAN_CE();
 
void RECOVER_DA();

void RECOVER_XIAO();

void PEN_DE();
	 
void PEN_KONG();

 void leaf_check(void);
#endif