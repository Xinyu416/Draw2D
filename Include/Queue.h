#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
	uint32_t length;
	uint32_t head;
	uint32_t tail;
	uint8_t* data;
	uint32_t elementSize;
	uint32_t count;
}Queue;

Queue CreateQueue(uint32_t length, uint32_t elementSize);

void ReleaseQueue(Queue* Q);

void EnQueue(Queue* Q, void* element);

uint8_t* DeQueue(Queue* Q);

void PrintQueue(Queue* Q);

void QueueTest();