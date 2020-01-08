#ifndef  __UDP_MULTCAST_H__
#define __UDP_MULTCAST_H__

typedef void PF_UDP_READ (const char *buf, int len);

void app_mcast(PF_UDP_READ* pfUdp);

void sendUDP(const char*buf,int len,char*ipv4,int port);
void sendUDPLastIP(const char*buf,int len);

void getLastRecvIP(char*ip,int*port);

#endif 
