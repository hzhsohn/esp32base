/*
 此模块针对MCU间的通讯协议
 
 //-----------------------------------------------
 例子:
 //--接收每条指令的最大接收缓存长度
 //通讯无校验
 //最短结构为4个字节
 //uchar head;  以0xAF开头
 //ushort parameter_len; 长度不大于65545个字节
 //uchar* parameter; 实际传输内容
 //uchar end;   以0xFA结尾
 
 uchar g_cache[255]={0};
 unsigned short g_len=0;
 TzhMiniData g_ocCmd;
 uchar g_isGetCmdOk;
 
 //数据接收
 #pragma vector=UART1_R_RXNE_vector
 __interrupt void UART1_RX_RXNE(void)
 {
 //等待终端输入
 //while(!UART1_SR_RXNE);
 
	if(g_len+1>254){ g_len=0; }
 g_cache[g_len]=UART1_DR;
 g_len++;
 
 if(0xFA==UART1_DR)
 {
 int tmp;
 //_nnc:
 //获取指令
 tmp=miniDataGet(g_cache,g_len,&g_ocCmd,&g_isGetCmdOk);
 //处理指令
 if(g_isGetCmdOk)
 {
 func(g_ocCmd.parameter_len,g_ocCmd.parameter);
 }
 //调整缓冲区
 if(tmp>0)
 {
 int n;
 g_len-=tmp;
 for(n=0;n<g_len;n++)
 { g_cache[n]=g_cache[tmp+n]; }
 //         goto _nnc;
 }
 }
 }
 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#ifndef _HXMCU_MINI_DATA_PROTOCOL_H__

#ifdef __cplusplus
extern "C"{
#endif
    
#define uchar   unsigned char
#define ushort   unsigned short
    
#ifndef NULL
#define NULL    0
#endif
    
    
    //-------------------------------------------------
    
    typedef struct _TzhMiniData
    {
        uchar head;  //0xAF开头
        ushort parameter_len;
        uchar* parameter;
        uchar end;   //0xFA结尾
        //
        uchar*frame_head;
        int frame_len;
    }TzhMiniData;
    
    /*
     功能:创建指令
     返回指令的长度
     */
    int miniDataCreate(uchar parm_len,
                       const uchar* parm,
                       uchar* dst_buf);
    
    /*
     功能:获取hx-kong的协议
     从缓冲区截取指令,如果第一个字节不是OC协议的头标识,会截取失败
     返回截取长度,如果指令无效的话,返回的是无用数据的长度
     */
    int miniDataGet(uchar* cache,int cache_len,TzhMiniData* pcmd,uchar* is_get_cmd_success);
    
    
#ifdef __cplusplus
}
#endif
#define _HXMCU_MINI_DATA_PROTOCOL_H__
#endif
