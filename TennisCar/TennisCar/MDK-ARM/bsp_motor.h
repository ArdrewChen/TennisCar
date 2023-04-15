#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include "stm32f10x.h"

#define Motor_RCC		 RCC_APB2Periph_GPIOB
#define Motor_Port		 GPIOB
#define Left_MotoA_Pin 	 GPIO_Pin_9
#define Left_MotoB_Pin 	 GPIO_Pin_8

#define Right_MotoA_Pin	 GPIO_Pin_4
#define Right_MotoB_Pin  GPIO_Pin_5



void Motor_PWM_Init(u16 arr, u16 psc, u16 arr2, u16 psc2);


#endif