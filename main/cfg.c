#include "cfg.h"
#include "trans_data.h"
#include "spi_data.h"
#include "cJSON.h"

//出厂初始化
void initFactoryCfgFlash()
{	
	printf("do initFactoryCfgFlash\n");
	char *pdefjson=(char*)get_spi_flash_binary_data(4095,SECTOR_CFG_JSON);

	//
	int i=0;
	for(i=0;i<4095;i++)
	{
		if(pdefjson[i]==0x00 || pdefjson[i]==0xFF)
		{ break; }
	}
	pdefjson[i]=0;
	printf("pdefjson=%s\n",pdefjson);
	//////////////////////////////////
	//解释JSON结构
	cJSON *json;
	json=cJSON_Parse(pdefjson);
	if(json)
	{
			char ssbuf[255]={0};
			cJSON *jsonVal;	
				
			TagCfgData cfgdata={0};
			cfgdata.devInitFlag=INIT_HARDWARE_FLAG;			
			//
			jsonVal=cJSON_GetObjectItem(json,"uart1_baudRate");
			cfgdata.uart1_baudRate=jsonVal->valueint;
			//
			uint8_t mac_addr[12]={0};
			esp_read_mac(mac_addr, ESP_MAC_WIFI_SOFTAP);
			//
			//printf("--------------------------1");
			jsonVal=cJSON_GetObjectItem(json,"is_wifi_start");
			if(jsonVal->valueint)
			{
				cfgdata.mode=ezhWifiModeAP;
			}
			else
			{
				cfgdata.mode=ezhWifiModeUndo;
			}
			//
			jsonVal=cJSON_GetObjectItem(json,"apssid");
			strncpy(ssbuf,jsonVal->valuestring,64);
			sprintf(cfgdata.apSSID,"%s%02x%02x%02x",ssbuf,mac_addr[3],mac_addr[4],mac_addr[5]);
			//
			//printf("--------------------------2");
			jsonVal=cJSON_GetObjectItem(json,"apssid_password");
			strncpy(ssbuf,jsonVal->valuestring,30);
			sprintf(cfgdata.apPassword,"%s",ssbuf);
			//printf("--------------------------3");
			//
			sprintf(cfgdata.devuuid,"HXK%02X%02X%02X%02X%02X%02X%d",mac_addr[0],mac_addr[1],mac_addr[2],
														mac_addr[3],mac_addr[4],mac_addr[5],rand()%1000);
			//
			jsonVal=cJSON_GetObjectItem(json,"devname");
			strncpy(ssbuf,jsonVal->valuestring,64);
			sprintf(cfgdata.devname,"%s%02x%02x%02x",ssbuf,mac_addr[3],mac_addr[4],mac_addr[5]);
			//
			jsonVal=cJSON_GetObjectItem(json,"devflag");
			strncpy(ssbuf,jsonVal->valuestring,64);
			strcpy(cfgdata.devflag,ssbuf);
			//
			jsonVal=cJSON_GetObjectItem(json,"mqtt_host");
			strncpy(ssbuf,jsonVal->valuestring,200);
			strcpy(cfgdata.mqtt_host,ssbuf);
			//
			jsonVal=cJSON_GetObjectItem(json,"mqtt_port");
			cfgdata.mqtt_port=jsonVal->valueint;
			//
			jsonVal=cJSON_GetObjectItem(json,"is_mqtt_start");
			cfgdata.is_mqtt_start=jsonVal->valueint;
			//
			jsonVal=cJSON_GetObjectItem(json,"tcpserv1_port");
			cfgdata.tcpserv1_port=jsonVal->valueint;
			//
			write_spi_flash_data((uint8_t*)&cfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
			//
			cJSON_Delete(json);
	}

	
	//
	free(pdefjson);
}

//////////////////////////////////////////////////////
//
// 执行操作指令 
//
void do_command(char* Cmdline)
{	
	char command[196]={0};
	fflush(stdin);
	sscanf(Cmdline,"%s",command);
	trim(command);

	printf("--------------------------------------------------\n");
	printf("command=%s\n",command);
	printf("--------------------------------------------------\n\n");
			
	if(0==strcmp("help",command))
	{
		printf("depend command:\n");
		printf("help                                     -- help information\n");
		printf("factory                                  -- factory setting\n");				
		printf("restart                                  -- restart system\n");	
		printf("openwifi <ssid>,<password>               -- open wifi and set ap mode\n");
		printf("connectwifi <ssid>,<password>            -- open wifi and connect router\n");
		printf("closewifi                                -- close wifi\n");
		printf("showuart                                 -- show uart infomation\n");
		printf("setuart1 <baudrate>                      -- set uart1 baudrate\n");
		printf("showmqtt                                 -- show mqtt info\n");
		printf("setmqtt <ip>,<port>                      -- set mqtt ip and port\n");
		printf("mqttstart                                -- mqtt start\n");
		printf("mqttstop                                 -- mqtt stop\n");
		printf("\n\n");
	}
	else if(0==strcmp("factory",command))
	{
		initFactoryCfgFlash();
		cmd_restart_sys();
	}
	else if(0==strcmp("restart",command))
	{
		cmd_restart_sys();
	}
	else if(0==strcmp("openwifi",command))
	{
		cmd_open_wifi(Cmdline);
	}
	else if(0==strcmp("connectwifi",command))
	{
		cmd_connect_wifi(Cmdline);
	}
	else if(0==strcmp("closewifi",command))
	{
		cmd_close_wifi();
	}
	else if(0==strcmp("showuart",command))
	{
		cmd_showuart();
	}
	else if(0==strcmp("setuart1",command))
	{
		cmd_setuart1(Cmdline+strlen(command)+1);
	}
	else if(0==strcmp("showmqtt",command))
	{				
		showmqtt();
	}
	else if(0==strcmp("mqttstart",command))
	{
		cmd_mqtt_start();
	}
	else if(0==strcmp("mqttstop",command))
	{
		cmd_mqtt_stop();
	}
	else if(0==strcmp("setmqtt",command))
	{
		cmd_setmqtt(Cmdline);
	}
	else if(0==strcmp("setmqttinfo",command))
	{
		cmd_setmqttinfo(Cmdline);
	}
	else if(strlen(command)>0)
	{
		//未知指令
		printf("unkown command\n");
	}
}
