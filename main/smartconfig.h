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

#ifndef __USER_SMARTCONFIG_1_H__
#define __USER_SMARTCONFIG_1_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
sample:
#include "smartconfig.h"


void app_main()
{
  // get system info
  esp_chip_info_t chip_info;
  esp_chip_info( &chip_info );

	printf("========================================================\r\n");
	printf("[WIFI SMARTCONFIG] Example : Smartconfig WeChat! \r\n");
	printf("[WIFI SMARTCONFIG] FLASH SIZE: %d MB (%s) \r\n", spi_flash_get_chip_size() / (1024 * 1024),
	                                                         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
	printf("[WIFI SMARTCONFIG] Free memory: %d bytes \r\n", esp_get_free_heap_size());
	
	
	smartconfig_init();
}

*/

#define LINKED_BIT      (BIT0)
#define CONNECTED_BIT   (BIT1)

void user_wifi_sta_init();
void smartconfig_init();

#ifdef __cplusplus
}
#endif

#endif /*#ifndef __USER_SMARTCONFIG_1_H__*/