#include "trans_data_a.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include <sys/socket.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "eth.h"
#include "event.h"
#include "wifi.h"
#include "hal_i2c.h"
#include "hal_i2s.h"
#include "uart1.h"
#include "webserver.h"
#include "websocket_serv.h"
#include "uart0.h"
#include "uart1.h"
#include "uart2.h"
#include "spi_data.h"
#include "scene_proc.h"
#include "h_gpio.h"
#include "udp_multicast.h"

void ledInit()
{
    gpio_config_t io_conf; 
	//���-----------------------------------------------------    
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO0_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	
	//���-----------------------------------------------------    
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO1_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	
}

void uart485Init()
{
	//���-----------------------------------------------------
    gpio_config_t io_conf;                      // ����GPIOʵ��
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_485DIR_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	//
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_485DIR2_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	//	
	gpio_set_level(GPIO_485DIR_IO, 0);
	gpio_set_level(GPIO_485DIR2_IO, 0);

}
void uart1485Send(char*buf,int len)
{
	gpio_set_level(GPIO_485DIR_IO, 1);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	uart1_send(buf,len);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_485DIR_IO, 0);
}
void uart2485Send(char*buf,int len)
{
	gpio_set_level(GPIO_485DIR2_IO, 1);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	uart2_send(buf,len);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_485DIR2_IO, 0);
}

void showmqtt()
{
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	//
	printf("-mqtt infomation-----------------------------------\n");
	printf("host=%s \n",pCfgdata->mqtt_host);
	printf("port=%d \n",pCfgdata->mqtt_port);
	printf("client_id=%s \n",pCfgdata->mqtt_clientid);
	printf("username=%s \n",pCfgdata->mqtt_username);
	printf("password=%s \n",pCfgdata->mqtt_password);
	printf("\n");
	//
	free(pCfgdata);
	pCfgdata=NULL;
}

//����MQTT��Ϣ
int setMqtt(char*host,int port)
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	strcpy(pCfgdata->mqtt_host, host);
	pCfgdata->mqtt_port=port;
	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	//����MQTT����

	return ret;
}
int setMqttInfo(char*client_id,char*username,char*password)
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	strcpy(pCfgdata->mqtt_clientid, client_id);
	strcpy(pCfgdata->mqtt_username, username);
	strcpy(pCfgdata->mqtt_password, password);
	
	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	return ret;
}

//����WIFI��Ϣ
int setConnectWifi(char*ssid,char*password)
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	strcpy(pCfgdata->staSSID, ssid);
	strcpy(pCfgdata->staPassword, password);
	pCfgdata->mode=ezhWifiModeSTA;

	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	esp_restart();
	return ret;

}

//����WIFI��Ϣ
int setOpenWifi(char*ssid,char*password)
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	strcpy(pCfgdata->apSSID, ssid);
	strcpy(pCfgdata->apPassword, password);
	pCfgdata->mode=ezhWifiModeAP;

	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	esp_restart();
	return ret;

}

//����WIFI��Ϣ
int setCloseWifi()
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	strcpy(pCfgdata->apSSID, "");
	strcpy(pCfgdata->apPassword, "");
	strcpy(pCfgdata->staSSID, "");
	strcpy(pCfgdata->staPassword, "");
	pCfgdata->mode=ezhWifiModeUndo;

	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	esp_restart();
	return ret;
}

int setMqttStartAndStop(int b)
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	pCfgdata->is_mqtt_start=b;	
	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	return ret;
}

int setYunStartAndStop(int b)
{
	int ret=0;
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	pCfgdata->is_yun_start=b;	
	ret=write_spi_flash_data((uint8_t*)pCfgdata,sizeof(TagCfgData),SECTOR_CFG_DATA);
	free(pCfgdata);
	pCfgdata=NULL;
	//
	return ret;
}

///////////////////////////////
//ȥ���ַ����ո�
char *  ltrim( char *str ) {
    /**ȥ����߿ո�**/
    int length;
    char *i;
    char *len;
    int m = 0;
    int n = 0;

	if(NULL==str){return NULL;}

	length = strlen( str );
    i = str;
    len = str + length;
    
    for (; i<len; i++ ) {
        if ( *i == ' ' || *i == '\t' || *i == '\n' ) {
            n ++;
        } else {
            break;
        }
    }
    for ( m=0; m<=length-n; m++ ) {
        *(str + m) = *(str + n + m);
    }
    return str;
}        

/**ȥ���ұ߿ո�**/
char *  rtrim( char *str) {
    char *i;	
	if(NULL==str){return NULL;}
    i = str + strlen( str ) - 1;    
    for (; i>=str; i-- ) {
        if ( *i == ' ' || *i == '\t' || *i == '\n' ) {
            *(str + strlen(str) -1) = '\0';
        } else {
            break;
        }
    }
    return str;                                                                                                                            
}

/**ȥ�����߿ո�**/
char *  trim(char *str)
{
    ltrim(str);
    rtrim(str);
    return str;
}



//�ظ�����Ӳ����Ϣ
int searchFrameData(char*sendBuf,const char* devUUID,
					const char* devname,const char* devflag,
					const char* publish,const char* subscr)
{
	int sendLen=0;
	int dsrlen=0;
	char* gen_json;

	gen_json=(char*)malloc(500);

	sendBuf[0]=0x00;
	gen_json[0]=0x00;

	//�ظ���־
	sendBuf[sendLen]='R';
	sendLen++;
	sendBuf[sendLen]='M';
	sendLen++;
	sendBuf[sendLen]='S';
	sendLen++;
	sendBuf[sendLen]='D';
	sendLen++;
	
	//����
	sendBuf[sendLen]=',';
	sendLen++;

	//ȫ��Ψһ��ʶ
	dsrlen=strlen(devUUID);
	memcpy(&sendBuf[sendLen],devUUID,dsrlen);
	sendLen+=dsrlen;

	//����
	sendBuf[sendLen]=',';
	sendLen++;

	//Ӳ����ʶ
	strcat(gen_json,"{\"f\":\"");
	strcat(gen_json,devflag);
	//Ӳ������
	strcat(gen_json,"\",\"n\":\"");
	strcat(gen_json,devname);
	//���ĵ�����
	strcat(gen_json,"\",\"s\":\"");
	strcat(gen_json,subscr);
	//����������
	strcat(gen_json,"\",\"p\":\"");
	strcat(gen_json,publish);
	//
	strcat(gen_json,"\"}");

	//JSON����
	dsrlen=strlen(gen_json)+1;
	memcpy(&sendBuf[sendLen],gen_json,dsrlen);
	sendLen+=dsrlen;

	free(gen_json);
	return sendLen;
}


//////////////////////////////////////////////////////////////////
//UDP�ظ�����Ӳ����Ϣ
void isSearchFrameData(const char* devUUID,
					const char* devname,
					const char* devflag)
{
	char* sendBuf;

	sendBuf=(char*)malloc(500);
	sendBuf[0]=0x00;
	//
	sprintf(sendBuf,"RMSD,%s,{\"f\":\"%s\",\"n\":\"%s\"}",devUUID,devflag,devname);
	sendUDPLastIP((char*)sendBuf,strlen(sendBuf));
	free(sendBuf);
}
