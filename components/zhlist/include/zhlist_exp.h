/*

//���ݵ�����
TzhList g_lstData;
zhListInit(&g_lstData);
zhListNodeAdd(&g_lstData,buf,len,0);

//-------------------------
//������������
TzhListElementNode *pNode;
pNode=(TzhListElementNode*)zhListFirst(&g_lstData);
if(pNode)
{
	printf("�������������� count=%d\n",zhListCount(&g_lstData));
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

//�����Ԫ��ָ��
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


//�ͷ�Ԫ���ڴ�,��ɾ���ڵ�
void zhListNodeFree(TzhListElementNode* node);
//ɾ��ָ���ڵ�
TzhListElementNode* zhListNodeDelete(TzhList* pListNodeList,TzhListElementNode* node);
//ɾ���ڵ㲢�ͷ��ڴ�
TzhListElementNode* zhListNodeFreeAndDelete(TzhList* pListNodeList,TzhListElementNode* node);

//ɾ������,�������ͷ����ӽڵ�Ԫ�ص�����
void zhListNodeDeleteAll(TzhList *pList);
//�������Ԫ��,����ɾ���ڵ�
void zhListNodeFreeAll(TzhList* pList);
//�ͷ���������������,��ɾ������ڵ�
void zhListNodeFreeAndDeleteAll(TzhList* pList);

#ifdef __cplusplus
}
#endif
#endif

