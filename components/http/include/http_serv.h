#ifndef __HTTP_H__

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

#ifdef  __cplusplus
extern "C" {
#endif

//���ظ��ͻ��˵���������ṹ
typedef struct _TzhHttpServResponeData  
{
	char sendbuf[500]; //���͵�����.�������500�ֽ�.�ᱻ�ָ���ٷ���
	short sendlen;    //���͵ĳ���
	struct _TzhHttpServResponeData *_next; //��һ���ڵ��ָ��
}TzhHttpServResponeData;

//�������ӵ����ݽṹ
typedef struct _TzhHttpServ
{
	TzhHttpServResponeData *p2;
	char cacheBuf[3000];
	int cacheLen;
}TzhHttpServ;


/*
��ȡ�ļ����Ͳ���
*/
void urlSplit(const char*szUrl,char*dst_file,char* dst_parm);


//�ַ�����ȡ�����ĺ���---------------------------------------
/*
    char *sd="qq=123&bb=66&cc";
    char dd[100];
    zhHttpGetParameter(sd,"bb",dd);
    printf("%s",dd);
*/
int urlGetParameter(const char*str,const char*parameter,char*value);


//--------------------------------------------------------------------------------------------------
//������������

/*
//����
���� =0 �ɹ�
     ��0 ʧ��
*/
int httpServReslovURL(TzhHttpServ* serv,char* recvbuf,int recvlen,char*dstURL);

/*
//������Ӧ
content ���ص����������
http_rspBuf ���緢�͵�����	
jumpUrl ��ת��ҳ��
*/
char* httpServResponeOK(TzhHttpServ* serv,int content_len);
char* httpServResponeNoFound(TzhHttpServ* serv);
char* httpServResponeJumpUrl(TzhHttpServ* serv,const char* jumpUrl);

//���ͻص�ʱ����
TzhHttpServResponeData* httpServResumeData(TzhHttpServ* serv);


#ifdef  __cplusplus
}
#endif

#define __HTTP_H__
#endif
