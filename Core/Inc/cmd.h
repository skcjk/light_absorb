#include <stdarg.h>
#include <stdio.h>
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"

typedef void (*callback)(cJSON *root);

typedef struct
{
    const char *name;
    callback fn;
} callback_t;

void CMDTask(void *argument);

void sum(cJSON *root);
void reboot(cJSON *root);
void readADC(cJSON *root);
void switch12V(cJSON *root);
void timeRTC(cJSON *root);
