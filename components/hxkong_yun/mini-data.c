#include "mini-data.h"

/*
  创建一条新的指令
    返回指令的长度buf
*/
int miniDataCreate(uchar parm_len,
                  const uchar* parm,
                  uchar* dst_buf)
{
    int n=0;
    int i=0;
    
    //错误长度
    if(0==parm_len)
      return 0;
 
    dst_buf[n]=0xAF;
    n++;
       
    if(parm_len<0xFF)
    {
      dst_buf[n]=parm_len;
      n++;
    }
    else
    {
	    dst_buf[n]=0xFF;
      n++;
      dst_buf[n]=parm_len&0xFF;
      n++;
      dst_buf[n]=(parm_len&0xFF00)>>8;
      n++;
    }
    
    for(i=0;i<parm_len;i++)
    {
        dst_buf[n]=parm[i];
        n++;
    }
    
    dst_buf[n]=0xFA;
    n+=1;
    
    return n;
}

int miniDataGet(uchar* cache,int cache_len,TzhMiniData* pcmd,uchar* is_get_cmd_success)
{
    int n=0;
    int tmp_frame_base=0;
    *is_get_cmd_success=0;//每帧的基本长度,帧头,字节长度,帧尾
    
    //最少长度为4
    //3=0xAF+参数长度+0xFA
    if(cache_len<3)
        return 0;
    
    for(n=0;n<cache_len;n++)
    {
        //判断是否协议头
        if(0xAF==cache[n])
        {
            pcmd->frame_head=&cache[n];
            break;
        }
    }
    //去掉头
    if(n>0)
        return n;
    
    pcmd->head=0xAF;

    //跳到去长度
    n++;
    if(0xFF==cache[n])
    {
        pcmd->parameter_len=cache[n+1]|cache[n+2]<<8;
        n+=3;
        tmp_frame_base=5;
    }
    else
    {
        pcmd->parameter_len=cache[n];
        n++;
        tmp_frame_base=3;
    }
    //长度为0,错误帧
    if(0==pcmd->parameter_len)
      return 1;
    
    //判断数据长度
    if(cache_len<pcmd->parameter_len+tmp_frame_base)
        return 0;

    //参数
    if(pcmd->parameter_len>0)
    {
        pcmd->parameter=&cache[n];
        n+=pcmd->parameter_len;
    }
    else
    {
        pcmd->parameter=0;
    }
    //结尾
    if(cache[n]!=0xFA)
        return 1;
    pcmd->end=0xFA;
    n+=1;

    pcmd->frame_len=(&cache[n]) - pcmd->frame_head;
    //数据校验成功
    *is_get_cmd_success=1;
    return n;
}
