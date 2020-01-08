#include "trans_data.h"
#include "uart0.h"
#include "uart1.h"
#include "spi_data.h"
#include "scene_proc.h"
#include "h_gpio.h"
#include "esp_wifi.h"
#include "websocket.h"
#include "sbufhex.h"
#include "zhlist_exp.h"
#include "mqtt_net_proc.h"
#include "tcpserv.h"
#include "tcpserv2.h"
#include "zhHttp.h"

//��������󳤶�
char g_debugCmdline[255]={0};
int g_debugCmdlineLen=0;
//���յ����ʱ��
unsigned long g_recvUart1CurrentMillis=0;
unsigned long g_CacleTime=0;
//
unsigned char g_recvUart1Buf[255]={0};
int  g_recvUart1BufLen=0;

//
esp_mqtt_client_handle_t g_mqttClient;
//�пصĶ��ĵ�ַ
extern char g_msdA_devuuid[72];
extern char g_msdA_devname[72];
extern char g_msdA_devflag[72];
extern char g_msdA_pub[72]; //�豸����
extern char g_msdA_subscr[72]; //�豸����


//
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
void uart1485Recv_cb(unsigned char*buf,int len);
//
char*g_webParameter;


void cfgInit()
{
	//
	g_recvUart1CurrentMillis=0;
	g_debugCmdlineLen=0;
	g_recvUart1BufLen=0;
	
	LED_WIFI_OnOff(false);	
	LED_MQTT_OnOff(false);	
	//mqtt_app_start();
}


//-----------------------------------------------------------------
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
#define MQTT_TAG		"MQTT_CLIENT"
    int msg_id=0;
    esp_mqtt_client_handle_t client = event->client;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            printf(">>>MQTT connected\n");
			LED_MQTT_OnOff(true);
            msg_id=esp_mqtt_client_subscribe(client, "MSD/A", 0);
            printf(">>>subscribe \"MSD/A\" successful, msg_id=%d\n", msg_id);
            msg_id=esp_mqtt_client_subscribe(client, g_msdA_subscr, 0);			
            printf(">>>subscribe \"%s\" successful, msg_id=%d\n",g_msdA_subscr, msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            printf(">>>MQTT disconnect\n");
			LED_MQTT_OnOff(false);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            //ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            //msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            //ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            //ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            //ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
			mqtt_proc_data(client,event->topic,event->topic_len,event->data,event->data_len);
            break;
        case MQTT_EVENT_ERROR:
            printf(">>>MQTT connect error\n");
			LED_MQTT_OnOff(false);
            break;
    }
    return ESP_OK;
}

void mqtt_app_start(void)
{
	if(g_mqttClient)
	{
		esp_mqtt_client_stop(g_mqttClient);
		g_mqttClient=NULL;
	}
	
	TagCfgData *pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	if(NULL==pCfgdata)
	{
		//��ȡʧ�������豸
		esp_restart();
	}

	if(pCfgdata->is_mqtt_start)
	{
		if(0==pCfgdata->mqtt_port)
		{
			printf(">>>mqtt port=0 ,not start service\n");
			return;
		}
	
		esp_mqtt_client_config_t mqtt_cfg={0};
		sprintf(mqtt_cfg.uri , "mqtt://%s:%d",pCfgdata->mqtt_host,pCfgdata->mqtt_port);
		//mqtt_cfg.uri = "mqtts://api.emitter.io:443",    // for mqtt over ssl
		// mqtt_cfg.uri = "mqtt://api.emitter.io:8080", //for mqtt over tcp
		// mqtt_cfg.uri = "ws://api.emitter.io:8080", //for mqtt over websocket
		// mqtt_cfg.uri = "wss://api.emitter.io:443", //for mqtt over websocket secure
		mqtt_cfg.event_handle = mqtt_event_handler;

		g_mqttClient = esp_mqtt_client_init(&mqtt_cfg);
		esp_mqtt_client_start(g_mqttClient);

		printf(">>>connect mqtt service %s\n", mqtt_cfg.uri);
		ESP_LOGI(MQTT_TAG, "Free memory: %d bytes", esp_get_free_heap_size());
	}
}


