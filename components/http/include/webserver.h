#ifndef __WEB_SERVER1_H__
#define __WEB_SERVER1_H__

#include "http.h"
#include "http_serv.h"
#include "url_parser.h"

//
typedef void PF_WEBSERV_CALLBACK (char *page, char * parameter);
typedef void PF_WEBSERV_DONE_CALLBACK (char *page);
void webserver_init(int web_port,PF_WEBSERV_CALLBACK* webcb,PF_WEBSERV_DONE_CALLBACK* webdone);
void webSendHeader(int len);
void webSendClient(char* data,int len);
void webSendClientNotFoundPage();

void app_webserver_task_accept();
void app_webserver_task_recv();

#endif
