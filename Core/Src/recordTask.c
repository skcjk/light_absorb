#include "recordTask.h"
#include "cmsis_os.h"
#include "SDTask.h"
#include <string.h>
#include <stdlib.h>
#include "adc.h"
#include "rtc.h"

extern osMessageQueueId_t sdCmdQueueHandle;
extern osThreadId_t recordTaskHandle;
extern char WriteBuffer[100];
extern RTC_TimeTypeDef RTC_TimeStruct;  

uint32_t period = 1000;

void RecordTask(void *argument)
{
    uint16_t sd_cmd = SD_WRITE;
    uint32_t ADC_Value;
    vTaskSuspend(NULL); 
    while(1){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
        osDelay(100);
        ADC_Value = read_adc();
        HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);

        sprintf(WriteBuffer, "{\"adcValue\":%d,\"trueVoltage\":%.4f,\"time\":\"%02d:%02d:%02d\"}\r\n", ADC_Value ,ADC_Value*3.3f/4096, RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds); 
        osMessageQueuePut(sdCmdQueueHandle, &sd_cmd, 0, 500);
        if (period > (30 * 1000)) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
        osDelay(period);
    }
}
