#include "SDTask.h"
#include "rtc.h"
#include <string.h>
#include "stdio.h"

FATFS fs;                         /* FatFs文件系统对象 */                         /* 文件对象 */
// FRESULT res_sd;                /* 文件操作结果 */
// UINT fnum;                        /* 文件成功读写数量 */
// BYTE ReadBuffer[1024]= {0};       /* 读缓冲区 */
// BYTE bpData[512] =  {0};   
char WriteBuffer[100]= {0};
char read_path[16] = {0};
char delete_path[16] = {0};

extern osMessageQueueId_t sdCmdQueueHandle;
extern osMutexId_t printMutexHandle;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct; 

void SDTask(void *argument)
{
    uint16_t sd_cmd;
    FRESULT sd_res;
    sd_res = f_mount(&fs, "0:", 1);
    while(1)
    {
        if (osOK == osMessageQueueGet(sdCmdQueueHandle, &sd_cmd, NULL, osWaitForever))
        {
            switch (sd_cmd)
            {
            case SD_LS:
                sd_res = list_dir("0:");
                break;
            case SD_WRITE:
                sd_res = save_data();
                break;
            case SD_READ:
                sd_res = read_data();
                break;
            case SD_DELETE_ALL:
                sd_res = delete_all_files("0:");
                break;
            case SD_DELETE:
                sd_res = f_unlink(delete_path);
                break;
            }
            osMutexAcquire(printMutexHandle, osWaitForever);
            if (sd_res != FR_OK) printf("fatfs error: %d\r\n", sd_res);
            osMutexRelease(printMutexHandle);
        }
    }
}

/* List contents of a directory */

FRESULT list_dir(const char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Error or end of dir */
            if (fno.fattrib & AM_DIR)
            { /* Directory */
                osMutexAcquire(printMutexHandle, osWaitForever);
                printf("   <DIR>   %s\r\n", fno.fname);
                osMutexRelease(printMutexHandle);
                ndir++;
            }
            else
            { /* File */
                osMutexAcquire(printMutexHandle, osWaitForever);
                printf("file_size:%10u file_name:%s\r\n", fno.fsize, fno.fname);
                osMutexRelease(printMutexHandle);
                nfile++;
            }
        }
        f_closedir(&dir);
        osMutexAcquire(printMutexHandle, osWaitForever);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
        osMutexRelease(printMutexHandle);
    }
    return res;
}

FRESULT save_data(void)
{
    FIL fp;
    char path[16];
    FRESULT res;
    UINT fnum;                        /* 文件成功读写数量 */

    HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
    sprintf(path, "%02d_%02d_%02d.txt\r\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);

    res = f_open(&fp, path, FA_WRITE | FA_OPEN_APPEND);
    if (res) return res;

    res = f_write(&fp, WriteBuffer, strlen(WriteBuffer), &fnum);
    f_close(&fp);

    return res;
}

FRESULT read_data(void)
{
    FIL fp;
    FRESULT res;
    char line[100]={0}; /* Line buffer */

    /* Open a text file */
    res = f_open(&fp, read_path, FA_READ);
    if (res) return res;

    /* Read every line and display it */
    osMutexAcquire(printMutexHandle, osWaitForever);
    while (f_gets(line, sizeof(line), &fp) != NULL){
        printf(line);
    }
    osMutexRelease(printMutexHandle);

    /* Close the file */
    f_close(&fp);

    return res;
}

FRESULT delete_all_files(const char *path) 
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Error or end of dir */
            if (fno.fattrib & AM_DIR)
            { /* Directory */
            }
            else
            { /* File */
                char path[16];
                snprintf(path, sizeof(path), "0:%s", fno.fname);
                res = f_unlink(path);
                if (res != FR_OK) break;
                osMutexAcquire(printMutexHandle, osWaitForever);
                printf("delete: file_size:%10u file_name:%s\r\n", fno.fsize, fno.fname);
                osMutexRelease(printMutexHandle);
                nfile++;
            }
        }
        f_closedir(&dir);
        osMutexAcquire(printMutexHandle, osWaitForever);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
        osMutexRelease(printMutexHandle);
    }
    return res;
}
