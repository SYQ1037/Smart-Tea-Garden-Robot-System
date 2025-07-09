#include "Action.h"
#include "LogSystem.h"
#include "GrayPID.h"
#include "SpeedPID.h"
#include "MPU6050.h"
#include "Delay.h"
#include "Servo.h"
#include "HC05.h"
#include "oled.h"
#include "Turn.h"
#include "Beep.h"
#include "vl53l0x_gen.h"
/////////////循迹动作/////////////
//循迹状态
TrackStateType TrackState;
//循迹模式
static TrackModeType TrackMode;
//循迹时的基础速度
static TrackBaseSpeedType GrayTrackBaseSpeed = TrackBaseSpeed_Slow;
//开始搜索的位置
static int GraySarchStartPoint;
//搜索方向
static GrayFindDirection GraySarchDirection;
//允许的灰度传感器
static uint16_t GrayEnable;
//目标线位置
static float TargetLinePosition;

extern uint8_t GoDirection_Flag;//行进方向标志位 0正着走 1倒着走

//当前线的位置
static float GrayLinePosition;
static float GrayLinePosition_FreshedFlag;

/// @brief 设置循迹参数
/// @param BaseSpeed    循迹时的基础速度
/// @param StartPoint   开始搜索的位置(灰度头)
/// @param Direction    搜索方向
/// @param Enable       允许的灰度传感器1代表允许 0代表不允许 如：1111 0000 1111 0000代表允许最左侧四路和右侧前四路
/// @param TargetLinePosition 目标线位置(参数为0时表示归中 -8~8)
void GrayTrack_Set( int GrayTrackBaseSpeed_ToSet,
                    int GraySarchStartPoint_ToSet,
                    GrayFindDirection GraySarchDirection_ToSet,
                    uint16_t GrayEnable_ToSet,
                    float TargetLinePosition_ToSet)
{
    GrayTrackBaseSpeed      =   GrayTrackBaseSpeed_ToSet;
    GraySarchStartPoint     =   GraySarchStartPoint_ToSet;
    GraySarchDirection      =   GraySarchDirection_ToSet;
    GrayEnable              =   GrayEnable_ToSet;
    TargetLinePosition      =   TargetLinePosition_ToSet;

    switch (GrayTrackBaseSpeed)
    {
    case TrackBaseSpeed_UltraSlow:
        // GrayPID_Change(3.0f,0.0f);
        break;
    case TrackBaseSpeed_Slow:
        GrayPID_Change(3.0f,0.0f);
        break;
    case TrackBaseSpeed_Normal:
        GrayPID_Change(3.0f,0.0f);
        break;
    case TrackBaseSpeed_Fast:
        GrayPID_Change(3.0f,0.0f);
        break;
    case TrackBaseSpeed_UltraFast:      
        GrayPID_Change(3.0f,0.0f);
        break;
    default:
        // GrayPID_Change(0.0f,0.0f);
        break;
    }

    if((TrackMode==TrackMode_TwoRedLine)||(TrackMode==TrackMode_TwoRedLine_lowlevel))
    {
        switch (GrayTrackBaseSpeed)
        {
        case TrackBaseSpeed_Slow:
            // GrayPID_Change(6.0f,3.0f);
            break;
        case TrackBaseSpeed_Normal:
            // GrayPID_Change(10.0f,3.0f);
            break;
        default:
            // GrayPID_Change(0.0f,0.0f);
            break;
        }
    }
}

/// @brief 内部函数 仅修改循迹基准速度
/// @param GrayTrackBaseSpeed_ToSet 要设置的基准速度
void GrayTrackSpeedSet(TrackBaseSpeedType GrayTrackBaseSpeed_ToSet)
{
    GrayTrackBaseSpeed = GrayTrackBaseSpeed_ToSet;

    switch (GrayTrackBaseSpeed)
    {
    case TrackBaseSpeed_UltraSlow:
        // GrayPID_Change(7.5f,0.0f);
        break;
    case TrackBaseSpeed_Slow:
        // GrayPID_Change(9.0f,2.5f);
        break;
    case TrackBaseSpeed_Normal:
        // GrayPID_Change(7.5f,0.0f);
        break;
    case TrackBaseSpeed_Fast:
        // GrayPID_Change(7.0f,2.0f);
        break;
    case TrackBaseSpeed_UltraFast:      
        // GrayPID_Change(5.0f,15.0f);
        break;
    default:
        // GrayPID_Change(0.0f,0.0f);
        break;
    }
}

