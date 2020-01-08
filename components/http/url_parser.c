/*
 * url_parser.c
 *
 * This is a wrapper around http_parser_url to simplify usage.
 *
 *  Created on: 29.03.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"

#include "http_parser.h"
#include "url_parser.h"

#define TAG "url_parser"


static char* url_get_scheme(struct http_parser_url *url, char *uri)
{
    return strndup(&uri[url->field_data[UF_SCHEMA].off], url->field_data[UF_SCHEMA].len);
}

static char* url_get_host(struct http_parser_url *url, char *uri)
{
    return strndup(&uri[url->field_data[UF_HOST].off], url->field_data[UF_HOST].len);
}

static uint16_t url_get_port(struct http_parser_url *url)
{
    uint16_t port;

    if (url->field_set & (1 << UF_PORT)) {
        port = url->port;
    } else {
        // assume: 4 = http, 5 = https
        port = (url->field_data[UF_SCHEMA].len == 5) ? 443 : 80;
    }

    return port;
}

static char* url_get_authority(struct http_parser_url *url, char *uri)
{
    char *authority;

    /* MAX 5 digits (max 65535) + 1 ':' + 1 NULL (because of snprintf) */
    size_t extra = 7;

    uint16_t authoritylen = url->field_data[UF_HOST].len;
    authority = calloc(authoritylen + extra, sizeof(char));
    memcpy(authority,
            &uri[url->field_data[UF_HOST].off],
            url->field_data[UF_HOST].len);
    /* maybe add port */
    if (url->field_set & (1 << UF_PORT)) {
        authoritylen += (size_t) snprintf(authority + authoritylen,
                extra, ":%u", url->port);
    }
    authority[authoritylen] = '\0';

    return authority;
}

static char* url_get_path(struct http_parser_url *url, char *uri)
{
    /* path */
    char *path;

    /* If we don't have path in URI, we use "/" as path. */
    uint16_t pathlen = 1;
    if (url->field_set & (1 << UF_PATH)) {
        pathlen = url->field_data[UF_PATH].len;
    }
    if (url->field_set & (1 << UF_QUERY)) {
        /* +1 for '?' character */
        pathlen += (size_t) (url->field_data[UF_QUERY].len + 1);
    }

    /* +1 for \0 */
    path = malloc(pathlen + 1);
    if (url->field_set & (1 << UF_PATH)) {
        memcpy(path, &uri[url->field_data[UF_PATH].off],
                url->field_data[UF_PATH].len);
    } else {
        path[0] = '/';
    }

    if (url->field_set & (1 << UF_QUERY)) {
        path[pathlen - url->field_data[UF_QUERY].len - 1] = '?';
        memcpy(
                path + pathlen - url->field_data[UF_QUERY].len,
                &uri[url->field_data[UF_QUERY].off], url->field_data[UF_QUERY].len);
    }
    path[pathlen] = '\0';

    return path;
}


url_t *url_parse(char *uri)
{
    struct http_parser_url *url_parser = calloc(1, sizeof(struct http_parser_url));
    if(url_parser == NULL) {
        ESP_LOGE(TAG, "could not allocate http_parser_url");
        return NULL;
    }

    int ret = http_parser_parse_url(uri, strlen(uri), 0, url_parser);
    if (ret != 0) {
        ESP_LOGE(TAG, "Could not parse URI %s", uri);
        return NULL;
    }

    url_t *url = calloc(1, sizeof(url_t));
    if(url_parser == NULL) {
        ESP_LOGE(TAG, "could not allocate url_t");
        return NULL;
    }

    url->scheme = url_get_scheme(url_parser, uri);
    url->host = url_get_host(url_parser, uri);
    url->port = url_get_port(url_parser);
    url->authority = url_get_authority(url_parser, uri);
    url->path = url_get_path(url_parser, uri);

    free(url_parser);

    return url;
}

void url_free(url_t *url)
{
    if(url->scheme != NULL) free(url->scheme);
    if(url->host != NULL) free(url->host);
    if(url->authority != NULL) free(url->authority);
    if(url->path != NULL) free(url->path);

    free(url);
}




//返回值
//0=替换失败
//1=替换成功
//replace_str(strSrc,"我被替换了","靓仔",strDst);
//查找并替换字符串
int replace_str(const char *pInput,const char *pSrc,const char *pDst,char *pOutput)
{
	 const char   *pi;
	 char *p;
	 int nSrcLen, nDstLen, nLen;
	 int ret;

	 ret=0;
	 // 指向输入字符串的游动指针.
	 pi = pInput;

	 // 计算被替换串和替换串的长度.
	 nSrcLen = (int)strlen(pSrc);
	 nDstLen = (int)strlen(pDst);
	 // 查找pi指向字符串中第一次出现替换串的位置,并返回指针(找不到则返回null).
	 p = strstr(pi, pSrc);
	 if(p)
	 {
				char *po; // 指向输出字符串的游动指针.
				po=(char*)malloc(strlen(pInput)+strlen(pDst)+1);

				// 计算被替换串前边字符串的长度.
				nLen = (int)(p - pi);
				// 复制到输出字符串.
				memcpy(po, pi, nLen);
				memcpy(po+nLen, pDst, nDstLen);
				strcpy(po+nLen+nDstLen, pi+nLen+nSrcLen);

				// 复制剩余字符串.
				strcpy(pOutput, po);
				free(po);
				po=NULL;
				ret=1;
	 }
	 else
	 {
				// 没有找到则原样复制.
				strcpy(pOutput, pi);
	 }
	return ret;
}
