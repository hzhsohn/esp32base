#include "websocket_serv.h"
#include "websocket.h"
#include "zhHttp.h"

//websocket����
typedef struct TzhWSockClient{
	int  pespconn;
	TzhWebSocket hs;
	char recvCache[2000];
	int recvLen;
}TzhWSockClient;

TzhWSockClient wsclient={0};
int wssocket_ret;
char wssocket_acceptBuf[512]={0};

int isOpenWebsocketFunc=0;
int g_websocket_port=0;
int g_wsk_client_fd=0;

static struct sockaddr_in wsockTcp_fd;
PF_WEBSOCKET_READ * g_trans_websocket_recv;

void websock_recv_cb(int sock_fd, char *pusrdata, unsigned short length)
{
    //printf("websock_recv_cb length=%d\n",length); //��ӡ��������
	//��������Ͽ�wsclient.recvCache
	if(wsclient.recvLen+length>sizeof(wsclient.recvCache))
	{
		//�Ͽ��ͻ���
		close(wsclient.pespconn);
	}
	//���ݻ���
	memcpy(&wsclient.recvCache[wsclient.recvLen],pusrdata,length);
	wsclient.recvLen+=length;
	//
	wssocket_ret=zhWebSockHandshake(wsclient.recvCache,wsclient.recvLen,&wsclient.hs,wssocket_acceptBuf);
	if(wssocket_ret==0)
	{
			//����0������������
			ESP_LOGI("websocket","new client:%d",g_wsk_client_fd);	
			//����н��ջ�����,�����ֺ�Ҫ�����������
			write(sock_fd,wssocket_acceptBuf,strlen(wssocket_acceptBuf));
			wsclient.recvLen=0;
	}
	else if(wssocket_ret==1)
	{
			int totle=0,frame_begin=0,frame_len=0;
			char str[1000]={0}; 
			int len=0;
		
			//printf("111ws=%d   %d   %d\n",wsclient.recvLen,len,frame_len);
			//���Ҫ���ճ��totle��Ҫ�ڻ�������ȥ���ֽ���,����û�д���ճ��
			totle=zhWebSockRecvPack(&wsclient.hs,wsclient.recvCache,wsclient.recvLen,str,&len,&frame_begin,&frame_len);
			if(totle>0)
			{				
				int recv_len;
				recv_len=zhHttpUrlDecode(str,len);
				//���ݽ���
			    g_trans_websocket_recv(str,recv_len);
				//printf("��������totle=%d frame_begin=%d frame_len=%d len=%d %s\r\n",totle,frame_begin,frame_len,len,str);
			}
			//ɾ��������������
			wsclient.recvLen-=totle;
			memcpy(&wsclient.recvCache[0],&wsclient.recvCache[totle],wsclient.recvLen);
			//printf("222ws=%d   %d   %d  totle=%d\n",wsclient.recvLen,len,frame_len,totle);
	}
}

int wsock_creat_socket_server(in_port_t in_port, in_addr_t in_addr)
{
	int socket_fd, on;
	//struct timeval timeout = {10,0};

	wsockTcp_fd.sin_family = AF_INET;
	wsockTcp_fd.sin_port = in_port;
	wsockTcp_fd.sin_addr.s_addr = in_addr;

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		perror("listen socket uninit\n");
		return -1;
	}
	on=1;
	//setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int) );
	//CALIB_DEBUG("on %x\n", on);
	if((bind(socket_fd, (struct sockaddr *)&wsockTcp_fd, sizeof(wsockTcp_fd)))<0) {
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

void app_websocket_task_accept()
{
	//printf("app_websocket_task_accept\n");

	//---------------------
	struct sockaddr_in  client;
	int client_size=sizeof(client);
	//---------------------
	int socket_fd = wsock_creat_socket_server(htons(g_websocket_port),htonl(INADDR_ANY));
	//----------------------
	if( socket_fd >= 0 ){
		for(;;){
				g_wsk_client_fd=accept(socket_fd,(struct sockaddr*)&client,(socklen_t*)&client_size);
				if(g_wsk_client_fd>0L){
							//---------------------
							//printf("app_websocket_task_accept wsclient.pespconn=%d \n",wsclient.pespconn);
							//��ʼ��websocket
							if(wsclient.pespconn!=0)
							{
								//�Ͽ�ԭ�����Ӳ���������websocket
								ESP_LOGI("websocket","websocket disconnect socket:%d",g_wsk_client_fd);
								close( wsclient.pespconn );
							}							
							wsclient.pespconn=g_wsk_client_fd;
							zhWebSockInit(&wsclient.hs);
							wsclient.recvLen=0;
							//ESP_LOGI("websocket","new connect:%d",g_wsk_client_fd);
				}
				else
				{
						ESP_LOGI("websocket","accept error restart...");
						esp_restart();
				}
			}
	}

	vTaskDelete(NULL); 
}
void app_websocket_task_recv()
{
	//printf("app_websocket_task_recv\n");

	int len=0;
	char recv_buf[128];
	for(;;)
	{
		if(g_wsk_client_fd>0)
		{
				//recv������
				len = recv( g_wsk_client_fd, recv_buf, sizeof(recv_buf), 0 );
				if(len>0)
				{
					websock_recv_cb(g_wsk_client_fd, recv_buf, len);
				}
				else if(0==len)
				{
					close(wsclient.pespconn);
					wsclient.pespconn=0;
					wsclient.recvLen=0;
					zhWebSockFree(&wsclient.hs);
					g_wsk_client_fd=0;
				}
				else if(-1==len)
				{
					close(wsclient.pespconn);
					wsclient.pespconn=0;
					wsclient.recvLen=0;
					zhWebSockFree(&wsclient.hs);
					g_wsk_client_fd=0;
				}
		}
		else
		{
			//��ʱ
			vTaskDelay(100 / portTICK_PERIOD_MS);      
		}
	}

	vTaskDelete(NULL); 
}

void  websocket_init(PF_WEBSOCKET_READ*pf, int port)
{
	if(port>0)
	{
		g_trans_websocket_recv=pf;
		g_websocket_port=port;
		isOpenWebsocketFunc=1;
	}
	else
	{
		ESP_LOGI("websocket","Failure to initizal port=%d",port);
	}
}

void  websocket_send(char* buf,int len)
{
	if(isOpenWebsocketFunc && g_wsk_client_fd>0)
	{
		char *sendData;
		int sendDataLen;
		char *pBuf;
		int new_len=0;
		sendData=(char *)malloc(len*3+255);
		pBuf=zhHttpUrlEncode(buf,len,&new_len);

		zhWebSockSendData(&wsclient.hs,pBuf,new_len,sendData,&sendDataLen);	
		write(g_wsk_client_fd,sendData,sendDataLen);

		free(pBuf);
		pBuf=NULL;
		free(sendData);
		sendData=NULL;
	}
}