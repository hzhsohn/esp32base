/* Standard includes. */
#include "string.h"
#include "esp_err.h"
/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_vfs_fat.h"
/* Utils includes. */
#include "esp_log.h"
#include "webserver.h"
#include "cJSON.h"

#include "esp_spi_flash.h"
#include <freertos/semphr.h>
#include "driver/timer.h"
#include "esp_intr_alloc.h"
#include "spi_data.h"
#include "http_serv.h"
#include "eth.h"
#include "zhHttp.h"


#define TAG "web:"


//---------------------------------

TzhHttpServ * g_httpServ=NULL;//http协议处理模块
PF_WEBSERV_CALLBACK* g_pfwebcbcbcb=NULL;
PF_WEBSERV_DONE_CALLBACK* g_pfwebdone=NULL;
static struct sockaddr_in server;
int g_web_port=0;
int socket_fd=0;
int g_web_client_fd=0;
int g_web_use_client_fd=0;

//将生成的web链表数据发送到客户端
void sendToWebClient(char* data,int len);

//打印16进制信息
int print16toBuf(char*buf,int len,char* dst_buf)
{
	dst_buf[0]=0x00;
	for(int i=0;i<len;i++)
	{
		sprintf(dst_buf,"%s %02X",dst_buf,buf[i]);
	}
	return strlen(dst_buf);
}



int web_socket_server(in_port_t in_port, in_addr_t in_addr)
{
	int socket_fd, on;
	//struct timeval timeout = {10,0};

	server.sin_family = AF_INET;
	server.sin_port = in_port;
	server.sin_addr.s_addr = in_addr;

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		perror("listen socket uninit\n");
		return -1;
	}
	on=1;
	//setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int) );
	//CALIB_DEBUG("on %x\n", on);
	if((bind(socket_fd, (struct sockaddr *)&server, sizeof(server)))<0) {
		perror("cannot bind srv socket\n");
		return -1;
	}

	if(listen(socket_fd, 1)<0) {
		perror("cannot listen");
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}
void webserver_init(int web_port,PF_WEBSERV_CALLBACK* webcb,PF_WEBSERV_DONE_CALLBACK* webdone)
{
	g_web_port=web_port;
	g_pfwebcbcbcb=webcb;
	g_pfwebdone=webdone;
	
	if(NULL==g_httpServ)
	{
		g_httpServ=(TzhHttpServ*)malloc(sizeof(TzhHttpServ));
		memset(g_httpServ,0,sizeof(TzhHttpServ));
	}
}

void app_webserver_task_accept()
{
	//printf("app_webserver_task_accept\n");

	//---------------------
	struct sockaddr_in  client;
	int client_size=sizeof(client);
	//---------------------
	int socket_fd = web_socket_server(htons(g_web_port),htonl(INADDR_ANY));
	//----------------------
	if( socket_fd >= 0 ){
		for(;;){
				g_web_client_fd=accept(socket_fd,(struct sockaddr*)&client,(socklen_t*)&client_size);
				//printf("g_web_client_fd=%d \n",g_web_client_fd);
				if(g_web_client_fd>0L){
							//---------------------
							//初始化web
							if(g_web_use_client_fd!=0)
							{
								//断开原有连接并
								//ESP_LOGI("web","web disconnect socket:%d",g_web_use_client_fd);
								close( g_web_use_client_fd );
							}
							g_web_use_client_fd=g_web_client_fd;
							if(g_httpServ)
							{
								memset(g_httpServ,0,sizeof(TzhHttpServ));
							}
				}
				else
				{
						ESP_LOGI("web","accept error restart...");
						esp_restart();
				}
			}
	}

	vTaskDelete(NULL); 
}
void app_webserver_task_recv()
{
	//printf("app_web_task_recv\n");
	//-----------------------------------
	char *url;
	url=(char *)malloc(1124);
	//
	int len=0;
	char recv_buf[128];
	for(;;)
	{
		if(g_web_use_client_fd>0)
		{
				len = recv( g_web_use_client_fd, recv_buf, sizeof(recv_buf), 0 );
				if(len>0)
				{
										int httpret=httpServReslovURL(g_httpServ,recv_buf,len,url);
										if(0==httpret)
										{
											char* page;
											char* parameter;
						
											//printf("httpServReslovURL=%s\n",g_httpServ->cacheBuf);
											//-----------------------------------
											page=(char*)malloc(64);
											parameter=(char*)malloc(1024);
											memset(page,0,64);
											memset(parameter,0,1024);

											//-----------------------------------
											//获取页面和参数
											urlSplit(url,page,parameter);
											//printf("url=%s\n",url);
											//
											g_pfwebcbcbcb(page,parameter);
											
											//
											close( g_web_use_client_fd );											
											g_pfwebdone(page);
											//-----------------------------------						
											free(page);
											page=NULL;
											free(parameter);
											parameter=NULL;
											//
										}
										else if(1==httpret)
										{
											//缓冲区错误断开连接
											printf("httpret = 1 disconnect web client \n");
											close( g_web_use_client_fd );
										}
					
				}
				else if(0==len)
				{
					g_web_use_client_fd=0;
				}
				else if(-1==len)
				{
					g_web_use_client_fd=0;
				}
		}
		else
		{
			//延时
			vTaskDelay(100 / portTICK_PERIOD_MS);      
		}
	}

	if(url)
	{
		free(url);
		url=NULL;
	}
	vTaskDelete(NULL); 
}

void webSendHeader(int len)
{
	char*pstr=httpServResponeOK(g_httpServ,len);
	webSendClient(pstr,strlen(pstr));
	free(pstr);
}
void webSendClient(char* data,int len)
{
		int bblen=0;
		int bb=0;
		while(bblen<=len)
		{
			bb=len-bblen;
			if(0==bb)
			{ break; }
			bb=bb>500?500:bb; //最大发送500字节
			bb=write(g_web_use_client_fd, data+bblen, bb);
			if(-1==bb)
			{ break; }
			bblen+=bb;
		}
}

void webSendClientNotFoundPage()
{
		//找不到页面
		char*pstr=httpServResponeNoFound(g_httpServ);
		webSendClient(pstr,strlen(pstr));
		free(pstr);
}