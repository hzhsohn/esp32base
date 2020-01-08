#ifndef _MQTT_NET_PROC_H_

#include <stdio.h>
#include <ctype.h>
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

#ifdef  __cplusplus
extern "C" {
#endif


	void mqttNetData(const char*buff,int len);



#ifdef  __cplusplus
}
#endif


#define _MQTT_NET_PROC_H_
#endif
