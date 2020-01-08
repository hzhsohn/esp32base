#include "scene_proc.h"
#include "spi_data.h"


//��ӡ16������Ϣ
void print16(char*buf,int len)
{
	for(int i=0;i<len;i++)
	{
		printf("%02X ",buf[i]);
	}
}

/*
�ַ���ת16��������
���� -1 �����Ż��治��
����0 ת����ĳ���
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
	
	//�ڴ�ת��
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


//��������ж�
//0�ɹ�,-1��ƥ��
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
