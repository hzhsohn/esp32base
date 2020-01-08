#ifndef __USER_WORK_TCPSERV_SERVICE_H__

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

typedef void PF_TCPSERV_READ (const char *buf, int len);

void app_tcpserv_task_accept();
void app_tcpserv_task_recv();


//调用初始化
void  tcpserv_init(PF_TCPSERV_READ*pf, int port);

//发送到客户端
void  tcpserv_send(char* buf,int len);

#define __USER_WORK_TCPSERV_SERVICE_H__
#endif