/// @brief 设置循迹模式
/// @param TrackMode_ToSet 正常循迹：TrackMode_Normal 原地巡线归中（已预设参数，不需要填写set）：TrackMode_Relocate 巡双红线：TrackMode_TwoRedLine 高精度巡线：TrackMode_HighPrecision
void GrayTrackModeSet(TrackModeType TrackMode_ToSet)
{
    TrackMode = TrackMode_ToSet;
    switch (TrackMode)
    {
    case TrackMode_Normal:
        break;
    case TrackMode_Relocate:
        GrayPID_Change(3.0f,0.0f);
        break;
    case TrackMode_TwoRedLine:  
        GrayPID_ClearLastOutput();
        break;
    case TrackMode_TwoRedLine_Reloacate:
        GrayPID_ClearLastOutput();
        break;
    case TrackMode_Normal_Sensitive:
        break;
    case TrackMode_AirBrake:
        GrayPID_Change(6.0f,0.0f);
        break;
    default:
        break;
    }
}

/// @brief 返回循迹模式
/// @return TrackModeType
TrackModeType GrayTrack_ReturnMode(void)
{
    return TrackMode;
}

//开启循迹
void GrayTrack_Enable(void)
{
    TrackState = TrackState_ON;
    Beep_Off();
}

//关闭循迹 注意其会设置速度为0
void GrayTrack_Disable(void)
{
    TrackState = TrackState_OFF;
    SpeedPID_SetTarget(Motor_L,0);
    SpeedPID_SetTarget(Motor_R,0);
    Beep_On();
}

//内部函数 用于正常循迹
void GrayTrack_Normal(void)
{
    float GrayLine=GrayPID_FindLine(GraySarchStartPoint,GraySarchDirection,GrayEnable);
    float GrayOutput=GrayPID_Calculation(GrayLine,TargetLinePosition);

    GrayLinePosition = GrayLine;
    GrayLinePosition_FreshedFlag = 1;
    
    //限幅
    if(GrayOutput>30)
    {
        GrayOutput = 30;
    }
    else if(GrayOutput<-30)
    {
        GrayOutput = -30;
    }

    if(GoDirection_Flag==1)
    {
        //输出到速度环
        SpeedPID_SetTarget(Motor_L,GrayTrackBaseSpeed+GrayOutput);
        SpeedPID_SetTarget(Motor_R,GrayTrackBaseSpeed-GrayOutput);
    }
    else
    {
        //输出到速度环
        SpeedPID_SetTarget(Motor_L,GrayTrackBaseSpeed-GrayOutput);
        SpeedPID_SetTarget(Motor_R,GrayTrackBaseSpeed+GrayOutput);
    }
    

    HC05_Printf("Line:%f \n",GrayLinePosition);
}

float compute_diffabs(float a,float b)
{
    if(a>b)
    {
        return a-b;
    }
    else
    {
        return b-a;
    }
}

//内部函数 用于正常循迹 线过多 拒绝输入版本
void GrayTrack_ForSensitive(void)
{
    float GrayLine=GrayPID_FindLine(GraySarchStartPoint,GraySarchDirection,GrayEnable);
    float GrayOutput=GrayPID_Calculation(GrayLine,TargetLinePosition);

    GrayLinePosition = GrayLine;
    GrayLinePosition_FreshedFlag = 1;
    
    //限幅
    if(GrayOutput>30)
    {
        GrayOutput = 30;
    }
    else if(GrayOutput<-30)
    {
        GrayOutput = -30;
    }

    uint8_t check=1;
    uint8_t count=0;

    for(check=1;check<=16;check++)
    {
        if(Gray01Data[check]==1)
        {
            count++;
        }
    }

    if(count>=6)
    {
        GrayOutput=0;
        HC05_Printf("TooManyLines\n");
    }

    //输出到速度环
    SpeedPID_SetTarget(Motor_L,GrayTrackBaseSpeed-GrayOutput);
    SpeedPID_SetTarget(Motor_R,GrayTrackBaseSpeed+GrayOutput);
}