//�����ص�
void btn_press(int gpio_num, int is_press)
{
		if(GPIO_INPUT_IO_0==gpio_num)
		{
			static int cur_press_cacal=0;
			static int is_long_press_ok=0;
			if(0==is_press)
			{
				cur_press_cacal=0;
				is_long_press_ok=0;
			}
			else
			{			

				if(0==is_long_press_ok)
				{
					cur_press_cacal++;
					if(cur_press_cacal>30)//30���5������
					{
						//��������
						is_long_press_ok=1;
						//-------------------
						//��������,Ϊϵͳ�ָ�
						initFactoryCfgFlash();
						esp_restart();
						//-------------------
						//������λ
						is_long_press_ok=0;
						cur_press_cacal=0;
					}
				}
			}
		}	
		
}

void trans_data_task()
{	
	while(1)
	{
			//printf("trans_data_task \n");

			//-------------------------
			//��ʱ�ȴ�ȫ�����ݽ��������г�������		
			g_CacleTime++;
			if(g_recvUart1BufLen>0)
			{
				if(g_CacleTime - g_recvUart1CurrentMillis>10)
				{
					//���ݴ���ģʽ
					uart1485Recv_cb(g_recvUart1Buf,g_recvUart1BufLen);
					//
					memset(g_recvUart1Buf,0,g_recvUart1BufLen);
					g_recvUart1BufLen=0;
				}
			}

			//---------------------------
			//����10����
			vTaskDelay(10 / portTICK_RATE_MS);
	}

	//-------------------------
	vTaskDelete(NULL);
	printf("trans_data_task destory...\n");
}


void uart_debug_read(const char *buf, int len)
{
	int isEnter=0;
	int i=0;
	
	//����ַ�
	uart_debug_send(buf, len);

	//����������
	if(g_debugCmdlineLen>=sizeof(g_debugCmdline))
	{ g_debugCmdlineLen=0; }
	//----------------------------------------
	//���ƽ��յ�������
	for(i=0;i<len;i++)
	{
		if(0x1B==buf[i])//�������������Ѿ��˳�ѭ��
		{
			break;
		}
		if((buf[i]>=32 && buf[i]<=127) || 0x0D==buf[i])
		{
			if(0x7F==buf[i])
			{
				if(g_debugCmdlineLen>0)
				{
					//�˸��
					g_debugCmdlineLen--;
					g_debugCmdline[g_debugCmdlineLen]=0;
				}
			}
			else	
			{
				g_debugCmdline[g_debugCmdlineLen]=buf[i];
				g_debugCmdlineLen++;
			}
		}
	}
	//----------------------------------------
	for(i=0;i<g_debugCmdlineLen;i++)
	{
			if(0x0D==g_debugCmdline[i] ) //�ж��Ƿ�Ϊ�س�
			{
				g_debugCmdline[i]=0;				
				g_debugCmdlineLen=i;
				isEnter=1;
				printf("\n");
				break;
			}
	}
	if(0==strcmp(g_debugCmdline,"")) //�ж��Ƿ�Ϊ��ָ��
	{
		g_debugCmdline[0]=0;
		g_debugCmdlineLen=0;
		return;
	}
	
	//----------------------------------------
	if(isEnter)
	{
			do_command(g_debugCmdline);			
			//-------------------------------------
			//ֻҪ�յ��س���������������е�����
			g_debugCmdline[0]=0;
			g_debugCmdlineLen=0;
	}
}

//����1Ϊ485��
void uart1_read(const char *buf, int len)
{
		printf("uart1_read data > len=%d > ",len);
		print16((char*)buf,len);
		printf("\n");

		//��������,��ʵʱѭ������Ϊ485���
		memcpy(&g_recvUart1Buf[g_recvUart1BufLen],buf,len);
		g_recvUart1BufLen+=len;
		g_recvUart1CurrentMillis = g_CacleTime;
		//printf("uart1_read len=%d ------ %d\n",len,g_recvUart1BufLen);
}

//����2Ϊ�������ڲ����ݴ����
void uart2_read(const char *buf, int len)
{
		printf("uart2_read len=%d ------",len);
		print16((char*)buf,len);
		printf("\n");
		
		//ת����TCP������2
		tcpserv2_send((char*)buf,len);
}

void websock_read(const char *buf, int len)
{
		uart1485Send((char*)buf,len);
}

void tcpserv_read(const char *buf, int len)
{
		uart1485Send((char*)buf,len);
}

void tcpserv2_read(const char *buf, int len)
{
		uart2485Send((char*)buf,len);
}

