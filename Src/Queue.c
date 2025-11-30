#include "Queue.h"
#include "Mesh.h"

Queue CreateQueue(uint32_t length, uint32_t elementSize) {
	Queue Q = { .elementSize = elementSize,.length = length,.data = (uint8_t*)malloc(elementSize * length),.head = 0,.tail = 0,.count = 0 };
	return Q;
}

void ReleaseQueue(Queue* Q) {
	Q->length = 0;
	Q->elementSize = 0;
	free(Q->data);
	Q->data = NULL;
}

//head = tail 空队列
//head = tail = 1 (0) 刚开始
//head = tail+1 满队列
void EnQueue(Queue* Q, void* element) {

	if (Q == NULL || element == NULL || Q->data == NULL) {
		return ;
	}
	uint32_t next_tail = (Q->tail + 1) % Q->length;

	if (/*Q->head == next_tail */ Q->count == Q->length)
	{
		//队列是满的 无法插入，元素上溢
		printf("EnQueue 满队列 无法入队\n");
		return;
	}
	memcpy(Q->data + (Q->tail * Q->elementSize), element, Q->elementSize);
	Q->tail = next_tail;
	Q->count++;
	PrintQueue(Q);
}

uint8_t* DeQueue(Queue* Q) {
	if (Q == NULL || Q->data == NULL) {
		return;
	}
	printf("DeQueue\n");
	if (/*Q->head == Q->tail*/  Q->count == 0)
	{
		printf("DeQueue 空队列 无法出队\n");
		return NULL;
	}
	uint8_t* item = Q->data + (Q->head * Q->elementSize);
	Q->head = (Q->head + 1) % Q->length;
	Q->count--;
	PrintQueue(Q);
	return item;
}

void PrintQueue(Queue* Q) {
	printf("Queue len:%d,elementSize:%d,head:%d,tail:%d,count:%d\n", Q->length, Q->elementSize, Q->head, Q->tail,Q->count);
}

void QueueTest() {
	Quad quad1 = {
		.vertices = {-110, -10, 110, -10, 110, -100,110, -105, -110, -110,-110, -15},
		.uvs = {
			0.0f, 0.f, 1.0f, 0.0f, 1.f, 1.f ,
			1.f, 1.f, 0.0f, 1.0f, 0.f, 0.f},
		.color = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255, 0,255,255,255, 255,0,255,255}
	};

	Quad quad2 = { .vertices = {-50, 50,-20, 50,-20, 20,-20, 20,-50, 20,-50, 50},
		.uvs = {
			0.0f, 0.f, 1.0f, 0.0f, 1.f, 1.f ,
			1.f, 1.f, 0.0f, 1.0f, 0.f, 0.f},
				.color = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255,0,255,255,255,255,255,255,255}
	};

	Quad quad3 = { .vertices = {30, 60,60, 60,60, 30,60, 30, 30, 30,30, 60},
				.color = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255,0,255,255,255,255,255,255,255}
	};

	Quad quad4 = { .vertices = { -4, 6, 6, 6, 6, -4, 6, -6, -6, -6,-6, 4},
				.color = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255, 0,255,255,255, 255,255,255,255}
	};

	Quad quad5 = { .vertices = { -4, 6, 6, 6, 6, -4, 6, -6, -6, -6,-6, 4},
				.color = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255, 0,255,255,255, 255,255,255,255} };

	Queue Q = CreateQueue(4, sizeof(Quad));
	//Quad* deq = (Quad*)DeQueue(&Q);
	EnQueue(&Q, &quad1);
	EnQueue(&Q, &quad2);
	EnQueue(&Q, &quad3);
	EnQueue(&Q, &quad4);
	EnQueue(&Q, &quad5);

	Quad* deq1 = (Quad*)DeQueue(&Q);
	Quad* deq2 = (Quad*)DeQueue(&Q);
	Quad* deq3 = (Quad*)DeQueue(&Q);
	Quad* deq4 = (Quad*)DeQueue(&Q);

	Quad* deq5 = (Quad*)DeQueue(&Q);
	EnQueue(&Q, &quad5);
	EnQueue(&Q, &quad4);
	EnQueue(&Q, &quad1);
	EnQueue(&Q, &quad2);
	EnQueue(&Q, &quad3);
	deq5 = (Quad*)DeQueue(&Q);
	return;

	EnQueue(&Q, &quad1);
	PrintQueue(&Q);
	EnQueue(&Q, &quad2);
	PrintQueue(&Q);
	EnQueue(&Q, &quad3);
	PrintQueue(&Q);
	deq1 = (Quad*)DeQueue(&Q);
	PrintQueue(&Q);

	/*if (deq1)
	{
		printf("quad1:vertices :%f\n", deq1->vertices[0].x);

	}
	if (deq2)
	{
		printf("quad2:vertices :%f\n", deq2->vertices[0].x);

	}
	if (deq3)
	{
		printf("quad3:vertices :%f\n", deq3->vertices[0].x);

	}
	if (deq4)
	{
		printf("quad4:vertices :%f\n", deq4->vertices[0].x);

	}*/
}