//内部函数 用于原地巡线归中
void GrayTrack_Relocate(void)
{
    float GrayLine_L=GrayPID_FindLine(6,FindDirection_Left,0xfff);
    float GrayLine_R=GrayPID_FindLine(7,FindDirection_Right,0xfff);
    float GrayLine_ToChase=0;

    GrayLinePosition = GrayLine_ToChase;
    GrayLinePosition_FreshedFlag = 1;

    //寻找最近的线
    if(compute_diffabs(GrayLine_L,0)<=compute_diffabs(GrayLine_R,0))
    {
        GrayLine_ToChase = GrayLine_L;
    }
    else
    {
        GrayLine_ToChase = GrayLine_R;
    }
    
    float GrayOutput=GrayPID_Calculation(GrayLine_ToChase,0);
    
    //由于没有基础速度 所以不需要放大或缩小输出
    //限幅 力求迅速归中
    if(GrayOutput>30)
    {
        GrayOutput = 30;
    }
    else if(GrayOutput<-30)
    {
        GrayOutput = -30;
    }

    //输出到速度环
    SpeedPID_SetTarget(Motor_L,-GrayOutput);
    SpeedPID_SetTarget(Motor_R,GrayOutput);
}

//内部函数 用于巡双红线
void GrayTrack_Seesaw(void)
{
    //找到左右两侧的线
    float GrayLine_L=GrayPID_FindLine(8,FindDirection_Left,GrayEnable);
    float GrayLine_R=GrayPID_FindLine(9,FindDirection_Right,GrayEnable);

    float GrayOutput=0;

    //正常情况下两边都丢线
    if((GrayLine_L==Gray_MissLine)&&(GrayLine_R==Gray_MissLine))
    {
        GrayOutput=0;
    }
    else
    {
        //如果有输出 看那边有线
        if(GrayLine_L!=Gray_MissLine)//左侧有线
        {
            GrayOutput=GrayPID_Calculation(GrayLine_L,-8.5); 
        }
        else if(GrayLine_R!=Gray_MissLine)//右侧有线
        {
            GrayOutput=GrayPID_Calculation(GrayLine_R,8.5);
        }
    }

    //限幅
    if(GrayOutput>40)
    {
        GrayOutput = 40;
    }
    else if(GrayOutput<-40)
    {
        GrayOutput = -40;
    }

    //输出到速度环
    SpeedPID_SetTarget(Motor_L,GrayTrackBaseSpeed-GrayOutput);
    SpeedPID_SetTarget(Motor_R,GrayTrackBaseSpeed+GrayOutput);

    // HC05_Printf("L:%f,R:%f,O:%f\n LS:%f RS:%f\n",GrayLine_L,GrayLine_R,GrayOutput,SpeedPID_GetTarget(Motor_L),SpeedPID_GetTarget(Motor_R));
}

//内部函数 用于归中双红线
void GrayTrack_SeesawRelocate(void)
{
    //找到左右两侧的线
    float GrayLine_L=GrayPID_FindLine(8,FindDirection_Left,GrayEnable);
    float GrayLine_R=GrayPID_FindLine(9,FindDirection_Right,GrayEnable);

    float GrayOutput=0;

    //正常情况下两边都丢线
    if((GrayLine_L==Gray_MissLine)&&(GrayLine_R==Gray_MissLine))
    {
        GrayOutput=0;
    }
    else
    {
        //如果有输出 看那边有线
        if(GrayLine_L!=Gray_MissLine)//左侧有线
        {
            GrayOutput=GrayPID_Calculation(GrayLine_L,-10); 
        }
        else if(GrayLine_R!=Gray_MissLine)//右侧有线
        {
            GrayOutput=GrayPID_Calculation(GrayLine_R,10); 
        }
    }

    //限幅
    if(GrayOutput>40)
    {
        GrayOutput = 40;
    }
    else if(GrayOutput<-40)
    {
        GrayOutput = -40;
    }

    //输出到速度环
    SpeedPID_SetTarget(Motor_L,-GrayOutput);
    SpeedPID_SetTarget(Motor_R,GrayOutput);

}