//����1 ������ʱ�������������
void uart1485Recv_cb(unsigned char*buf,int len)
{	
	char* ssmbuf=NULL;
	ssmbuf=(char*)malloc(128+len*2);

	printf("receive 485 frame > len=%d > ",len);
	print16((char*)buf,len);
	printf("\n");
		
	//----------------------------------------------------
	//���͵�MQTT����
	printf("publish = %s\n",g_msdA_pub);
	
	int dstLen=0;
	char* psbuf=sbufEncode(buf,len,&dstLen);
	sprintf(ssmbuf,"{\"cmd\":\"lbus\",\"relation\":{\"sbuf\":\"%s\"}}",psbuf);
	free(psbuf);
	printf("len=%d >> %s \n",dstLen,ssmbuf);
	if(g_mqttClient)
	{
		//���ͳ�������
		esp_mqtt_client_publish(g_mqttClient, g_msdA_pub,ssmbuf, strlen(ssmbuf), 0, 0);
	}

	free(ssmbuf);
	ssmbuf=NULL;

	//----------------------------------------------------
	//ת����websocketҳ����
	websocket_send((char*)g_recvUart1Buf,g_recvUart1BufLen);

	//----------------------------------------------------
	//ת����TCP������
	tcpserv_send((char*)g_recvUart1Buf,g_recvUart1BufLen);

}

//UDP�ظ�
void udp_read(const char *data, int len)
{
	if(len>4)
	{
				if( data[0]=='F' &&
					data[1]=='M' &&
					data[2]=='S' && 
					data[3]=='D')
				{
					isSearchFrameData(g_msdA_devuuid,g_msdA_devname,g_msdA_devflag);
					return;
				}
	}

}

//�ƽ���
void yun_recv(const char *buf, int len)
{
	
}

void cmd_setmqtt(char* commandline)
{
	char*pastr=NULL;
	char param1[150]={0};
	char param2[150]={0};
	char* pstr=NULL;
	int port=0;
	//printf("commandline=%s\n",commandline);
	pstr=commandline;
	//
	pastr=pstr;
	pstr=strchr(pstr,',');
	if(NULL==pstr){printf("parameter1 error.\n");return;}
	*pstr=0;
	strcpy(param1,pastr);
	trim(param1);
	printf("host=%s\n",param1);
	pstr++;
	//
	strcpy(param2,pstr);
	trim(param2);
	port=atoi(param2);
	printf("port=%d\n",port);
	//
	if(0==setMqtt(param1,port))
	{
		printf("set mqtt address success.\n");
		mqtt_app_start();
	}
	else
	{	
		printf("set mqtt address fail!!!\n");
	}
}

void cmd_setmqttinfo(char* commandline)
{
	char*pastr=NULL;
	char param1[150]={0};
	char param2[150]={0};
	char param3[150]={0};
	char* pstr=NULL;

	//printf("commandline=%s\n",commandline);
	pstr=commandline;
	//
	pastr=pstr;
	pstr=strchr(pstr,',');
	if(NULL==pstr){printf("parameter1 error.\n");return;}
	*pstr=0;
	strcpy(param1,pastr);
	trim(param1);
	printf("client_id=%s\n",param1);
	pstr++;
	//
	pastr=pstr;
	pstr=strchr(pstr,',');
	if(NULL==pstr){printf("parameter2 error.\n");return;}
	*pstr=0;
	strcpy(param2,pastr);
	trim(param2);
	printf("username=%s\n",param2);
	pstr++;
	//
	strcpy(param3,pstr);
	trim(param3);
	printf("password=%s\n",param3);
	//
	if(0==setMqttInfo(param1,param2,param3))
	{
		printf("set mqtt info success.\n");
		mqtt_app_start();
	}
	else
	{	
		printf("set mqtt info fail!!!\n");
	}
}

void mqtt_proc_data(esp_mqtt_client_handle_t client,char* topic,int topic_len,char*buf,int len)
{
	//printf("TOPIC=%.*s\r\n", topic_len, topic);
	//printf("DATA=%.*s\r\n", len, buf);

	if(0==memcmp(topic,"MSD/A",5))
	{
		if(0==memcmp(buf,"FMSD,",5))
		{
			char sebuf[600]={0};
			int selen=0;
			selen=searchFrameData(sebuf, g_msdA_devuuid,
										 g_msdA_devname,
										 g_msdA_devflag,
										 g_msdA_pub,
										 g_msdA_subscr);
			//�ظ�����Ӳ������Ϣ
			esp_mqtt_client_publish(client, "MSD/B",sebuf, selen, 0, 0);
		}
	}
	else if(0==memcmp(topic,g_msdA_subscr,topic_len))
	{
		//һ��ͨѶ����
		printf("mqtt_proc_data=%.*s >> %.*s\r\n", topic_len, topic,len,buf);
		mqttNetData(buf,len);
	}
}

