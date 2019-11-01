#include "stdafx.h"
#include "C_SingleCircuList.h"

_DATA *g_pBase;

//创建队列，分配内存空间
void CreateQueue(PQUEUE Q, int maxsize)
{
	Q->pBase = (_DATA*)malloc(sizeof(_DATA)*maxsize);
	if (!Q->pBase)
	{
		TRACE("malloc error\n");
		return;
	}

	g_pBase = Q->pBase;
	memset(Q->pBase, 0, sizeof(_DATA)*maxsize);

	if (NULL == Q->pBase)
	{
		TRACE("Memory allocation failure\n");
		exit(-1);        //退出程序
	}

	Q->front = 0;         //初始化参数
	Q->rear = 0;
	Q->maxsize = maxsize;

}

//入队
bool Enqueue(PQUEUE Q, _DATA* data)
{
	if (!data)
	{
		TRACE("data error!!!\n");
		return false;
	}
	else if (FullQueue(Q))
	{
		TRACE("FullQueue!!!\n");
		return false;
	}	
	else
	{
		memset((char*)&Q->pBase[Q->rear], 0, sizeof(_DATA));
		memcpy((char*)&Q->pBase[Q->rear], (char*)data, sizeof(_DATA));
		Q->rear = (Q->rear + 1) % Q->maxsize;
		
		return true;
	}
}

//出队列
bool Dequeue(PQUEUE Q, _DATA *data)
{
	if (EmptyQueue(Q))
	{
		return false;
	}
	else
	{
		memcpy(data, (char*)&Q->pBase[Q->front], sizeof(_DATA));
		Q->front = (Q->front + 1) % Q->maxsize;

		return true;
	}
}

//清空队列
bool Clequeue(PQUEUE Q)
{
	if (EmptyQueue(Q))
	{
		return false;//原本就是空的
	}
	else
	{
		Q->front = 0;        
		Q->rear = 0;
		return true;
	}
}

//销毁队列操作
void DestroyQueue(PQUEUE Q)
{
	if (g_pBase)
	{
		free(g_pBase);
	}
	 
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool FullQueue(PQUEUE Q)
{
	if (Q->front == (Q->rear + 1) % Q->maxsize)    //判断循环链表是否满，留一个预留空间不用
		return true;
	else
		return false;
}

bool EmptyQueue(PQUEUE Q)
{
	if (Q->front == Q->rear)    //判断是否为空
		return true;
	else
		return false;
}