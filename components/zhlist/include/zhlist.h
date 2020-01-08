#ifndef _ZH_D_LIST_NODE_MUD_H__
#define _ZH_D_LIST_NODE_MUD_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _TzhListNode
{
	struct _TzhListNode *next;
	struct _TzhListNode *prev;
}TzhListNode;

typedef struct _TzhList
{
	TzhListNode node;
	int count;
}TzhList;

TzhListNode *zhListFirst(TzhList *pList);
TzhListNode *zhListLast(TzhList *pList);
TzhListNode *zhListNext(TzhListNode *pNode);
TzhListNode *zhListPrev(TzhListNode *pNode);
int zhListCount(TzhList *pList);
int zhListFind(TzhList *pList, TzhListNode *pNode);
void zhListAdd(TzhList *pList, TzhListNode *pNode);
void zhListDelete(TzhList *pList, TzhListNode *pNode);
void zhListFree(TzhList *pList);
void zhListInit(TzhList *pList);
void zhListInsert(TzhList *pList, TzhListNode *pPrev, TzhListNode *pNode);

#ifdef __cplusplus
}
#endif
#endif
