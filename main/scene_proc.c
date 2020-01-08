#include "scene_proc.h"
#include "spi_data.h"


//打印16进制信息
void print16(char*buf,int len)
{
	for(int i=0;i<len;i++)
	{
		printf("%02X ",buf[i]);
	}
}

/*
字符串转16进制数据
返回 -1 代表存放缓存不足
大于0 转换后的长度
*/
int strToHex(const char*str,char* dstBuf,int dstbufLen)
{
	char* tszTmp,*tpsz,*tpszsk;
	int nTmp=0;
	int nHex=0;

	nHex=strlen(str)+1;
	tszTmp=(char*)malloc(nHex);
	memset(tszTmp,0,nHex);
	strcpy(tszTmp,str);
	
	//内存转换
	tpsz=strtok_r(tszTmp," ",&tpszsk);
	if(tpsz)
	{
		do
		{
			if(nTmp<dstbufLen)
			{
				//printf("tpsz=%s\n",tpsz);
				sscanf(tpsz,"%x",&nHex);
				dstBuf[nTmp]=nHex;
				nTmp++;
			}
			else
			{
				nTmp=-1;
				break;			
			}
		}while((tpsz=strtok_r(NULL," ",&tpszsk)));
	}
	free(tszTmp);
	tszTmp=NULL;
	return nTmp;
}


//配对数据判断
//0成功,-1不匹配
int scmpBuf(char*src,int srclen,char*scmpBuf,int scmpBuflen)
{
	int i=0;	
	if(0==scmpBuflen)
	{
		return -1;
	}
	for(i=0;i<srclen;i++)
	{		
		int n=0;
		for(n=0;n<scmpBuflen;n++)
		{
			if(src[i]==scmpBuf[n])
			{ i++; }
			else
			{ break; }
		}
		if(n==scmpBuflen)
		{ return 0; }
	}
	return -1;
}
