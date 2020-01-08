#include "http_serv.h"

//查找HTTP协议头尾
char* getbegnstr(char*buf);
char* rnrnstr(char*buf);

// 增加了一个end参数，这样不必提取出字符串后，再做剔除处理
void zhMatchString(const char* src, const char* pattern, char end,char*dstBuf);

//////////////////////////////////////////
char* getbegnstr(char*buf)
{
	char *p;
	p=buf;
	while(*p!=0)
	{
		if(*p=='G' || *p=='g')
		if(*(p+1)=='E' || *(p+1)=='e')
		if(*(p+2)=='T' || *(p+2)=='t')
		if(*(p+3)==' ')
			return p;

		if(*p=='P' || *p=='p')
		if(*(p+1)=='O' || *(p+1)=='o')
		if(*(p+2)=='S' || *(p+2)=='s')
		if(*(p+3)=='T' || *(p+2)=='t')
		if(*(p+4)==' ')
			return p;
		p++;
	}
	return NULL;
}
char* rnrnstr(char*buf)
{
	char *p;
	p=buf;
	while(*p!=0)
	{
		if(*p==0x0D)
		if(*(p+1)==0x0A)
		if(*(p+2)==0x0D)
		if(*(p+3)==0x0A)
			return p;

		p++;
	}
	return NULL;
}

// 增加了一个end参数，这样不必提取出字符串后，再做剔除处理
void zhMatchString(const char* src, const char* pattern, char end,char*dstBuf)
{
		int src_len,ptn_len;
		unsigned short b=0, p=0, i=0;
		char c;
        src_len = strlen(src); 
        ptn_len = strlen(pattern);
        
        for(i=0; i<src_len; i++){
                c = src[i];
                if(p==ptn_len){ // p==ptn_len 表示正在匹配中
                        if(c=='\r' || c=='\n'  || (end !='\0' && c==end) ) p++; // 匹配结束
                        else dstBuf[b++]=c; // 匹配到的字符 
                }else if(p<ptn_len){ // 为达到匹配要求
                        if(c==pattern[p]) p++;
                        else p=0;
                }
        }         
        dstBuf[b]=0;
}

//字符串获取参数的函数---------------------------------------
/*
    char *sd="qq=123&bb=66&cc";
    char dd[100];
    urlGetParameter(sd,"bb",dd);
    printf("%s",dd);

	返回值
	1成功
	0失败
*/
int urlGetParameter(const char*str,const char*parameter,char*value)
{
	 //format is "a=123&b=456"
	 #define SPLIT_1         "&"
	 #define SPLIT_2         "="
	 int bRet;
	 char *pSplit,*pSplit2;
	 char *pPara;
	 char *p1,*p2;
	 char *pszStr;
	 int nStrLen;
 
	 bRet=0;
	 nStrLen=strlen(str);

	 //限制长度
	 if(nStrLen>2048)
	 {return 0;}
 
	 if (0==nStrLen) {
	  return bRet;
	 }
 
	 pszStr=(char*)malloc(nStrLen+1);
	 memset(pszStr, 0, nStrLen+1);
	 strcpy(pszStr,str);
 
	 pPara=strtok_r(pszStr,SPLIT_1, &pSplit);
 
	 do{
		  p1=strtok_r(pPara, SPLIT_2, &pSplit2);
		  p2=strtok_r(NULL, SPLIT_2, &pSplit2);
		  if (0==strcmp(parameter, p1)) {
		   if (p2) {
			strcpy(value, p2);
			bRet=1;
		   }
		   break;
		  }
	 }while ((pPara=strtok_r(NULL,SPLIT_1, &pSplit)));
	 free(pszStr);
	 pszStr=NULL;
	 return bRet;
}