void cmd_showuart()
{
	TagCfgData *pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	if(pCfgdata)
	{
		printf("uart1 = { %d , 8 , 1 , N } \n",pCfgdata->uart1_baudRate);
		printf("uart2 = { %d , 8 , 1 , N } \n",pCfgdata->uart2_baudRate);
	}
	else
	{
		printf("read uart1 error. \n");
	}
	printf("\n");
}

void cmd_setuart1(char* commandline)
{
	char param1[150]={0};
	char* pstr=NULL;
	//printf("commandline=%s\n",commandline);
	pstr=commandline;
	//
	strcpy(param1,pstr);
	trim(param1);
	//
	int uart1_baudrate=atoi(param1);
	if(uart1_baudrate>0)
	{
		//
		TagCfgData *pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
		if(pCfgdata)
		{
			pCfgdata->uart1_baudRate=uart1_baudrate;
			if(0==write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA))
			{				
					printf("set uart1 = { %d , 8 , 1 , N } success .\n",pCfgdata->uart1_baudRate);
					printf("uart1 enable need restart system... \n");
					esp_restart();
			}
			else
			{
					printf("save uart1 fail.\n");
			}

		}
		else
		{
			printf("set uart1 error. \n");
		}
	}
	else
	{
		printf("set uart1 baudrate error. value=%d\n",uart1_baudrate);
	}
	printf("\n");
}

void cmd_setuart2(char* commandline)
{
	char param1[150]={0};
	char* pstr=NULL;
	//printf("commandline=%s\n",commandline);
	pstr=commandline;
	//
	strcpy(param1,pstr);
	trim(param1);
	//
	int uart2_baudrate=atoi(param1);
	if(uart2_baudrate>0)
	{
		//
		TagCfgData *pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
		if(pCfgdata)
		{
			pCfgdata->uart2_baudRate=uart2_baudrate;
			if(0==write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA))
			{				
					printf("set uart2 = { %d , 8 , 1 , N } success .\n",pCfgdata->uart2_baudRate);
					printf("uart2 enable need restart system... \n");
					esp_restart();
			}
			else
			{
					printf("save uart2 fail.\n");
			}

		}
		else
		{
			printf("set uart2 error. \n");
		}
	}
	else
	{
		printf("set uart2 baudrate error. value=%d\n",uart2_baudrate);
	}
	printf("\n");
}

void cmd_restart_sys()
{
	printf("restart system... \n");
	esp_restart();
}

void cmd_open_wifi(char* commandline)
{
	char*pastr=NULL;
	char param1[150]={0};
	char param2[150]={0};
	char* pstr=NULL;
	//printf("commandline=%s\n",commandline);
	pstr=commandline;
	//
	pastr=pstr;
	pstr=strchr(pstr,',');
	if(NULL==pstr){printf("parameter1 error.\n");return;}
	*pstr=0;
	strcpy(param1,pastr);
	trim(param1);
	printf("ssid=%s\n",param1);
	pstr++;
	//
	strcpy(param2,pstr);
	trim(param2);
	printf("password=%s\n",param2);
	//
	if(0==setOpenWifi(param1,param2))
	{
		printf("open wifi info ssid=%s,password=%s\n",param1,param2);
		mqtt_app_start();
	}
	else
	{	
		printf("open wifi info!\n");
	}
}

void cmd_connect_wifi(char* commandline)
{
	char*pastr=NULL;
	char param1[150]={0};
	char param2[150]={0};
	char* pstr=NULL;
	//printf("commandline=%s\n",commandline);
	pstr=commandline;
	//
	pastr=pstr;
	pstr=strchr(pstr,',');
	if(NULL==pstr){printf("parameter1 error.\n");return;}
	*pstr=0;
	strcpy(param1,pastr);
	trim(param1);
	printf("ssid=%s\n",param1);
	pstr++;
	//
	strcpy(param2,pstr);
	trim(param2);
	printf("password=%s\n",param2);
	//
	if(0==setConnectWifi(param1,param2))
	{
		printf("connect wifi info ssid=%s,password=%s\n",param1,param2);
		mqtt_app_start();
	}
	else
	{	
		printf("connect wifi info!\n");
	}
}

void cmd_close_wifi()
{
	setCloseWifi();
}

