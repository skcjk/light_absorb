/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "fatfs.h"
#include "stdio.h"
#include "adc.h"
#define RXBUFFERSIZE  256     //�????大接收字节数
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char RxBuffer[RXBUFFERSIZE];   //接收数据
uint8_t aRxBuffer;			//接收中断缓冲
uint8_t Uart1_Rx_Cnt = 0;		//接收缓冲计数
extern UART_HandleTypeDef huart1;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
FATFS fs;                         /* FatFs文件系统对象 */
FIL fnew;                         /* 文件对象 */
FRESULT res_sd;                /* 文件操作结果 */
UINT fnum;                        /* 文件成功读写数量 */
BYTE ReadBuffer[1024]= {0};       /* 读缓冲区 */
BYTE bpData[512] =  {0};   
BYTE WriteBuffer[] =   "test\r\n";         /* 写缓冲区*/
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  // printf("\r\n****** SD test ******\r\n");

  // res_sd = f_mount(&fs, "0:", 1);

  // /*----------------------- 格式化测�???? ---------------------------*/
  // /* 如果没有文件系统就格式化创建创建文件系统 */
  // if (res_sd == FR_NO_FILESYSTEM)
  // {
  //   printf("》SD has no FILESYSTEM, create FILESYSTEM ing\r\n");
  //   /* 格式�???? */
  //   res_sd = f_mkfs("0:", FM_FAT32, 512, bpData, 512);
  //   if (res_sd == FR_OK)
  //   {
  //     printf("create FILESYSTEM ok\r\n");
  //     /* 格式化后，先取消挂载 */
  //     res_sd = f_mount(NULL, "0:", 1);
  //     /* 重新挂载 */
  //     res_sd = f_mount(&fs, "0:", 1);
  //   }
  //   else
  //   {
  //     printf("create FILESYSTEM error\r\n");
  //     while (1)
  //       ;
  //   }
  // }
  // else if (res_sd != FR_OK)
  // {
  //   printf("mount error(%d)\r\n", res_sd);
  //   while (1)
  //     ;
  // }
  // else
  // {
  //   printf("mount ok\r\n");
  // }

  // /*--------------------- 文件系统测试：写测试 -----------------------*/
  // /* 打开文件，如果文件不存在则创建它 */
  // printf("\r\n****** read and write test ******\r\n");
  // res_sd = f_open(&fnew, "0:FatFs_read_and_write.txt", FA_CREATE_ALWAYS | FA_WRITE);
  // if (res_sd == FR_OK)
  // {
  //   printf("open or creat FatFs_read_and_write.txt ok\r\n");
  //   /* 将指定存储区内容写入到文件内 */
  //   res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
  //   if (res_sd == FR_OK)
  //   {
  //     printf("write_ok:%d\n", fnum);
  //     printf("write_data is:\r\n%s\r\n", WriteBuffer);
  //   }
  //   else
  //   {
  //     printf("write_error(%d)\n", res_sd);
  //   }
  //   /* 不再读写，关闭文�???? */
  //   f_close(&fnew);
  // }
  // else
  // {
  //   printf("open or creat FatFs_read_and_write.txt error:%d\r\n", res_sd);
  // }

  // /*------------------ 文件系统测试：读测试 --------------------------*/
  // printf("****** read test ******\r\n");
  // res_sd = f_open(&fnew, "0:FatFs_read_and_write.txt", FA_OPEN_EXISTING | FA_READ);
  // if (res_sd == FR_OK)
  // {
  //   printf("open ok\r\n");
  //   res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
  //   if (res_sd == FR_OK)
  //   {
  //     printf("read byte:%d\r\n", fnum);
  //     printf("data is:\r\n%s \r\n", ReadBuffer);
  //   }
  //   else
  //   {
  //     printf("read error:(%d)\n", res_sd);
  //   }
  // }
  // else
  // {
  //   printf("open error\r\n");
  // }
  // /* 不再读写，关闭文�???? */
  // f_close(&fnew);

  // /* 不再使用文件系统，取消挂载文件系�???? */
  // f_mount(NULL, "0:", 1);

  // // char *test = "test\r\n";
  // HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
  uint32_t ADC_Value;
  /* Infinite loop */
  for (;;)
  {
    HAL_ADC_Start(&hadc1);  
    HAL_ADC_PollForConversion(&hadc1, 50);
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
    {
      ADC_Value = HAL_ADC_GetValue(&hadc1);   //获取AD值

      printf("ADC1 Reading : %d \r\n",ADC_Value);
      printf("PA3 True Voltage value : %.4f \r\n",ADC_Value*3.3f/4096);
    }
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */

  if (Uart1_Rx_Cnt >= 255) //
  {
    Uart1_Rx_Cnt = 0;
    memset(RxBuffer, 0x00, sizeof(RxBuffer));
    HAL_UART_Transmit(&huart1, (uint8_t *)"数据溢出", 10, 0xFFFF);
  }
  else
  {
    RxBuffer[Uart1_Rx_Cnt++] = aRxBuffer; //

    if ((RxBuffer[Uart1_Rx_Cnt - 1] == 0x0A) && (RxBuffer[Uart1_Rx_Cnt - 2] == 0x0D)) //
    {
      HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt, 0xFFFF); //
      while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX)
        ; //
      Uart1_Rx_Cnt = 0;
      memset(RxBuffer, 0x00, sizeof(RxBuffer)); //
    }
  }

  HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1); //
}
/* USER CODE END Application */