/*

返回值
	1成功
	0失败

char dst_host[512];
int dst_port;
char dst_file[512];
char dst_pram[2048];

*/
void urlSplit(const char*szUrl,char*dst_file,char* dst_parm)
{
	int bufsize=0; 
	int i=0;
 
	i=0;
		 
	//获取文件路径 
	bufsize=0;
	while(szUrl[i]!='?' && szUrl[i]!=0) {
		dst_file[bufsize]=szUrl[i];
		bufsize++;
		i++;
	}
	dst_file[bufsize]=0;
 
	//获取问号后面的参数
	bufsize=0;
	i++;
	while(szUrl[i]!=0)
	{
		dst_parm[bufsize]=szUrl[i];
		bufsize++;
		i++;
	}
	dst_parm[bufsize]=0;
}

/*

	返回值:
	=0 成功解析
	=1 缓冲区错误
*/
int httpServReslovURL(TzhHttpServ*serv,char* recvbuf,int recvlen,char*dstURL)
{
	int tmp;
	tmp = serv->cacheLen + recvlen;
	if(tmp<sizeof(serv->cacheBuf))
	{
		char *ppheader;
		char *pheadstr;
		char *pstr;

		memcpy(&serv->cacheBuf[serv->cacheLen],recvbuf,recvlen);
		serv->cacheLen=tmp;

		//获取开头
		ppheader=serv->cacheBuf;
		pheadstr=getbegnstr(serv->cacheBuf);
		if(NULL==pheadstr)
		{
			serv->cacheLen=0;
			return 2;
		}
		//获取结尾		
		pstr=(char *)rnrnstr(serv->cacheBuf);
		if(NULL==pstr)
		{
			return 3;
		}
		
		//获取GET
		if(0==memcmp(pheadstr,"GET",3))
		{
			zhMatchString(pheadstr, "GET ", 0x20,dstURL); // 提取空格之前
		}
		else if(0==memcmp(pheadstr,"POST",4))
		{
			zhMatchString(pheadstr, "POST ", 0x20,dstURL); // 提取空格之前
		}
		serv->cacheLen=0;//清空所有数据
		return 0;
	}

	serv->cacheLen=0;
	return 1;
}

//生成响应
char* httpServResponeOK(TzhHttpServ* serv,int content_len)
{
	//////////////////////////////////////////
	//组建发送回去HTTP的内容
	char* http_rsp=(char*)malloc(500);
	http_rsp[0]=0x00;
	sprintf(http_rsp,"HTTP/1.1 200 OK\r\n\
Server：hx-httpd (hx-mcu)\r\n\
Content-Length：%d\r\n\
Content-Type: text/html\r\n\
Keep-Alive: timeout=5, max=100\r\n\
Connection：Close\r\n\r\n",content_len);
	return http_rsp;	
}

//生成响应
char* httpServResponeNoFound(TzhHttpServ* serv)
{
	//////////////////////////////////////////
	//组建发送回去HTTP的内容
	char* http_rsp=(char*)malloc(500);
	http_rsp[0]=0x00;
	char content[]="404 not found";

	sprintf(http_rsp,"HTTP/1.1 404 Not Found\r\n\
Server：hx-httpd (hx-mcu)\r\n\
Content-Length：%d\r\n\
Content-Type: text/html\r\n\
Keep-Alive: timeout=5, max=100\r\n\
Connection：Close\r\n\r\n%s",strlen(content),content);
	return http_rsp;
}

//生成页面跳转
char* httpServResponeJumpUrl(TzhHttpServ* serv,const char* jumpUrl)
{
	//////////////////////////////////////////
	//组建发送回去HTTP的内容
	char* http_rsp=(char*)malloc(500);
	http_rsp[0]=0x00;
	sprintf(http_rsp,"HTTP/1.1 302 Found\r\n\
Server：hx-httpd (hx-mcu)\r\n\
Content-Length：0\r\n\
Content-Type: text/html\r\n\
Keep-Alive: timeout=5, max=100\r\n\
Location: %s\r\n\
Connection：Close\r\n\r\n",jumpUrl);
	return http_rsp;
}
