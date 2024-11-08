#include "cmd.h"
#include "adc.h"
#include "rtc.h"

extern uint8_t aRxBuffer;	
extern osMessageQueueId_t rxQueueHandle;
extern osMutexId_t printMutexHandle;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct; 

rxStruct receiveRxFromQueneForCmd;

void CMDTask(void *argument)
{

    callback_t callbacks[] = { // 回调函数反射表
        {"sum", sum},
        {"reboot", reboot},
        {"readADC", readADC},
        {"switch12V", switch12V},
        {"time", timeRTC},
    };

    HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);

    while (1)
    {
        if (osOK == osMessageQueueGet(rxQueueHandle, &receiveRxFromQueneForCmd, NULL, osWaitForever))
        { // 从串口接收缓冲区接收数据
            cJSON *root = cJSON_ParseWithLength((char *)receiveRxFromQueneForCmd.rx_buf, (size_t)receiveRxFromQueneForCmd.data_length); // 解析接收信息

            if (root == NULL)
            { // 解析失败
                osMutexAcquire(printMutexHandle, osWaitForever);
                printf("Json Parse error\r\n");
                osMutexRelease(printMutexHandle);
            }
            else
            { // 解析成功
                cJSON *cmdItem = cJSON_GetObjectItem(root, "cmd");
                cJSON *argvItem = cJSON_GetObjectItem(root, "argv");

                if ((cmdItem != NULL) && (argvItem != NULL))
                {
                    for (uint8_t i = 0; i < sizeof(callbacks) / sizeof(callbacks[0]); i++)
                    {
                        if (!strcmp(callbacks[i].name, cJSON_GetStringValue(cmdItem)))
                        {
                            callbacks[i].fn(argvItem);
                        }
                    }
                }
                else // 指令错误
                {
                    osMutexAcquire(printMutexHandle, osWaitForever);
                    printf("Cmd error\r\n");
                    osMutexRelease(printMutexHandle);
                }
            }

            cJSON_Delete(root); // 释放内存
        }
    }
}

void sum(cJSON *root)
{
    double sum;

    cJSON *aItem = cJSON_GetObjectItem(root, "a");
    cJSON *bItem = cJSON_GetObjectItem(root, "b");
    if ((aItem != NULL && cJSON_IsNumber(aItem)) && (bItem != NULL && cJSON_IsNumber(bItem)))
    {
        sum = cJSON_GetNumberValue(aItem) + cJSON_GetNumberValue(bItem);

        cJSON *return_root = cJSON_CreateObject();
        cJSON_AddStringToObject(return_root, "return", "sum");
        cJSON_AddNumberToObject(return_root, "result", sum);
        char *return_str = cJSON_Print(return_root);

        osMutexAcquire(printMutexHandle, osWaitForever);
        printf(return_str);
        printf("\r\n");
        osMutexRelease(printMutexHandle);

        free(return_str); // 释放内存
        cJSON_Delete(return_root);
    }
    else
    {
        osMutexAcquire(printMutexHandle, osWaitForever);
        printf("argv error\r\n");
        osMutexRelease(printMutexHandle);
    }
}

void reboot(cJSON *root){
    osMutexAcquire(printMutexHandle, osWaitForever);
    printf("reboot\r\n");
    osMutexRelease(printMutexHandle);
    osDelay(50);
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
}

void readADC(cJSON *root){
    uint32_t ADC_Value;
    ADC_Value = read_adc();
    osMutexAcquire(printMutexHandle, osWaitForever);
    printf("ADC Reading : %d \r\n",ADC_Value);
    printf("True Voltage value : %.4f \r\n",ADC_Value*3.3f/4096);
    osMutexRelease(printMutexHandle);
}

void switch12V(cJSON *root){
    if (root != NULL && cJSON_IsString(root)){
        if (!strcmp(cJSON_GetStringValue(root), "on")){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
            osMutexAcquire(printMutexHandle, osWaitForever);
            printf("12V on\r\n");
            osMutexRelease(printMutexHandle);
            return;
        }
        if (!strcmp(cJSON_GetStringValue(root), "off")){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
            osMutexAcquire(printMutexHandle, osWaitForever);
            printf("12V off\r\n");
            osMutexRelease(printMutexHandle);
            return;
        }
    }
    osMutexAcquire(printMutexHandle, osWaitForever);
    printf("argv error\r\n");
    osMutexRelease(printMutexHandle);
}

void timeRTC(cJSON *root){
    cJSON *timeItem = cJSON_GetObjectItem(root, "setTime");
    if ((timeItem != NULL && cJSON_IsString(timeItem)))
    {
        char *pt = cJSON_GetStringValue(timeItem);
        BKTime temptimept;
        temptimept.Year = CharToDec(pt, 2);
        pt += 2;
        temptimept.Month = CharToDec(pt, 2);
        pt += 2;
        temptimept.Date = CharToDec(pt, 2);
        pt += 2;
        temptimept.WeekDay = CharToDec(pt, 1);
        pt += 1;
        temptimept.Hours = CharToDec(pt, 2);
        pt += 2;
        temptimept.Minutes = CharToDec(pt, 2);
        pt += 2;
        temptimept.Seconds = CharToDec(pt, 2);
        UISet_Time(temptimept.Hours, temptimept.Minutes, temptimept.Seconds, RTC_HOURFORMAT_24);
        UISet_Date(temptimept.Year, temptimept.Month, temptimept.Date, temptimept.WeekDay);
    }
    HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
    osMutexAcquire(printMutexHandle, osWaitForever);
    printf("%02d/%02d/%02d\r\n",2000 + RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
    printf("%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    printf("\r\n");
    osMutexRelease(printMutexHandle);
}
