#ifndef __USER_WORK_WEBSOCK_SERVICE_H__

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

typedef void PF_WEBSOCKET_READ (const char *buf, int len);

void app_websocket_task_accept();
void app_websocket_task_recv();


//调用初始化
void  websocket_init(PF_WEBSOCKET_READ*pf, int port);

//发送到客户端
void  websocket_send(char* buf,int len);

#define __USER_WORK_WEBSOCK_SERVICE_H__
#endif