//循迹控制 它的输出是速度环的输入
void GrayTrack_Control(void)
{
    if(TrackState==TrackState_ON)
    {
        switch(TrackMode)
        {
            case TrackMode_Normal:
                GrayPID_Get01State();
                GrayTrack_Normal();
                break;
           
            case TrackMode_Relocate:
                GrayPID_Get01State();
                GrayTrack_Relocate();
                break;

            case TrackMode_TwoRedLine:
                GrayPID_Get01State();
                GrayTrack_Seesaw();
                break;

            case TrackMode_TwoRedLine_lowlevel:
                GrayPID_Get01StateForSeesaw();
                GrayTrack_Seesaw();
                break;

            case TrackMode_TwoRedLine_Reloacate:
                GrayPID_Get01StateForSeesaw();
                GrayTrack_SeesawRelocate();
                break;

            case TrackMode_Normal_Sensitive:
                GrayPID_Get01State_Sensitive();
                GrayTrack_ForSensitive();
                break;

            case TrackMode_AirBrake:
                GrayPID_Get01State();
                GrayTrack_Normal();
                break;

            default:
                
                break;
        }
    }
    else
    {
        GrayPID_Get01State();
    }
}

/// @brief 获取一次灰度线的位置 注意此函数会阻塞主函数
/// @param  void
/// @return 返回低精度 普通 高精度 原地 四个模式运行时 最新的灰度线位置
float GrayTrack_GetGrayLinePosition(void)
{
    GrayLinePosition_FreshedFlag = 0;
    while(GrayLinePosition_FreshedFlag==0);
    return GrayLinePosition;
}

////////////////////////编码器动作////////////////////////

static int EncoderCount;
uint8_t EncoderWaitState;//1:等待 0:完成

/// @brief 增加/减小 编码器累计计数
void Action_EncoderADD(int16_t EncoderCountToAdd)
{
    EncoderCount += EncoderCountToAdd;
}

/// @brief 清除编码器累计计数
void Action_EncoderCLEAR(void)
{
    EncoderCount = 0;
}

/// @brief 主函数在此函数中停留 等待编码器累计计数到达指定值
void Action_EncoderWait(int EncoderWaitCount)
{
    Action_EncoderCLEAR();
    EncoderWaitState=1;
    if(EncoderWaitCount<0)
    {
        while(EncoderCount>EncoderWaitCount);
        EncoderWaitState=0;
    }
    else
    {
        while(EncoderCount<EncoderWaitCount);
        EncoderWaitState=0;
    }
}

////////////////////////总体动作////////////////////////

/// @brief 盲走函数
/// @param Speed 
/// @param Time_ms 
void Action_JUSTgo(int8_t Speed)
{
//    GrayTrack_Disable();
//    SpeedPID_SetTarget(Motor_L,Speed);
//    SpeedPID_SetTarget(Motor_R,Speed);
	Motor_SetOuput(Motor_L,Speed);
	Motor_SetOuput(Motor_R,Speed);
	
}


/// @brief 在此函数中前进一段时间后速度设为0
/// @param Speed 设定的前进速度
/// @param Time_ms 设定的前进时间
void Action_JUSTgoMs(int8_t Speed,uint16_t Time_ms)
{
    GrayTrack_Disable();
    SpeedPID_SetTarget(Motor_L,Speed);
    SpeedPID_SetTarget(Motor_R,Speed);
    delay_ms(Time_ms);
    SpeedPID_SetTarget(Motor_L,0);
    SpeedPID_SetTarget(Motor_R,0);
}

/// @brief 在此函数中停留 等待编码器累计计数到达指定值 结束后速度设为0
/// @param Speed 设定的前进速度
/// @param EncoderCount 设定的指定编码器计数
void Action_JUSTgoEncoder(int8_t Speed,int EncoderCount)
{
    GrayTrack_Disable();
    SpeedPID_SetTarget(Motor_L,Speed);
    SpeedPID_SetTarget(Motor_R,Speed);
    Action_EncoderWait(EncoderCount);
    SpeedPID_SetTarget(Motor_L,0);
    SpeedPID_SetTarget(Motor_R,0);
}

/// @brief 在此函数中停留 等待前进或后退足够距离后结束 结束后速度设为0
/// @param Speed 
/// @param CM 指定距离 单位cm
void Action_JUSTgoCM(int8_t Speed,int CM)
{
    if(GoDirection_Flag==1)
    {
        CM=-CM;
    }
    GrayTrack_Disable();
    SpeedPID_SetTarget(Motor_L,Speed);
    SpeedPID_SetTarget(Motor_R,Speed);
    Action_EncoderWait(CM*83);
    SpeedPID_SetTarget(Motor_L,0);
    SpeedPID_SetTarget(Motor_R,0);
}

