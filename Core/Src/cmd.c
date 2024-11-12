#include "cmd.h"
#include "adc.h"
#include "rtc.h"
#include "SDTask.h"

extern osThreadId_t recordTaskHandle;
extern osMessageQueueId_t rxQueueHandle;
extern osMessageQueueId_t sdCmdQueueHandle;
extern osMutexId_t printMutexHandle;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct; 
extern char read_path[16];
extern char delete_path[16];
extern char WriteBuffer[100];
extern uint8_t aRxBuffer;	
extern uint32_t period1;
extern uint32_t period2;
extern uint32_t quantity;
extern void RecordTask(void *argument);
extern const osThreadAttr_t recordTask_attributes;

rxStruct receiveRxFromQueneForCmd;

void CMDTask(void *argument)
{

    callback_t callbacks[] = { // 回调函数反射表
        {"sum", sum},
        {"reboot", reboot},
        {"readADC", readADC},
        {"switch12V", switch12V},
        {"time", timeRTC},
        {"sd", sdCMD},
        {"record", record},
    };
    uint8_t res = NO_SUCH_CMD;

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);

    while (1)
    {
        if (osOK == osMessageQueueGet(rxQueueHandle, &receiveRxFromQueneForCmd, NULL, osWaitForever))
        { // 从串口接收缓冲区接收数据
            cJSON *root = cJSON_ParseWithLength((char *)receiveRxFromQueneForCmd.rx_buf, (size_t)receiveRxFromQueneForCmd.data_length); // 解析接收信息

            if (root == NULL)
            { // 解析失败
                res = JSON_PARSE_ERROR;
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
                            res = callbacks[i].fn(argvItem);
                        }
                    }
                }
                else res = CMD_ERROR;
            }

            osMutexAcquire(printMutexHandle, osWaitForever);
            if (res != JSON_CMD_OK) printf("cmd error: %d\r\n", res);
            else printf("cmd_ok\r\n");
            osMutexRelease(printMutexHandle);
            cJSON_Delete(root); // 释放内存
        }
    }
}

uint8_t sum(cJSON *root)
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

        return JSON_CMD_OK;
    }
    return ARGV_ERROR;
}

uint8_t reboot(cJSON *root){
    osMutexAcquire(printMutexHandle, osWaitForever);
    printf("reboot\r\n");
    osMutexRelease(printMutexHandle);
    osDelay(50);
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
    return JSON_CMD_OK;
}

uint8_t readADC(cJSON *root){
    uint32_t ADC_Value;
    ADC_Value = read_adc();
    osMutexAcquire(printMutexHandle, osWaitForever);
    printf("ADC Reading : %d \r\n",ADC_Value);
    printf("True Voltage value : %.4f \r\n",ADC_Value*3.3f/4096);
    osMutexRelease(printMutexHandle);
    return JSON_CMD_OK;
}

uint8_t switch12V(cJSON *root){
    if (root != NULL && cJSON_IsString(root)){
        if (!strcmp(cJSON_GetStringValue(root), "on")){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
            osMutexAcquire(printMutexHandle, osWaitForever);
            printf("12V on\r\n");
            osMutexRelease(printMutexHandle);
            return JSON_CMD_OK;
        }
        if (!strcmp(cJSON_GetStringValue(root), "off")){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
            osMutexAcquire(printMutexHandle, osWaitForever);
            printf("12V off\r\n");
            osMutexRelease(printMutexHandle);
            return JSON_CMD_OK;
        }
    }
    return ARGV_ERROR;
}

uint8_t timeRTC(cJSON *root){
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
    return JSON_CMD_OK;
}

uint8_t sdCMD(cJSON *root){
    cJSON *optItem = cJSON_GetObjectItem(root, "opt");
    uint8_t res = ARGV_ERROR;
    uint16_t sd_cmd;
    if (optItem != NULL && cJSON_IsString(optItem)){
        char *opt_str = cJSON_GetStringValue(optItem);
        if (!strcmp(opt_str, "ls")){
            sd_cmd = SD_LS;
            res = JSON_CMD_OK;
        }
        if (!strcmp(opt_str, "saveTest")){
            strcpy(WriteBuffer, "test\r\n");
            sd_cmd = SD_WRITE;
            res = JSON_CMD_OK;
        }
        if (!strcmp(opt_str, "delete_all")){
            sd_cmd = SD_DELETE_ALL;
            res = JSON_CMD_OK;
        }
        if (!strcmp(opt_str, "read")){
            cJSON *pathItem = cJSON_GetObjectItem(root, "path");
            if (pathItem != NULL && cJSON_IsString(pathItem)){
                sd_cmd = SD_READ;
                strcpy(read_path, cJSON_GetStringValue(pathItem));
                res = JSON_CMD_OK;
            }
            else res = ARGV_ERROR;
        }
        if (!strcmp(opt_str, "delete")){
            cJSON *pathItem = cJSON_GetObjectItem(root, "path");
            if (pathItem != NULL && cJSON_IsString(pathItem)){
                sd_cmd = SD_DELETE;
                strcpy(delete_path, cJSON_GetStringValue(pathItem));
                res = JSON_CMD_OK;
            }
            else res = ARGV_ERROR;
        }
        
        free(opt_str);
        if (res == JSON_CMD_OK){
            if (osOK != osMessageQueuePut(sdCmdQueueHandle, &sd_cmd, 0, 0)) return OS_ERROR;
            return JSON_CMD_OK;
        } 
    }
    else res = ARGV_ERROR;
    return res;
}

uint8_t record(cJSON *root){
    cJSON *optItem = cJSON_GetObjectItem(root, "opt");
    if (optItem != NULL && cJSON_IsString(optItem)){
        if (!strcmp(cJSON_GetStringValue(optItem), "off")){
            osThreadTerminate(recordTaskHandle);
            return JSON_CMD_OK;
        }
        if (!strcmp(cJSON_GetStringValue(optItem), "on")){
            cJSON *period1Item = cJSON_GetObjectItem(root, "period1");
            cJSON *period2Item = cJSON_GetObjectItem(root, "period2");
            cJSON *quantityItem = cJSON_GetObjectItem(root, "quantity");
            if ((period1Item != NULL && cJSON_IsNumber(period1Item)) && (period2Item != NULL && cJSON_IsNumber(period2Item)) && (quantityItem != NULL && cJSON_IsNumber(quantityItem))){
                period1 = cJSON_GetNumberValue(period1Item);
                period2 = cJSON_GetNumberValue(period2Item);
                quantity = cJSON_GetNumberValue(quantityItem);
                osThreadTerminate(recordTaskHandle);
                recordTaskHandle = osThreadNew(RecordTask, NULL, &recordTask_attributes);
                return JSON_CMD_OK;
            }
        }
    }
    return ARGV_ERROR;
}
