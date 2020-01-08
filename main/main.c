/*

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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
#include "wifi.h"
#include "eth.h"
#include "hal_i2c.h"
#include "hal_i2s.h"
#include "uart0.h"
#include "uart1.h"
#include "uart2.h"

#include "webserver.h"
#include "dirent.h"

#include "trans_data.h"
#include "smartconfig.h"

#include "h_gpio.h"
#include "websocket_serv.h"

#include <lwip/api.h>
#include <lwip/dns.h>
#include <lwip/ip.h>

#include "zhHttp.h"
#include "driver/timer.h"
#include "timer_group.h"
#include "spi_data.h"
#include "scene_proc.h"
#include "wifi.h"
#include "mqtt_net_proc.h"
#include "tcpserv.h"
#include "tcpserv2.h"
#include "hxkong_yun.h"
#include "udp_multicast.h"


static esp_err_t event_handler(void *ctx, system_event_t *event);

//-------------------------------------------------------
//mqtt使用的变量
char g_msdA_devuuid[128];
char g_msdA_devname[128];
char g_msdA_devflag[128];
char g_msdA_pub[128];		//设备发布
char g_msdA_subscr[128];	//设备订阅

//定时器
void timegroup(int timerID)
{

}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:// station start
        esp_wifi_connect();
		//led1_on(); //状态LED亮
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED: //station disconnect from ap
		//led1_on(); //状态LED亮
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED: //station connect to ap
        break;
    case SYSTEM_EVENT_STA_GOT_IP:  //station get ip
		{
    		ESP_LOGI("event", "SYSTEM_EVENT_STA_GOT_IP:%s\n",	ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));			
			//led1_close();//状态LED关
			//获取IP后重连MQTT
			mqtt_app_start();
			yun_app_start();
		}
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:// a station connect to ap
    	ESP_LOGI("event", "station:"MACSTR" join,AID=%d\n",
		MAC2STR(event->event_info.sta_connected.mac),
		event->event_info.sta_connected.aid);
    	break;
    case SYSTEM_EVENT_AP_STADISCONNECTED://a station disconnect from ap
    	ESP_LOGI("event", "station:"MACSTR"leave,AID=%d\n",
		MAC2STR(event->event_info.sta_disconnected.mac),
		event->event_info.sta_disconnected.aid);
    	break;
    case SYSTEM_EVENT_ETH_CONNECTED:
		ESP_LOGI("event", "SYSTEM_EVENT_ETH_CONNECTED");
    	break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
		ESP_LOGI("event", "SYSTEM_EVENT_ETH_DISCONNECTED");
    case  SYSTEM_EVENT_ETH_GOT_IP:
    	ESP_LOGI("event", "SYSTEM_EVENT_ETH_GOT_IP:%s\n",	ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
		//获取IP后重连MQTT
		mqtt_app_start();
		yun_app_start();
    default:
        break;
    }
    return ESP_OK;
}

void app_main()
{
	//-----------------------------------------------------------------------
	//关闭调试日志
	/*esp_log_level_set("MQTT_CLIENT",ESP_LOG_NONE);
	esp_log_level_set("system_api",ESP_LOG_NONE);	
	esp_log_level_set("emac",ESP_LOG_NONE);	
	esp_log_level_set("gpio",ESP_LOG_NONE);
	esp_log_level_set("wifi",ESP_LOG_NONE);*/
	
	//基本MAC
	uint8_t base_mac_addr[12]={0};
    esp_read_mac(base_mac_addr, ESP_MAC_WIFI_SOFTAP);	
	esp_base_mac_addr_set(base_mac_addr);
	printf("\nbase mac addr %02x-%02x-%02x-%02x-%02x-%02x\n",
				base_mac_addr[0],base_mac_addr[1],base_mac_addr[2],
				base_mac_addr[3],base_mac_addr[4],base_mac_addr[5]);

	//-----------------------------------------------------------------------
	printf("------------------------------------------------\n");
	printf("--\n");
	printf("-- copyright: www.hx-kong.com\n");
	printf("-- version: mqtt_485 - v1.0\n");
	printf("--\n");
	printf("------------------------------------------------\n");
	printf(">>>>startup system...\n");
	
	printf("adapter init...\n");
	//
	esp_event_loop_init(event_handler,NULL);
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    tcpip_adapter_init();
	//printf("hardware init...\n");

	//单片机初始化设置
	printf("checking mcu init settings...\n");
	TagCfgData *pCfgdata=NULL;
	pCfgdata=(TagCfgData*)get_spi_flash_binary_data(sizeof(TagCfgData),SECTOR_CFG_DATA);
	if(NULL==pCfgdata)
	{
		esp_restart();
	}
	if(pCfgdata->devInitFlag!=INIT_HARDWARE_FLAG)
	{
		initFactoryCfgFlash();
		
		LED_WIFI_OnOff(false);	
		LED_MQTT_OnOff(false);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_WIFI_OnOff(true);	
		LED_MQTT_OnOff(true);	
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_WIFI_OnOff(false);	
		LED_MQTT_OnOff(false);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_WIFI_OnOff(true);	
		LED_MQTT_OnOff(true);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_WIFI_OnOff(false);	
		LED_MQTT_OnOff(false);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_WIFI_OnOff(true);	
		LED_MQTT_OnOff(true);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_WIFI_OnOff(false);	
		LED_MQTT_OnOff(false);

		esp_restart();
	}


	//生成全局常用变量
	sprintf(g_msdA_devuuid,"%s",pCfgdata->devuuid);
	sprintf(g_msdA_devname,"%s",pCfgdata->devname);
	sprintf(g_msdA_devflag,"%s",pCfgdata->devflag);
	sprintf(g_msdA_pub,"%s_",pCfgdata->devname);
	sprintf(g_msdA_subscr,"%s",pCfgdata->devname);
	//
	printf("devuuid = %s\n",g_msdA_devuuid);
	printf("devname = %s\n",g_msdA_devname);
	printf("devflag = %s\n",g_msdA_devflag);
	printf("mqtt_pub = %s\n",g_msdA_pub);
	printf("mqtt_subscr = %s\n",g_msdA_subscr);

	//-----------------------------------------------------------------------
	//初始化网卡
	printf("eht init...\n");
	app_eth();
	
	//-----------------------------------------------------------------------
	//初始化服务
	//printf("timer init...\n");
	//timer_group_init(timegroup,1.0f,0);
	//xTaskCreate(timer_evt_task, "timer_evt_task", 2*1024, NULL, 5, NULL);

	//工作线程	
	xTaskCreate(trans_data_task, "trans_data_task", 6*1024, NULL, 1, NULL);

	printf("gpio init...\n");
	h_gpio_init(btn_press);
	ledInit();
	xTaskCreate(task_gpio_thread, "task_gpio", 4*1024, NULL, 10, NULL);
	
	printf("uart_debug init...\n");
	uart_debug_init(uart_debug_read);
	xTaskCreate(task_uart_debug, "uart_dbg_task", 4*1024, NULL, 4, NULL);

	printf("uart1 init...baudRate=%d\n",pCfgdata->uart1_baudRate);
	uart1_init(uart1_read , pCfgdata->uart1_baudRate);
	uart485Init();
	xTaskCreate(task_uart1, "tag_uart1_task", 4*1024, NULL, 4, NULL);
	
	printf("uart2 init...baudRate=%d\n",pCfgdata->uart2_baudRate);
	uart2_init(uart2_read , pCfgdata->uart2_baudRate);
	xTaskCreate(task_uart2, "tag_uart2_task", 4*1024, NULL, 4, NULL);
	
	printf("web init...port=%d\n",80);
	webserver_init(80,pfWebServ_Callback,pfWebServ_Done);
	xTaskCreate(app_webserver_task_accept, "tweb_task", 4*1024, NULL, 4, NULL);
	xTaskCreate(app_webserver_task_recv, "tweb_task", 6*1024, NULL, 4, NULL);
	
	printf("websocket init...port=%d\n",1000);
	websocket_init(websock_read, 1000);	
	xTaskCreate(app_websocket_task_accept, "websk_accept", 4*1024, NULL, 5, NULL);
	xTaskCreate(app_websocket_task_recv, "websk_recv", 6*1024, NULL, 5, NULL);
	
	printf("tcpserv init...port=%d\n",pCfgdata->tcpserv1_port);
	tcpserv_init(tcpserv_read, pCfgdata->tcpserv1_port);	
	xTaskCreate(app_tcpserv_task_accept, "tcpserv_accept", 4*1024, NULL, 5, NULL);
	xTaskCreate(app_tcpserv_task_recv, "tcpserv_recv", 6*1024, NULL, 5, NULL);
	
	printf("tcpserv init...port=%d\n",pCfgdata->tcpserv2_port);
	tcpserv2_init(tcpserv2_read, pCfgdata->tcpserv2_port);	
	xTaskCreate(app_tcpserv2_task_accept, "tcpserv2_accept", 4*1024, NULL, 5, NULL);
	xTaskCreate(app_tcpserv2_task_recv, "tcpserv2_recv", 6*1024, NULL, 5, NULL);
	
	printf("udp init...\n");
	app_mcast(udp_read);

	if(pCfgdata->is_yun_start)
	{
		printf("yun init..%s:%d\n",pCfgdata->yun_host,pCfgdata->yun_port);
		yun_init(yun_recv , pCfgdata->yun_host , pCfgdata->yun_port);
	}
	///////////////////////////////////////////////////////////////////////
	//初始化其它服务
	cfgInit();
	//WIFI处理
	switch(pCfgdata->mode)
	{
	case ezhWifiModeAP:
		{
			LED_WIFI_OnOff(true);	
			app_wifi_softap(pCfgdata->apSSID,pCfgdata->apPassword);
			printf(">>>Wifi Mode AP ssid = %s , password = %s\n",pCfgdata->apSSID,pCfgdata->apPassword);
		}
		break;
	case ezhWifiModeSTA:
		{
			LED_WIFI_OnOff(true);	
			app_wifi_sta(pCfgdata->staSSID,pCfgdata->staPassword);
			printf(">>>Wifi Mode STA ssid = %s , password = %s\n",pCfgdata->staSSID,pCfgdata->staPassword);
		}
		break;
	default:
		LED_WIFI_OnOff(false);	
		break;
	}

	///////////////////////////////////////////////////////////////////////
	free(pCfgdata);
	pCfgdata=NULL;
	///////////////////////////////////////////////////////////////////////
	printf(">>>>print system info.\n");	
	esp_chip_info_t chip_info;
	esp_chip_info( &chip_info );
	printf("FLASH SIZE: %d MB (%s) \r\n", spi_flash_get_chip_size() / (1024 * 1024),
										  (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
	printf("Free memory: %d bytes \r\n", esp_get_free_heap_size());
	printf(">>>>successfully.\n");
	
}

