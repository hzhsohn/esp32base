/* UDP MultiCast Send/Receive Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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

#include "uart1.h"

#include "udp_multicast.h"

void app_mcast_task();

#define UDP_PORT								2288
#define MULTICAST_LOOPBACK						1
#define MULTICAST_TTL							255
#define MULTICAST_IPV4_ADDR						"228.0.0.8"

static const char *TAG = "udpmcast";
static const char *V4TAG = "mcast-ipv4";

int g_udp_sock;

char g_lastUDP_ip[20]={0};
int g_lastUDP_port=0;

PF_UDP_READ* g_pfUdpRead=NULL;

/* Add a socket, either IPV4-only or IPV6 dual mode, to the IPV4
   multicast group */
static int socket_add_ipv4_multicast_group(int sock, bool assign_source_if)
{
    struct ip_mreq imreq = { 0 };
    struct in_addr iaddr = { 0 };
    int err = 0;
    tcpip_adapter_ip_info_t ip_info = { 0 };
    err = tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
    if (err != ESP_OK) {
        ESP_LOGE(V4TAG, "Failed to get IP address info. Error 0x%x", err);
        goto err;
    }
    inet_addr_from_ipaddr(&iaddr, &ip_info.ip);

    // Configure multicast address to listen to
    err = inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr);
    if (err != 1) {
        ESP_LOGE(V4TAG, "Configured IPV4 multicast address '%s' is invalid.", MULTICAST_IPV4_ADDR);
        goto err;
    }
    ESP_LOGI(TAG, "Configured IPV4 Multicast address %s", inet_ntoa(imreq.imr_multiaddr.s_addr));
    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr))) {
        ESP_LOGW(V4TAG, "Configured IPV4 multicast address '%s' is not a valid multicast address. This will probably not work.", MULTICAST_IPV4_ADDR);
    }

    if (assign_source_if) {
        // Assign the IPv4 multicast source interface, via its IP
        // (only necessary if this socket is IPV4 only)
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
                         sizeof(struct in_addr));
        if (err < 0) {
            ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_IF. Error %d", errno);
            goto err;
        }
    }

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         &imreq, sizeof(struct ip_mreq));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }

 err:
    return err;
}

static int create_multicast_ipv4_socket()
{
    struct sockaddr_in saddr = { 0 };
    int sock = -1;
    int err = 0;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(V4TAG, "Failed to create socket. Error %d", errno);
        return -1;
    }

    // Bind the socket to any address
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(UDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to bind socket. Error %d", errno);
        goto err;
    }

    // Assign multicast TTL (set separately from normal interface TTL)
    uint8_t ttl = MULTICAST_TTL;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_TTL. Error %d", errno);
        goto err;
    }

    // select whether multicast traffic should be received by this device, too
    // (if setsockopt() is not called, the default is no)
    uint8_t loopback_val = MULTICAST_LOOPBACK;
    err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
                     &loopback_val, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_LOOP. Error %d", errno);
        goto err;
    }

    // this is also a listening socket, so add it to the multicast
    // group for listening...
    err = socket_add_ipv4_multicast_group(sock, true);
    if (err < 0) {
        goto err;
    }

    // All set, socket is configured for sending and receiving
    return sock;

err:
    close(sock);
    return -1;
}

void app_mcast_task()
{
	char *udpRecvbuf;
	udpRecvbuf=(char *)malloc(1400);
	memset(udpRecvbuf,0,1400);
    while (true) {
        g_udp_sock = create_multicast_ipv4_socket();
        if (g_udp_sock < 0) {
            ESP_LOGE(TAG, "Failed to create IPv4 multicast socket");
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }
        // set destination multicast addresses for sending from these sockets
        struct sockaddr_in sdestv4 = {
            .sin_family = AF_INET,
            .sin_port = htons(UDP_PORT),
        };
        // We know this inet_aton will pass because we did it above already
        inet_aton(MULTICAST_IPV4_ADDR, &sdestv4.sin_addr.s_addr);
		//
        int err = 1;
        while (err > 0) {
					//ESP_LOGE(TAG, "udp thread... \r\n");
					//
                    struct sockaddr_in6 raddr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(raddr);
                    int len = recvfrom(g_udp_sock, udpRecvbuf,1400, 0,
                                       (struct sockaddr *)&raddr, &socklen);
                    
				     if (len < 0) {
                        ESP_LOGE(TAG, "multicast recvfrom failed: errno %d\r\n", errno);
                        err = -1;
                        break;
                    }
                    // Get the sender's address as a string
                    if (raddr.sin6_family == AF_INET) {
                        inet_ntoa_r(((struct sockaddr_in *)&raddr)->sin_addr.s_addr,
                                    g_lastUDP_ip, sizeof(g_lastUDP_ip)-1);
												
						g_lastUDP_port=ntohs(((struct sockaddr_in *)&raddr)->sin_port);
                    }
					if(len>0)
					{
						//ESP_LOGI(TAG, "udp recv %s:%d->%d bytes", g_lastUDP_ip,g_lastUDP_port, len);
						g_pfUdpRead(udpRecvbuf,len);
					}
        }

        ESP_LOGE(TAG, "Shutting down socket and restarting...\r\n");
        close(g_udp_sock);
    }
	free(udpRecvbuf);
	udpRecvbuf=NULL;
}

void sendUDP(const char*buf,int len,char*ipv4,int port)
{
				struct addrinfo hints = {
                    .ai_flags = AI_PASSIVE,
                    .ai_socktype = SOCK_DGRAM,
                };
                struct addrinfo *res;

                hints.ai_family = AF_INET; // For an IPv4 socket
                int err = getaddrinfo(ipv4,
                                      NULL,
                                      &hints,
                                      &res);
                if (err < 0) {
                    ESP_LOGE(TAG, "getaddrinfo() failed for IPV4 destination address. error: %d", err);
                    return;
                }
                ((struct sockaddr_in *)res->ai_addr)->sin_port = htons(port);
                //ESP_LOGI(TAG, "Sending to IPV4 multicast address %s:%d",  ipv4,port);
                err = sendto(g_udp_sock,(uint8_t*) buf, len, 0, res->ai_addr, res->ai_addrlen);
                if (err < 0) {
                    ESP_LOGE(TAG, "IPV4 sendto failed. errno: %d", errno);
                }
}

void sendUDPLastIP(const char*buf,int len)
{
			if(g_lastUDP_port!=0 && g_lastUDP_ip[0]!=0)
			{
				sendUDP(buf,len,g_lastUDP_ip,g_lastUDP_port);
			}
}

void getLastRecvIP(char*ip,int*port)
{
	strcpy(ip,g_lastUDP_ip);
	*port=g_lastUDP_port;
}

void app_mcast(PF_UDP_READ* pfUdp)
{
	g_pfUdpRead=pfUdp;
	xTaskCreate(app_mcast_task, "mudp_task", 4096,NULL, 5, NULL);
}