void cmd_mqtt_start()
{
	setMqttStartAndStop(1);
	//
	esp_restart();
}
void cmd_mqtt_stop()
{
	setMqttStartAndStop(0);
	//
	esp_restart();
}
void cmd_yun_start()
{
	setYunStartAndStop(1);
	//
	esp_restart();
}
void cmd_yun_stop()
{
	setYunStartAndStop(0);
	//
	esp_restart();
}

void pfWebServ_Callback (char *page, char * parameter)
{
	int i=0;
	g_webParameter=NULL;
	//-----------------------------------
	//WEBҳ��
	if(0==strcmp(page,"/") || 0==strcmp(page,"/index.htm"))//��ҳ
	{
		int nnelen=1024*8;
		char *htmldata=NULL;
		//
		htmldata=(char *)get_spi_flash_binary_data(nnelen, SECTOR_WEBPAGE_1);
		if(htmldata)
		{
			for(i=0;i<nnelen;i++)
			{
				if(htmldata[i]==0x00 || htmldata[i]==0xFF)
				{ break; }
			}
			htmldata[i]=0;
			webSendHeader(i);
			webSendClient(htmldata,i);
			//printf("htmldata=%s\n",htmldata);
		}
		free(htmldata);
	}
	else if(0==strcmp(page,"/485view.htm"))
	{
		int nnelen=1024*8;
		char *htmldata=NULL;
		//
		htmldata=(char *)get_spi_flash_binary_data(nnelen, SECTOR_WEBPAGE_2);
		if(htmldata)
		{
			for(i=0;i<nnelen;i++)
			{
				if(htmldata[i]==0x00 || htmldata[i]==0xFF)
				{ break; }
			}
			htmldata[i]=0;
			webSendHeader(i);
			webSendClient(htmldata,i);
			//printf("htmldata=%s\n",htmldata);
		}
		free(htmldata);
	}	
	else if(0==strcmp(page,"/info.htm"))
	{
		int nnelen=1024*8;
		char *htmldata=NULL;
		//
		htmldata=(char *)get_spi_flash_binary_data(nnelen, SECTOR_WEBPAGE_3);
		if(htmldata)
		{
				TagCfgData *pCfgdata=NULL;
				pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
				if(pCfgdata)
				{
					//�滻����
					char strbuf[20]={0};
					replace_str(htmldata,"{mqttip}",pCfgdata->mqtt_host,htmldata);	
					itoa(pCfgdata->mqtt_port,strbuf,10);
					replace_str(htmldata,"{mqttport}",strbuf,htmldata);	
					replace_str(htmldata,"{mqttcid}",pCfgdata->mqtt_clientid,htmldata);	
					replace_str(htmldata,"{mqttuser}",pCfgdata->mqtt_username,htmldata);	
					replace_str(htmldata,"{mqttpasswd}",pCfgdata->mqtt_password,htmldata);	
					//
					itoa(pCfgdata->uart1_baudRate,strbuf,10);
					replace_str(htmldata,"{uart1brt}",strbuf,htmldata);	
					//
					replace_str(htmldata,"{devuuid}",pCfgdata->devuuid,htmldata);
					replace_str(htmldata,"{devname}",pCfgdata->devname,htmldata);
					replace_str(htmldata,"{devflag}",pCfgdata->devflag,htmldata);
					//
					itoa(pCfgdata->tcpserv1_port,strbuf,10);
					replace_str(htmldata,"{tcpserv1}",strbuf,htmldata);
					itoa(pCfgdata->tcpserv2_port,strbuf,10);
					replace_str(htmldata,"{tcpserv2}",strbuf,htmldata);

					//
					for(i=0;i<nnelen;i++)
					{
						if(htmldata[i]==0x00 || htmldata[i]==0xFF)
						{ break; }
					}
					htmldata[i]=0;
					//
					webSendHeader(i);
					webSendClient(htmldata,i);
					//printf("htmldata=%s\n",htmldata);
				}
		}
		free(htmldata);
	}
	else if(0==strcmp(page,"/command"))
	{
			zhHttpUrlDecode(parameter,strlen(parameter));
			webSendHeader(strlen(parameter));
			webSendClient(parameter,strlen(parameter));
			
			g_webParameter=parameter;
	}
	else if(0==strcmp(page,"/favicon.ico"))
	{
			char* rebuf="��Ѷ���ؼ���!!";
			webSendHeader(strlen(rebuf));
			webSendClient(rebuf,strlen(rebuf));
	}
	else
	{
			//�Ҳ���ҳ��
			webSendClientNotFoundPage();
	}
}

void pfWebServ_Done (char *page)
{
	if(g_webParameter)
	{
		do_command(g_webParameter);
	}
}