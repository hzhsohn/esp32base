/*

//数据的链表
TzhList g_lstData;
zhListInit(&g_lstData);
zhListNodeAdd(&g_lstData,buf,len,0);

//-------------------------
//处理数据链表
TzhListElementNode *pNode;
pNode=(TzhListElementNode*)zhListFirst(&g_lstData);
if(pNode)
{
	printf("数据链表有数据 count=%d\n",zhListCount(&g_lstData));
	pNode=zhListNodeFreeAndDelete(&g_lstData,pNode);
}

*/

#ifndef _ZH_D_LIST_NODE_MUD_EXP_H__
#define _ZH_D_LIST_NODE_MUD_EXP_H__

#include <stdio.h>
#include "zhlist.h"
#include <mem.h>
#include "string.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_vfs_fat.h"
/* Utils includes. */
#include "esp_log.h"
#include <freertos/semphr.h>

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __cplusplus
#ifndef bool
typedef unsigned char bool;
#define true	1
#define false	0
#endif
#endif

//链表的元素指针
typedef struct _TzhElementNode{
    TzhListNode node;
    void *pElement;
	int nElementSize;
    int nEleType;
}TzhListElementNode;

void zhListNodeInit(TzhListElementNode* eleNode);
TzhListElementNode * zhListNodeFind(TzhList* pList,void *elementNode);
TzhListElementNode * zhListNodeFindWithObject(TzhList* pList,void *pElement);
TzhListElementNode *zhListNodeFindWithIndex(TzhList *pList, int index);

TzhListElementNode * zhListNodeAdd(TzhList *pList,void *pElement,int nElementSize,int elementType);


//释放元素内存,不删除节点
void zhListNodeFree(TzhListElementNode* node);
//删除指定节点
TzhListElementNode* zhListNodeDelete(TzhList* pListNodeList,TzhListElementNode* node);
//删除节点并释放内存
TzhListElementNode* zhListNodeFreeAndDelete(TzhList* pListNodeList,TzhListElementNode* node);

//删除链表,并不会释放其子节点元素的内容
void zhListNodeDeleteAll(TzhList *pList);
//清空所有元素,并不删除节点
void zhListNodeFreeAll(TzhList* pList);
//释放链表内所有内容,并删除链表节点
void zhListNodeFreeAndDeleteAll(TzhList* pList);

#ifdef __cplusplus
}
#endif
#endif

