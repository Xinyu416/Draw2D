#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define ALIGNBYTE 4

typedef struct {
	uint8_t* data;
	uint32_t staticLength;
	uint32_t dynamicLength;
	uint32_t memSize;
	uint8_t block[4];//64位操作系统需要补
}MemManager;

MemManager Mem_Create(uint32_t memSize);

void Mem_Release(MemManager* memM);

void* Mem_AllocateStatic(MemManager* memM, uint32_t elementSize);

void* Mem_AllocateDynamic(MemManager* memM, uint32_t elementSize);

uint32_t Mem_AlignSize(uint32_t size);

bool Mem_GetHasPlace(MemManager* memM,uint32_t elementSize);

uint32_t Mem_GetRemainSize(MemManager* memM);

void Mem_ClearDynamic(MemManager* memM);

void Mem_ClearStatic(MemManager* memM);

void PrintMemManager(MemManager* memM);

void PrintMemData(MemManager* memM);
