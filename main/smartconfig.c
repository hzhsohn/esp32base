/* 
  Copyright (C) 2017-12, Han.zhihong, Developer. Created 2017. 
  All rights reserved.
  License to copy and use this software is granted provided that it
  is identified as the "Han.zhihong. Message-Digest
  Algorithm" in all material mentioning or referencing this software
  or this function.
  
  Han.zhihong. makes no representations concerning either
  the merchantability of this software or the suitability of this
  software for any particular purpose. It is provided "as is"
  without express or implied warranty of any kind.
	  
  These notices must be retained in any copies of any part of this
  documentation and/or software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "esp_smartconfig.h"
#include "freertos/event_groups.h"
#include "smartconfig.h"


static EventGroupHandle_t smartconfig_event_group = NULL;

static esp_err_t wifi_event_handler_smartcfg(void *ctx, system_event_t *event)
{
	wifi_ap_record_t my_wifi_info;
	
    switch(event->event_id) 
	{
		case SYSTEM_EVENT_STA_START:
			printf("[WIFI STA] STA start event!\r\n");
			xEventGroupWaitBits(smartconfig_event_group, LINKED_BIT, false, true, portMAX_DELAY);
			ESP_ERROR_CHECK( esp_wifi_connect() );
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			printf("[WIFI STA] Get IP event!\r\n");
			printf("[WIFI STA] ESP32 IP: %s !\r\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			
			break;
		case SYSTEM_EVENT_STA_CONNECTED:                     
			printf("[WIFI STA] Wifi STA connect event!\r\n");
			esp_wifi_sta_get_ap_info(&my_wifi_info);       // STA模式下，获取模块连接上的wifi热点的信息
			printf("[WIFI STA] Connect to : %s!\r\n", my_wifi_info.ssid);
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			printf("[WIFI STA] Wifi STA disconnect event, reconnect!\r\n");
			ESP_ERROR_CHECK( esp_wifi_connect() );
			xEventGroupClearBits(smartconfig_event_group, CONNECTED_BIT);
			break;
		default:
			break;
    }
	
    return ESP_OK;
}

void smartconfig_handler(smartconfig_status_t status, void *pdata)
{
	switch (status) 
	{
		case SC_STATUS_WAIT:
			printf("[WIFI SMARTCONFIG] SC_STATUS_WAIT event!\r\n");
			break;
		case SC_STATUS_FIND_CHANNEL:
			printf("[WIFI SMARTCONFIG] SC_STATUS_FIND_CHANNEL event!\r\n");
			break;
		case SC_STATUS_GETTING_SSID_PSWD:
			printf("[WIFI SMARTCONFIG] SC_STATUS_GETTING_SSID_PSWD event!\r\n");
			smartconfig_type_t *type = pdata;
			if (*type == SC_TYPE_ESPTOUCH) 
			{
				printf("[WIFI SMARTCONFIG] SC_TYPE:SC_TYPE_ESPTOUCH \r\n");
			} 
			else 
			{
				printf("[WIFI SMARTCONFIG] SC_TYPE:SC_TYPE_AIRKISS \r\n");
			}
			break;
		case SC_STATUS_LINK:
			printf("[WIFI SMARTCONFIG] SC_STATUS_LINK event!\r\n");
			wifi_config_t wifi_config;
			wifi_config.sta = *((wifi_sta_config_t *)pdata);
			esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

			esp_wifi_disconnect();
			xEventGroupSetBits(smartconfig_event_group, LINKED_BIT);
			//esp_wifi_connect();
			break;
		case SC_STATUS_LINK_OVER:
			printf("[WIFI SMARTCONFIG] SC_STATUS_LINK_OVER event!\r\n");
			if (pdata != NULL) 
			{
				uint8_t phone_ip[4] = { 0 };

				memcpy(phone_ip, (uint8_t *) pdata, 4);
				printf("[WIFI SMARTCONFIG] Phone ip: %d.%d.%d.%d \r\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
			}
			esp_smartconfig_stop();
			xEventGroupSetBits(smartconfig_event_group, CONNECTED_BIT);
			break;
	}
}

void user_wifi_sta_init()
{
	printf("[WIFI STA INIT] WIFI Init start!\r\n");
	
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler_smartcfg, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
}

void smartconfig_init()
{
    printf("[WIFI SMARTCONFIG] =.= Smartconfig Init start!\r\n");
    smartconfig_event_group = xEventGroupCreate();
    user_wifi_sta_init();
    // wechat airkiss
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS));  
	  ESP_ERROR_CHECK(esp_smartconfig_start(smartconfig_handler));
    xEventGroupWaitBits(smartconfig_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);  // 等待配置入网成功
	
    printf("[WIFI SMARTCONFIG] Smartconfig ok!\r\n");	
}