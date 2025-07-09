#ifndef _ACTION_H_
#define _ACTION_H_

#include "stm32f4xx.h"
#include "GrayPID.h"
#include "LogSystem.h"
#include "Gray.h"
#include "Servo.h"

typedef enum
{
    Action_Left,
    Action_Right
}ActionDirectionType;

/////////////////////循迹相关定义/////////////////////
typedef enum
{
    TrackState_OFF,
    TrackState_ON
}TrackStateType;

typedef enum
{
    TrackBaseSpeed_UltraSlow=10,    //10
    TrackBaseSpeed_Slow=20,         //20
    TrackBaseSpeed_Normal=25,       //25
    TrackBaseSpeed_Fast=35,         //35
    TrackBaseSpeed_UltraFast=45,    //45
}TrackBaseSpeedType;

typedef enum
{
    TrackMode_Normal,//正常巡线
    TrackMode_Relocate,//原地巡线归中
    TrackMode_TwoRedLine,//巡双红线
    TrackMode_TwoRedLine_Reloacate,//巡双红线归中
    TrackMode_TwoRedLine_lowlevel,//双红线 高敏感度版本
    TrackMode_Normal_Sensitive,//正常巡线 灵敏版本
    TrackMode_AirBrake,
}TrackModeType;

void GrayTrack_Enable(void);
void GrayTrack_Disable(void);
void GrayTrackModeSet(TrackModeType TrackMode_ToSet);
void GrayTrack_Set( int GrayTrackBaseSpeed_ToSet,
                    int GraySarchStartPoint_ToSet,
                    GrayFindDirection GraySarchDirection_ToSet,
                    uint16_t GrayEnable_ToSet,
                    float TargetLinePosition_ToSet);

TrackModeType GrayTrack_ReturnMode(void);
float GrayTrack_GetGrayLinePosition(void);

/////////////////////编码器相关定义/////////////////////

void Action_EncoderADD(int16_t EncoderCountToAdd);
void Action_EncoderCLEAR(void);
void Action_EncoderWait(int EncoderWaitCount);

/////////////////////总体动作相关定义/////////////////////

void Action_JUSTgo(int8_t Speed);
void Action_JUSTgoMs(int8_t Speed,uint16_t Time_ms);
void Action_JUSTgoEncoder(int8_t Speed,int EncoderCount);
void Action_JUSTgoCM(int8_t Speed,int CM);

typedef enum
{
    Fork_Up,
    Fork_Down,
    Fork_Middle
}Fork_StateType;

PausesType Action_WaitForkState(float AngleLevel,Fork_StateType StateToWait);


//提速相关

void Action_TrackEncoder(uint16_t Distance_cm);


void Action_TrackSpeedUp_Time(TrackBaseSpeedType TrackBaseSpeed_ToSet,uint16_t Time_ms);
void Action_TrackSpeedUp_Distance(TrackBaseSpeedType TrackBaseSpeed_ToSet,uint16_t Distance_cm);
void Action_TrackSpeedUp_Sensor(TrackBaseSpeedType TrackBaseSpeed_ToSet,Gray_WhichType Sensor);


extern uint8_t Gray01Data[17];
//void Action_TurnLine(ActionDirectionType Direction, uint8_t WhichGray, uint16_t delaytimes_ms);

/////////////////////舵机相关定义/////////////////////

typedef enum 
{
    Servo_Soil_First_Start = 25,
    Servo_Soil_First_Middle = 10,
    Servo_Soil_First_End = 20,
    Servo_Soil_Second_Start = 5,
    Servo_Soil_Second_Middle,
    Servo_Soil_Second_End = 18
}ServoStateType;//舵机状态



void Action_ServoSetState(ServoWhichType ServoWhich,ServoStateType ServoState);
void Action_Waitforgo(void);

void Action_IronArm_Start(void);
void Action_IronArm_End(void);
///////////////////速度相关定义/////////////////////

#endif
