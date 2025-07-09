#include "Motor.h"
#include "LogSystem.h"

//用于保存电机输出
static int8_t Motor_L_Output = 0;
static int8_t Motor_R_Output = 0;
//电机初始化
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);// 
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	 //使能GPIO外设时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8); //GPIOC8复用为定时器8
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8); //GPIOC9复用为定时器8
	
	
	/* GPIO */
	//设置该引脚为复用输出功能,输出TIM8-CH1、2、3、4的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;//TIM8_CH1、2、3、4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);              //初始化PC
	
	/* GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);              //初始化PC
	
	TIM_TimeBaseStructure.TIM_Period = 99; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =167; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//通道映射使能     此处不映射
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低

	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	
    TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE 主输出使能	      高级定时器初始化必须多加此句！！！


	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH3
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH4
	
	TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM8, ENABLE);  //使能TIM8
 
	//初始占空比设为0  防止按下复位键时速度瞬时过大
	//0~100对应占空比0~100%
	TIM_SetCompare3(TIM8,0);
	TIM_SetCompare4(TIM8,0);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)0);
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)0);
	GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)1);
}

/// @brief 直接输出PWM到驱动 仅在速度PID、自检中可见
/// @param Which Motor_L,Motor_R
/// @param Output -90~90的整数
void Motor_SetOuput(Motor_Type Which, int Output)
{
    if(Output>90)
    {
        LogWrite(ERROR_MOTOR_SPEED);
        Output = 90;
    }
    else if(Output<-90)
    {
        LogWrite(ERROR_MOTOR_SPEED);
        Output = -90;
    }

	//清除错误
    LogClear(ERROR_MOTOR_SPEED);

	//0~100对应占空比0~100%，负数反转
	//换算到占空比

    switch(Which)
    {
        case Motor_R:
            if(Output>=0)
            {
								GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)1);
								GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)0);

                TIM_SetCompare4(TIM8,Output);
//                TIM_SetCompare2(TIM8,0);
            }
            else
            {
								GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)0);
								GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)1);
                TIM_SetCompare4(TIM8,Output);
//                TIM_SetCompare2(TIM8,-Output);
            }
			Motor_R_Output = Output;
            break;
        case Motor_L:
            if(Output>=0)
            {
								GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)0);
								GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)1);
                TIM_SetCompare3(TIM8,Output);
//                TIM_SetCompare1(TIM8,Output);
            }
            else
            {
								GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)1);
								GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)0);
                TIM_SetCompare3(TIM8,Output);
//                TIM_SetCompare1(TIM8,0);
            }
			Motor_L_Output = Output;
            break;
    }
}


//读取电机输出 读取占空比
int Motor_ReadOutput(Motor_Type Which)
{
	switch(Which)
	{
		case Motor_R:
			return Motor_R_Output;


		case Motor_L:
			return Motor_L_Output;

		default:
			return 0;

	}

}

void Action_JUSTgo(int8_t SpeedL,int8_t SpeedR)
{
//    GrayTrack_Disable();
//    SpeedPID_SetTarget(Motor_L,Speed);
//    SpeedPID_SetTarget(Motor_R,Speed);
	Motor_SetOuput(Motor_L,SpeedL);
	Motor_SetOuput(Motor_R,SpeedR);
	
}
