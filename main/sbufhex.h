#ifndef _SBUF_HEX_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif


int sbufDecode(char *src, char *hex);
//用完后要释放
char* sbufEncode(const unsigned char *p_pBuff, int p_pBuffLen,int* dstLen);

#ifdef  __cplusplus
}
#endif


#define _SBUF_HEX_H_
#endif
