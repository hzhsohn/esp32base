#ifndef _SCENE_DATA_H_H___

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cfg.h"

//-----------------------------------------------------------------

//打印16进制字符
void print16(char*buf,int len);

//字符串转16进制
int strToHex(const char*str,char* dstBuf,int dstbufLen);
//配对数据判断
//0成功,-1不匹配
int scmpBuf(char*src,int srclen,char*scmpBuf,int scmpBuflen);


#define _SCENE_DATA_H_H___
#endif