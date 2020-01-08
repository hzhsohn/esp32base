#ifndef __MD5_H__
#define __MD5_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

/////////////////////////////////////////////////////////////////////////////////
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

/* MD5 context. */
typedef struct {
  UINT4 state[4];          /* state (ABCD) */
  UINT4 count[2];          /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];/* input buffer */
} MD5_CTX;

void WsockMD5Init(MD5_CTX *);
void WsockMD5Update(MD5_CTX *, unsigned char *, unsigned int);
void WsockMD5Final(unsigned char [16], MD5_CTX *);


/* extended function */
void WsockMDData(char *data, int len,unsigned char *digest);


#ifdef __cplusplus
}
#endif

#endif