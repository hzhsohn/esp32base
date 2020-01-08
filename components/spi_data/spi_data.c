#include "spi_data.h"

#include "string.h"
#include "esp_err.h"
/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_vfs_fat.h"
/* Utils includes. */
#include "esp_log.h"

#include "esp_spi_flash.h"
#include <freertos/semphr.h>
#include "driver/timer.h"
#include "esp_intr_alloc.h"

int write_spi_flash_data(const uint8_t*data,int datalen,int sector)
{
	//printf("write_spi_flash_data sector=%d datalen=%d\n",sector,datalen);
	if (spi_flash_erase_sector(sector) == ESP_OK) {
		 if (spi_flash_write(sector * SPI_FLASH_SEC_SIZE, data, datalen) == ESP_OK) {
			//printf("Write flash ok\n");
			return 0;
		 }
	}
	return 1;
}

char* get_spi_flash_binary_data(int read_len,int sector)
{
	char*buf;
	buf=(char*)malloc(read_len);
	memset(buf,0,read_len);
	if (spi_flash_read(sector * SPI_FLASH_SEC_SIZE, (uint8_t *) buf, read_len)!= ESP_OK)
	{
		//printf("get_spi_flash_binary_data error. sector=%d\n", sector);
		free(buf);
		buf=NULL;
		return NULL;
	}
	/*else
	{
		printf("get_spi_flash_binary_data ok. sector=%d len=%d\n", sector,read_len);
	}*/
	return buf;
}

char* get_spi_flash_text_data(int *buf_len,int sector,int sector_count)
{
	char*buf;
	int ret=0;
	int len=4096*sector_count;
	if(0==sector_count)
	{
		*buf_len=0;
		return NULL;
	}
	buf=(char*)malloc(len); //8K页面
	memset(buf,0,len);
	if (spi_flash_read(sector * SPI_FLASH_SEC_SIZE, (uint8_t *) buf, len-1)!= ESP_OK)
	{
		//printf("get_spi_flash_text_data error. sector=%d\n", sector);
		free(buf);
		buf=NULL;
		*buf_len=0;
		return NULL;
	}
	else
	{
		ret=0;
		while(ret<len)
		{
			if(0x00==buf[ret] || 0xFF==buf[ret])
			{
				buf[ret]=0x00;
				break;
			}
			ret++;
		}
		//printf("get_spi_flash_text_data ok. sector=%d len=%d\n", sector,ret);
	}
	*buf_len=ret;
	return buf;
}