#pragma once

using namespace std;

typedef struct 
{
	char data[16 * 1024 * 1024];
}_DATA, *pDATA;

typedef struct queue
{
	_DATA *pBase;
	int front;    //ָ����е�һ��Ԫ��
	int rear;    //ָ��������һ��Ԫ�ص���һ��Ԫ��
	int maxsize; //ѭ�����е����洢�ռ�
}QUEUE, *PQUEUE;

void CreateQueue(PQUEUE Q, int maxsize);

bool FullQueue(PQUEUE Q);
bool EmptyQueue(PQUEUE Q);

bool Enqueue(PQUEUE Q, _DATA* data);
bool Dequeue(PQUEUE Q, _DATA *data);

bool Clequeue(PQUEUE Q);
void DestroyQueue(PQUEUE Q);