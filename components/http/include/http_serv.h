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

//发回给客户端的数据链表结构
typedef struct _TzhHttpServResponeData  
{
	char sendbuf[500]; //发送的数据.如果大于500字节.会被分割后再发送
	short sendlen;    //发送的长度
	struct _TzhHttpServResponeData *_next; //下一个节点的指针
}TzhHttpServResponeData;

//单个连接的数据结构
typedef struct _TzhHttpServ
{
	TzhHttpServResponeData *p2;
	char cacheBuf[3000];
	int cacheLen;
}TzhHttpServ;


/*
获取文件名和参数
*/
void urlSplit(const char*szUrl,char*dst_file,char* dst_parm);


//字符串获取参数的函数---------------------------------------
/*
    char *sd="qq=123&bb=66&cc";
    char dd[100];
    zhHttpGetParameter(sd,"bb",dd);
    printf("%s",dd);
*/
int urlGetParameter(const char*str,const char*parameter,char*value);


//--------------------------------------------------------------------------------------------------
//服务器处理函数

/*
//解析
返回 =0 成功
     非0 失败
*/
int httpServReslovURL(TzhHttpServ* serv,char* recvbuf,int recvlen,char*dstURL);

/*
//生成响应
content 返回到浏览器内容
http_rspBuf 网络发送的内容	
jumpUrl 跳转的页面
*/
char* httpServResponeOK(TzhHttpServ* serv,int content_len);
char* httpServResponeNoFound(TzhHttpServ* serv);
char* httpServResponeJumpUrl(TzhHttpServ* serv,const char* jumpUrl);

//发送回调时调用
TzhHttpServResponeData* httpServResumeData(TzhHttpServ* serv);


#ifdef  __cplusplus
}
#endif

#define __HTTP_H__
#endif
