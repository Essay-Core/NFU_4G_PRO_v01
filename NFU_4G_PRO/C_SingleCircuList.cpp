#include "stdafx.h"
#include "C_SingleCircuList.h"

_DATA *g_pBase;

//�������У������ڴ�ռ�
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
		exit(-1);        //�˳�����
	}

	Q->front = 0;         //��ʼ������
	Q->rear = 0;
	Q->maxsize = maxsize;

}

//���
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

//������
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

//��ն���
bool Clequeue(PQUEUE Q)
{
	if (EmptyQueue(Q))
	{
		return false;//ԭ�����ǿյ�
	}
	else
	{
		Q->front = 0;        
		Q->rear = 0;
		return true;
	}
}

//���ٶ��в���
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
	if (Q->front == (Q->rear + 1) % Q->maxsize)    //�ж�ѭ�������Ƿ�������һ��Ԥ���ռ䲻��
		return true;
	else
		return false;
}

bool EmptyQueue(PQUEUE Q)
{
	if (Q->front == Q->rear)    //�ж��Ƿ�Ϊ��
		return true;
	else
		return false;
}