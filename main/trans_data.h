#ifndef _TRANS_DATA_H_H___
/* GPIO Example

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
#include "eth.h"
#include "event.h"
#include "wifi.h"
#include "hal_i2c.h"
#include "hal_i2s.h"
#include "uart1.h"
#include "webserver.h"
#include "websocket_serv.h"
#include "trans_data_a.h"
#include "mqtt_client.h"

//-------------------------------------------------------
void mqtt_app_start(void);
void mqtt_proc_data(esp_mqtt_client_handle_t client,char* topic,int topic_len,char*buf,int len);

//-------------------------------------------------------
void trans_data_task();

//-------------------------------------------------------
void cfgInit();
void uart_debug_read(const char *buf, int len);
void uart1_read(const char *buf, int len);
void uart2_read(const char *buf, int len);
void websock_read(const char *buf, int len);
void btn_press(int gpio_num, int is_press);
void tcpserv_read(const char *buf, int len);
void tcpserv2_read(const char *buf, int len);
void udp_read(const char *data, int len);
void yun_recv(const char *buf, int len);
void pfWebServ_Callback (char *page, char * parameter);
void pfWebServ_Done (char *page);
//

void cmd_setmqtt(char* commandline);
void cmd_setmqttinfo(char* commandline);

//
void cmd_showuart();
void cmd_setuart1(char* commandline);
void cmd_setuart2(char* commandline);
//
void cmd_restart_sys();
//
void cmd_open_wifi(char* commandline);
void cmd_connect_wifi(char* commandline);
void cmd_close_wifi();
//
void cmd_mqtt_start();
void cmd_mqtt_stop();
void cmd_yun_start();
void cmd_yun_stop();

#define _TRANS_DATA_H_H___
#endif