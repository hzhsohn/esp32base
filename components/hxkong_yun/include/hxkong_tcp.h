/* tcp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

typedef void PF_HXKONG_DTRS_RECV (const char *buf, int len);
extern PF_HXKONG_DTRS_RECV*	g_dtrs_sys_recv;

typedef struct _ThxtLinkInfo
{
	char host[255];
	int port;
}ThxtLinkInfo;

//send data
void hxtcp_send(char* databuff, int len);
//
int hxtcp_check_working_socket();
void hxtcp_close_socket();
//
void hxtcp_conn();