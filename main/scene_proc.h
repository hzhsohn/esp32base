#ifndef _SCENE_DATA_H_H___

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cfg.h"

//-----------------------------------------------------------------

//��ӡ16�����ַ�
void print16(char*buf,int len);

//�ַ���ת16����
int strToHex(const char*str,char* dstBuf,int dstbufLen);
//��������ж�
//0�ɹ�,-1��ƥ��
int scmpBuf(char*src,int srclen,char*scmpBuf,int scmpBuflen);


#define _SCENE_DATA_H_H___
#endif