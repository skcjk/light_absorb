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
#include "stdio.h"
#include "adc.h"
#include "usart.h"
#include "SDTask.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for cmdTask */
osThreadId_t cmdTaskHandle;
const osThreadAttr_t cmdTask_attributes = {
  .name = "cmdTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for sdTask */
osThreadId_t sdTaskHandle;
const osThreadAttr_t sdTask_attributes = {
  .name = "sdTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for recordTask */
osThreadId_t recordTaskHandle;
const osThreadAttr_t recordTask_attributes = {
  .name = "recordTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for rxQueue */
osMessageQueueId_t rxQueueHandle;
const osMessageQueueAttr_t rxQueue_attributes = {
  .name = "rxQueue"
};
/* Definitions for sdCmdQueue */
osMessageQueueId_t sdCmdQueueHandle;
const osMessageQueueAttr_t sdCmdQueue_attributes = {
  .name = "sdCmdQueue"
};
/* Definitions for printMutex */
osMutexId_t printMutexHandle;
const osMutexAttr_t printMutex_attributes = {
  .name = "printMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void CMDTask(void *argument);
extern void SDTask(void *argument);
extern void RecordTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of printMutex */
  printMutexHandle = osMutexNew(&printMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of rxQueue */
  rxQueueHandle = osMessageQueueNew (3, sizeof(rxStruct), &rxQueue_attributes);

  /* creation of sdCmdQueue */
  sdCmdQueueHandle = osMessageQueueNew (1, sizeof(uint16_t), &sdCmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of cmdTask */
  cmdTaskHandle = osThreadNew(CMDTask, NULL, &cmdTask_attributes);

  /* creation of sdTask */
  sdTaskHandle = osThreadNew(SDTask, NULL, &sdTask_attributes);

  /* creation of recordTask */
  recordTaskHandle = osThreadNew(RecordTask, NULL, &recordTask_attributes);

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

  // /*----------------------- 格式化测�???????????????? ---------------------------*/
  // /* 如果没有文件系统就格式化创建创建文件系统 */
  // if (res_sd == FR_NO_FILESYSTEM)
  // {
  //   printf("》SD has no FILESYSTEM, create FILESYSTEM ing\r\n");
  //   /* 格式�???????????????? */
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
  //   /* 不再读写，关闭文�???????????????? */
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
  // /* 不再读写，关闭文�???????????????? */
  // f_close(&fnew);

  // /* 不再使用文件系统，取消挂载文件系�???????????????? */
  // f_mount(NULL, "0:", 1);

  // // char *test = "test\r\n";
  // HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
  /* Infinite loop */
  for (;;)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

