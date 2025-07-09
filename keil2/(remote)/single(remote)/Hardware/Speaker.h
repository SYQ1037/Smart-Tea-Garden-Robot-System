#ifndef _SPEAKER_H
#define _SPEAKER_H

#include "stm32f4xx.h"

#define IO0_GPIO GPIOG
#define IO1_GPIO GPIOG
#define IO2_GPIO GPIOG
#define IO3_GPIO GPIOG
#define IO4_GPIO GPIOG
#define IO5_GPIO GPIOG
#define IO6_GPIO GPIOG
#define IO7_GPIO GPIOE

#define IO0_GPIO_Pin GPIO_Pin_8
#define IO1_GPIO_Pin GPIO_Pin_7
#define IO2_GPIO_Pin GPIO_Pin_6
#define IO3_GPIO_Pin GPIO_Pin_5
#define IO4_GPIO_Pin GPIO_Pin_4
#define IO5_GPIO_Pin GPIO_Pin_3
#define IO6_GPIO_Pin GPIO_Pin_2
#define IO7_GPIO_Pin GPIO_Pin_11

typedef enum
{
    SONG_BEGIN =1,
	SONG_Plateform1 = 2,
    SONG_Plateform2 = 3,
    SONG_Plateform3 = 4,
    SONG_Plateform4 = 5,
    SONG_Plateform5 = 6,
    SONG_Plateform6 = 7,
    SONG_Plateform7 = 8,
    SONG_Plateform8 = 9,
    SONG_UprightAttractions=10,
}SongType;

void Speaker_Init(void);
void Speaker_Play(SongType Which_Song);



#endif
