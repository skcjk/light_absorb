/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_TimeTypeDef RTC_TimeStruct;  
RTC_DateTypeDef RTC_DateStruct; 

HAL_StatusTypeDef UISet_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeStruct.Hours=hour;
	RTC_TimeStruct.Minutes=min;
	RTC_TimeStruct.Seconds=sec;
	RTC_TimeStruct.TimeFormat=ampm;
	RTC_TimeStruct.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStruct.StoreOperation=RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&hrtc,&RTC_TimeStruct,RTC_FORMAT_BIN);	
}

HAL_StatusTypeDef UISet_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateStruct.Date=date;
	RTC_DateStruct.Month=month;
	RTC_DateStruct.WeekDay=week;
	RTC_DateStruct.Year=year;
	return HAL_RTC_SetDate(&hrtc,&RTC_DateStruct,RTC_FORMAT_BIN);
}

int16_t CharToDec(char *str, uint8_t cnt)
{
  int16_t data = 0;
  uint8_t i;

  if (str[0] == '-')
  {
    for (i = 1; i < cnt; i++)
    {
      data *= 10;
      if (str[i] < '0' || str[i] > '9')
        return 0;
      data += str[i] - '0';
    }
    data = -data;
  }
  else
  {
    for (i = 0; i < cnt; i++)
    {
      data *= 10;
      if (str[i] < '0' || str[i] > '9')
        return 0;
      data += str[i] - '0';
    }
  }
  return data;
}
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)!= 0x5051)
	  {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  RTC_DateStruct = sDate;                                // 把日期数据拷贝到自己定义的data�?
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051); // 向指定的后备域寄存器写入数据
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)RTC_DateStruct.Year);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)RTC_DateStruct.Month);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)RTC_DateStruct.Date);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)RTC_DateStruct.WeekDay);
  }
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
