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

#include <lwip/opt.h>
#include <lwip/api.h>
#include <lwip/dns.h>
#include <lwip/ip.h>
#include <stddef.h> 
#include <lwip/netbuf.h>
#include <lwip/sys.h>
#include <lwip/ip_addr.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>

#include "hxkong_tcp.h"
#include "event.h"

#define TAG		"hxkong_tcp"

PF_HXKONG_DTRS_RECV*	g_dtrs_sys_recv=NULL;
extern ThxtLinkInfo g_thxlinkinfo;

/*socket*/
static struct sockaddr_in server_addr;
static int connect_socket = 0;

//函数定义
esp_err_t create_tcp_client(char* SERVER_HOST,int SERVER_PORT);
int get_socket_error_code(int socket);
int show_socket_error_reason(const char *str, int socket);

//-------------------------------------------------------------
//send data
void hxtcp_send(char* databuff, int len)
{
            len = send(connect_socket, databuff , len, 0);
            if (len > 0) {
            } else {
                int err = get_socket_error_code(connect_socket);
                if (err != ENOMEM) {
                    show_socket_error_reason("send_data", connect_socket);
                }
            }
 }

//use this esp32 as a tcp client. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client(char* SERVER_HOST,int SERVER_PORT)
{
	char SERVER_IP[20]={0};
	ESP_LOGI(TAG, "create_tcp_client.   addr=%s:%d\n", SERVER_HOST, SERVER_PORT);
	if(0==SERVER_HOST[0])
			return ESP_FAIL;
	if(0==SERVER_PORT)
			return ESP_FAIL;
	//---------------------------------------------------------
	//获取服务器IP
	struct hostent * host = gethostbyname (SERVER_HOST);
	char* dn_or_ip = (char *)inet_ntoa(*(struct in_addr *)(host->h_addr));
	if(dn_or_ip)
	{
		printf("----------------- >>> %s... ip=%s\n",SERVER_HOST,dn_or_ip);
		strcpy(SERVER_IP,dn_or_ip);

		//---------------------------------------------------------
		//客户端连接
		connect_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect_socket < 0) {
			show_socket_error_reason("create client", connect_socket);
			return ESP_FAIL;
		}
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
		ESP_LOGI(TAG, "connecting to server...");
		if (connect(connect_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
			show_socket_error_reason("client connect", connect_socket);
			return ESP_FAIL;
		}
		ESP_LOGI(TAG, "connect to server success!");
		return ESP_OK;
	}

	printf("----------------- >>> %s  get ip error\n",SERVER_HOST);
	return ESP_FAIL;
}

int get_socket_error_code(int socket)
{
    int result;
    u32_t optlen = sizeof(int);
    int err = getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen);
    if (err == -1) {
        ESP_LOGE(TAG, "getsockopt failed:%s", strerror(err));
        return -1;
    }
    return result;
}

int show_socket_error_reason(const char *str, int socket)
{
    int err = get_socket_error_code(socket);

    if (err != 0) {
        ESP_LOGW(TAG, "%s socket error %d %s", str, err, strerror(err));
    }

    return err;
}

int hxtcp_check_working_socket()
{
    int ret;
    ESP_LOGD(TAG, "check connect_socket");
    ret = get_socket_error_code(connect_socket);
    if (ret != 0) {
        ESP_LOGW(TAG, "connect socket error %d %s", ret, strerror(ret));
    }
    if (ret != 0) {
        return ret;
    }
    return 0;
}

void hxtcp_close_socket()
{
    close(connect_socket);
}

void hxtcp_conn()
{
	int len = 0;
	char *databuff = (char *)malloc(2048);

	//转发服务器信息
	char transServIP[20]={0};
	int transServPort=0;

    while (1) {
            ESP_LOGI(TAG, "tcp_client will start after 1s...");
            vTaskDelay(1000 / portTICK_RATE_MS);
			//连接到调度服务器
			if(ESP_OK== create_tcp_client(g_thxlinkinfo.host,g_thxlinkinfo.port))
			{
					char rvcahce[32];
					int rvlen=0;
					len=0;
					memset(databuff,0,2048);
					printf("-------------------   connect dispatch server ---------------------- \n");
					while (1) {
							len = recv(connect_socket, databuff , 2048, 0);
							if (len > 0) {
								printf("recv_data have new data len=%d\n",len);
								memcpy(&rvcahce[rvlen],databuff,len);
								rvlen+=len;
								if(rvlen >=6)
								{
									sprintf(transServIP,"%d.%d.%d.%d",rvcahce[0],rvcahce[1],rvcahce[2],rvcahce[3]);
									transServPort=rvcahce[4] | (rvcahce[5] <<8);
									printf("transServIP=%s transServPort=%d\n",transServIP,transServPort);
									break;
								}
							}
							else if(0==len) 
							{
								printf("tcp_conn disconnect 0\n");
								break;
							}
							else if(-1==len) 
							{
								printf("tcp_conn disconnect -1\n");
								break;
							}
					}
					hxtcp_close_socket();
			}
			//连接到转发服务器
			if(ESP_OK== create_tcp_client(transServIP,transServPort))
			{
					bool isloginDTRS=false; //是否已经登录DTRS系统
					len=0;
					memset(databuff,0,2048);
					printf("-------------------   connect DTRS server ---------------------- \n");
					while (1) {
							len = recv(connect_socket, databuff , 2048, 0);
							if (len > 0) {
								//未登录处理
								if(false==isloginDTRS)
								{
										switch (databuff[0])
										{
										case 0x00:
												hxtcp_send("iot-dev",8);
											break;
										case 0x01:
												isloginDTRS=true;
												len--;
												if(len>0)
												{memcpy(&databuff[0],&databuff[1],len);}
												printf("login DTRS success...\n");
											break;
										}
								}
								//已经登录DTRS系统
								if(true==isloginDTRS)
								{
									g_dtrs_sys_recv(databuff,len);
								}
							}
							else if(0==len) 
							{
								printf("tcp_conn disconnect 0\n");
								break;
							}
							else if(-1==len) 
							{
								printf("tcp_conn disconnect -1\n");
								break;
							}
					}
					hxtcp_close_socket();
			}
    }
	free(databuff);
	vTaskDelete(NULL);
}
