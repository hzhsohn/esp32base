/*
 Author：han.zhihong
*/

#ifndef _SPI_DATA_H__
#define _SPI_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <dirent.h>

/*
//写入数据
返回 0写入成功
		1失败
*/
int write_spi_flash_data(const uint8_t*data,int datalen,int sector);

/* 例子:使用完后,需要释放内存
char*buf=NULL;
buf=get_spi_flash_binary_data(99,160);
if(buf)
{
	free(buf);
	buf=NULL;
}
*/
char* get_spi_flash_binary_data(int read_len,int sector);

/* 例子:使用完后,需要释放内存
char*buf=NULL;
int len=0;
buf=get_spi_flash_text_data(&len,160,2);
if(buf)
{
	free(buf);
	buf=NULL;
}
*/
char* get_spi_flash_text_data(int *buf_len,int sector,int sector_count);

#ifdef __cplusplus
}
#endif

#endif /*#ifndef _SPI_DATA_H__*/