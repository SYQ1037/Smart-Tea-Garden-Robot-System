#ifndef __RS485_H
#define __RS485_H			 
#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//RS485���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
	  		  	
extern u8 RS485_RX_BUF[64]; 		//���ջ���,���64���ֽ�
extern u8 RS485_RX_CNT;   			//���յ������ݳ���

//ģʽ����
#define RS485_TX_EN		PDout(7)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_UART4_RX 	1			//0,������;1,����.

void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);


void RS485_NUM_GET(void);
void wen(void);
void shi(void);
void PH_value(void);
void diaodao_value(void);
void dan_value(void);
void lin_value(void);
void jia_value(void);


extern float real_tem;
extern float real_hum;
extern float real_ph;
extern float real_diandao;
extern float real_dan;
extern float real_lin;
extern float real_jia;


extern int tem_gao;
extern int tem_di;
extern int hum_gao;
extern int hum_di;
extern int ph_gao;
extern int ph_di;
extern int diandao_gao;
extern int diandao_di;

#endif	   
















