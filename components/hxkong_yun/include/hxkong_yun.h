#ifndef _HXKONG_YUN_PROC_H__

#include "mini-data.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _EzhDevNetProtocol //命令标识,传输用一个字节uchar
{
	ezhDNProl_Data			=0,	//[uchar buf] 传输到服务器的数据
	ezhDNProl_Data_Ret		=1, //[uchar buf] 传输到服务器的数据
	ezhDNProl_Keep			=2, //<NULL> 定时给服务器发送,如果服务器收不到ezhDNProl_RTT视为断开
	ezhDNProl_Keep_Ret		=3, //<NULL>回复到客户端的RTT时间
	ezhDNProl_SignInDev		=4, //[string DPID][string devUUID]连接成功后,服务器给的回复
	ezhDNProl_SignInDev_Ret	=5,	//<NULL>接入回复
	ezhDNProl_SetDevInfo	=6, //[string CAID][string devflag][string devName]SignIn成功后,服务器给的回复
	ezhDNProl_SetDevInfo_Ret=7,	//<NULL>接入回复
}EzhTransDevNetProtocol;

typedef void PF_HXKONG_YUN_RECV (const char *buf, int len);


//初始化线程使用
void yun_init(PF_HXKONG_YUN_RECV* pf, char* host ,int port);
//打开连接
void yun_app_start();

//服务器过来的数据处理
void actionTransServDataFunc(char cmd, uchar* param, int len);

//发送数据
void iotYunSend(char* buf,ushort len);

//------------------------------------------------------------
//签入硬件到转发服务器
void actionSignIn(const char* DPID,const char* devUUID);
//
void actionSetDevInfo(const char* CAID ,
										const char* devflag ,
										const char* devName);

//更新RTT信息到服务器
void actionKeep();
    
#ifdef __cplusplus
}
#endif
#define _HXKONG_YUN_PROC_H__
#endif