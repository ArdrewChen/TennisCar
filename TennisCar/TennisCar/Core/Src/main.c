/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdlib.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t RxBuffer[3];
uint16_t Flag ;  //��־λ
uint8_t OpenMV[4]; //openmvģ�����
uint8_t RxFlag;     // openmvģ����ձ�־λ 
float PID;
uint16_t Left_Speed;
uint16_t Right_Speed ;
uint16_t stop =0;
uint16_t cout;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

// С�������˶�
void Car_Go(void );
void Car_Back(void);
void Car_Left(void);
void Car_Right(void);
void Car_Stop(void);
void Car_Turn(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Car_advance(void);
void TIM_SetCompare1(TIM_TypeDef *, int);
void Car_Run(int,int);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim4);//����TIM2��ʱ���ж�
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);//����PWM��
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);//����PWM��
  HAL_UART_Receive_IT(&huart3,(uint8_t*)OpenMV,4); //ʹ�ܽ����ж�
  Flag = 0;        // 0��ʾ���в��Դ��룬1��ʾ��ʽ����
//  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(Flag==1)
        {
        while(1)
        {
            if(HAL_UART_Receive(&huart2,RxBuffer,3,100)==HAL_OK)
            {
                break;
            }
        }
           if(RxBuffer[0]=='F')
              {
                Car_Go();
//             HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET); 
                HAL_Delay(100);
              }
          else if(RxBuffer[0]=='B')
              {
                Car_Back();
                HAL_Delay(100);
              }
          else if(RxBuffer[0]=='L')
              {
                Car_Left();
                HAL_Delay(100);
              }
          else if(RxBuffer[0]=='R')
              {
                Car_Right();
                HAL_Delay(100);
              }
          else if(RxBuffer[0]=='P')
              {
                Car_Stop();
                HAL_Delay(100);
              }
          else
              {
               Car_Stop();
              }
 
      }
    else //���Դ���
        {
            if (RxFlag)
            {
                RxFlag = 0;
                if (OpenMV[0] == 0x6B && OpenMV[3] == 0x6A)
                {
                    Right_Speed = -OpenMV[1]-OpenMV[2];
                    Left_Speed = OpenMV[1]-OpenMV[2];
                    Car_Run(Left_Speed,Right_Speed);
                    cout = 0 ;
                }
                else if (OpenMV[0] == 0x6A && OpenMV[3] == 0x6C)
                {   
                    cout = cout +1;
                    if (cout>15)
                    {
                    Car_Turn();
                    }
                }
            }
         
        }

    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//�ص�����
{
    int PID;
    if (Flag==1){
        PID = 1800 ;
        if(htim==&htim4)
        {
            __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,PID);//(�޸�PIDֵ,�ı�ռ�ձ�)
            __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,PID);
        }
    }

}
void Car_Go(void){
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
}

void Car_Back(void){
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
}

void Car_Left(void){
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
}

void Car_Right(void){
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
}

void Car_Stop(void){
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
}

void Car_Turn(void){
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
}
void Car_Run(left_speed, right_speed){
      if(left_speed>0)
      {
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
      }
      else
      {
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET); //SET����ߵ�ƽ��RESET����͵�ƽ
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
      }
      left_speed = abs(left_speed);
      __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,left_speed);
      left_speed =0 ;
      if(right_speed>0)
      {
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
      }
      else
      {
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); 
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
      }
      right_speed = abs(right_speed);
      __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,right_speed);
      right_speed = 0 ;

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
        if(huart ->Instance == USART3)
        {
            RxFlag = 1;
            HAL_UART_Receive_IT(&huart3,(uint8_t*)OpenMV, 4);
        }

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
