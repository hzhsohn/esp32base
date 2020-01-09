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

/*

ʹ��:
	
	printf("tcpserv init...port=%d\n",pCfgdata->tcpserv1_port);
	tcpserv_init(tcpserv_read, pCfgdata->tcpserv1_port);	
	xTaskCreate(app_tcpserv_task_accept, "tcpserv_accept", 4*1024, NULL, 5, NULL);
	xTaskCreate(app_tcpserv_task_recv, "tcpserv_recv", 6*1024, NULL, 5, NULL);
	

*/
typedef void PF_TCPSERV_READ (const char *buf, int len);

void app_tcpserv_task_accept();
void app_tcpserv_task_recv();


//���ó�ʼ��
void  tcpserv_init(PF_TCPSERV_READ*pf, int port);

//���͵��ͻ���
void  tcpserv_send(char* buf,int len);

#define __USER_WORK_TCPSERV_SERVICE_H__
#endif

