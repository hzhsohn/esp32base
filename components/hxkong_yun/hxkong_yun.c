#include "hxkong_yun.h"
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

#include "hxkong_tcp.h"
#include "hxkong_yun.h"

//全局变量
ThxtLinkInfo g_thxlinkinfo={0};
PF_HXKONG_YUN_RECV*	g_dtrs_yun_recv;
/*****************************
云处理数据
*/
uchar g_YunCache[1400];
int g_YunLen=0;
TzhMiniData g_ocYunCmd;
uchar g_isYunGetCmdOk;

//函数头定义
void dtrs_recv_data_callback (const char *yunParam, int paramLen);

//
void yun_init(PF_HXKONG_YUN_RECV* pf, char* host ,int port)
{
	g_dtrs_sys_recv=dtrs_recv_data_callback;
	g_dtrs_yun_recv=pf;
	strcpy(g_thxlinkinfo.host, host);
	g_thxlinkinfo.port=port;
}
void yun_app_start()
{
	if(g_thxlinkinfo.port>0)
	{
		xTaskCreate(hxtcp_conn, "hxtcp_conn", 6*1024,NULL , 5, NULL);
		ESP_LOGI("yun", "Free memory: %d bytes", esp_get_free_heap_size());
	}
}
//DTRS系统过来的数据
void dtrs_recv_data_callback (const char *yunParam, int paramLen)
{
		int tmp;
		int n;
  
		printf("dtrs_recv_data_callback ... paramLen=%d\n", paramLen);

		//限制缓冲区溢出
		if(g_YunLen+paramLen>1399/*sizeof(g_YunCache)*/)
		{ g_YunLen=0; }

		//将接收到的数据放到缓冲区
		memcpy(&g_YunCache[g_YunLen],yunParam,paramLen);
		g_YunLen+=paramLen;
  
		//处理缓冲区获取数据帧
 _nnc:
        //获取指令
        tmp=miniDataGet(g_YunCache,g_YunLen,&g_ocYunCmd,&g_isYunGetCmdOk);
		if(g_isYunGetCmdOk)
		{	
			uchar cmd=g_ocYunCmd.parameter[0];						
			//处理其它功能的协议
			actionTransServDataFunc(cmd,&g_ocYunCmd.parameter[1],g_ocYunCmd.parameter_len - 1);
		}
		//调整缓冲区
		if(tmp>0)
		{
			g_YunLen-=tmp;
			for(n=0;n<g_YunLen;n++)
			{
				g_YunCache[n]=g_YunCache[tmp+n];
			}
			goto _nnc;
		}
}

//服务器过来的数据处理
void actionTransServDataFunc(char cmd, uchar* param, int len)
{
		switch (cmd)
		{
		case ezhDNProl_Data_Ret:
			{
				g_dtrs_yun_recv((char*)param,len);
			}
			break;			
		case ezhDNProl_Keep_Ret:
			{
				//可以利用这个返回值计算出RTT时间				
			}
			break;
		case ezhDNProl_SignInDev_Ret://连接成功
			{
				printf("ezhTDProcol_SignInDev_Ret\n");
				//actionSetDevInfo(config.CAID_account, config.devFlag, config.devname);
			}
			break;
		case ezhDNProl_SetDevInfo_Ret:
			{
				printf("ezhDNProl_SetDevInfo_Ret\n");				
			}
			break;
		}
}

//发送数据
void iotYunSend(char* buf,ushort len)
{
	//封装成miniData发送
	char *yesSendBuf=NULL,*tmpBuf=NULL;
	int yesSendLen=0;
	yesSendBuf=(char*)malloc(len+10);
	tmpBuf=(char*)malloc(len+2);

	tmpBuf[0]=ezhDNProl_Data;
	memcpy(&tmpBuf[1],buf,len);
	yesSendLen=miniDataCreate(len+1,(uchar*)tmpBuf,(uchar*)yesSendBuf);
	//
	hxtcp_send((char*)yesSendBuf,yesSendLen);
	//
	free(tmpBuf);
	tmpBuf=NULL;
	free(yesSendBuf);
	yesSendBuf=NULL;
}

//------------------------------------------------------------
//签入硬件到转发服务器
void actionSignIn(const char* DPID,const char* devUUID)
{
	char param[500]={0};
	char sendBuf[600]={0};
	int sendLen=0;
	int tmp=0,pos=0;
	
	param[pos]=ezhDNProl_SignInDev;
	pos++;
	
	tmp=strlen(DPID)+1;
	memcpy(&param[pos],DPID,tmp);
	pos+=tmp;

	tmp=strlen(devUUID)+1;
	memcpy(&param[pos],devUUID,tmp);
	pos+=tmp;

	sendLen=miniDataCreate(pos,(uchar*)param,(uchar*)sendBuf);
	hxtcp_send((char*)sendBuf,sendLen);
}

void actionSetDevInfo(const char* CAID ,
										const char* devflag ,
										const char* devName)
{
	char param[500]={0};
	char sendBuf[600]={0};
	int sendLen=0;
	int tmp=0,pos=0;
	
	param[pos]=ezhDNProl_SetDevInfo;
	pos++;
	
	tmp=strlen(CAID)+1;
	memcpy(&param[pos],CAID,tmp);
	pos+=tmp;

	tmp=strlen(devflag)+1;
	memcpy(&param[pos],devflag,tmp);
	pos+=tmp;

	tmp=strlen(devName)+1;
	memcpy(&param[pos],devName,tmp);
	pos+=tmp;

	sendLen=miniDataCreate(pos,(uchar*)param,(uchar*)sendBuf);
	hxtcp_send((char*)sendBuf,sendLen);
}

//更新RTT信息到服务器
void actionKeep()
{
	//封装成miniData发送
	char sendBuf[20]={0};
	int sendLen=0;
	char param[10]={0};
	
	param[0]=ezhDNProl_Keep;

	sendLen=miniDataCreate(1,(uchar*)param,(uchar*)sendBuf);
	//
	hxtcp_send(sendBuf,sendLen);
}