/// @brief 保持原先状态 前进一段距离后退出
/// @param Distance_cm 前进的距离 单位cm
void Action_TrackEncoder(uint16_t Distance_cm)
{
    if(GoDirection_Flag==1)
    {
        Action_EncoderWait(-Distance_cm*83);
    }
    else
    {
        Action_EncoderWait(Distance_cm*83);
    }
    
}

/// @brief 加速到指定速度 持续一段时间后恢复原速度
/// @param TrackBaseSpeed_ToSet 要设置的基准速度
/// @param Time_ms 持续时间
void Action_TrackSpeedUp_Time(TrackBaseSpeedType TrackBaseSpeed_ToSet,uint16_t Time_ms)
{
    TrackBaseSpeedType oirginalSpeed = GrayTrackBaseSpeed;
    GrayTrackSpeedSet(TrackBaseSpeed_ToSet);
    delay_ms(Time_ms);
    GrayTrackSpeedSet(oirginalSpeed);
}

/// @brief 加速到指定速度 持续一段距离后恢复原速度
/// @param TrackBaseSpeed_ToSet 要设置的基准速度
/// @param Distance_cm 持续距离
void Action_TrackSpeedUp_Distance(TrackBaseSpeedType TrackBaseSpeed_ToSet,uint16_t Distance_cm)
{
    TrackBaseSpeedType oirginalSpeed = GrayTrackBaseSpeed;
    GrayTrackSpeedSet(TrackBaseSpeed_ToSet);
    Action_EncoderWait(Distance_cm*83);
    GrayTrackSpeedSet(oirginalSpeed);
}

/// @brief 加速到指定速度 指定传感器检测到后恢复原速度
/// @param TrackBaseSpeed_ToSet 要设置的基准速度
/// @param Sensor 指定传感器
void Action_TrackSpeedUp_Sensor(TrackBaseSpeedType TrackBaseSpeed_ToSet,Gray_WhichType Sensor)
{
    TrackBaseSpeedType oirginalSpeed = GrayTrackBaseSpeed;
    GrayTrackSpeedSet(TrackBaseSpeed_ToSet);
    GrayPacked_Wait(1,Sensor);
    GrayTrackSpeedSet(oirginalSpeed);
}

////////////////////////舵机动作////////////////////////

/// @brief 更改舵机状态
/// @param ServoWhich 指定的舵机     Servo_Soil_first,Servo_Soil_second
/// @param ServoState 指定的状态     Servo_Soil_First_Start,Servo_Soil_First_End,Servo_Soil_Second_Start,Servo_Soil_Second_End
void Action_ServoSetState(ServoWhichType ServoWhich,ServoStateType ServoState)
{
    if(ServoWhich==Servo_Soil_first)
    {
        if(ServoState==Servo_Soil_First_Start)
        {
            Servo_SetDuty(Servo_Soil_first,Servo_Soil_First_Start);
        }
        else
        {
            Servo_SetDuty(Servo_Soil_first,Servo_Soil_First_End);
        }
    }
    else
    {
        if(ServoState==Servo_Soil_Second_Start)
        {
            Servo_SetDuty(Servo_Soil_second,Servo_Soil_Second_Start);
        }
        else
        {
            Servo_SetDuty(Servo_Soil_second,Servo_Soil_Second_End);
        }
    }

}

void Action_IronArm_Start(void)
{

  
     Action_ServoSetState(Servo_Soil_first, Servo_Soil_First_End); 

    delay_ms(6000);
	Action_ServoSetState(Servo_Soil_second, Servo_Soil_Second_End);
}

void Action_IronArm_End(void)
{

  
	Action_ServoSetState(Servo_Soil_second, Servo_Soil_Second_Start); 
   	delay_ms(6000);
		Action_ServoSetState(Servo_Soil_first, Servo_Soil_First_Start);



}


#define ForkUP_HighSensitive 20
#define ForkDown_HighSensitive 20
#define ForkUP_LowSensitive 40
#define ForkDown_LowSensitive 40
