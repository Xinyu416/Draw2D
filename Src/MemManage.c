#include "MemManager.h"

MemManager Mem_Create(uint32_t memSize) {
	uint32_t alignSize = Mem_AlignSize(memSize);
	MemManager memManager = { .memSize = alignSize,.data = (uint8_t*)calloc(sizeof(uint8_t) * alignSize,1) };
	memManager.staticLength = 0;
	memManager.dynamicLength = 0;
	return memManager;
}

void Mem_Release(MemManager* memM) {
	if (memM == NULL || memM->data == NULL)return;
	free(memM->data);
	memM->data = NULL;
	memM->dynamicLength = 0;
	memM->staticLength = 0;
	memM->memSize = 0;

}

void* Mem_AllocateStatic(MemManager* memM, uint32_t elementSize) {
	if (memM == NULL)return NULL;
	uint32_t alignSize = Mem_AlignSize(elementSize);
	if (!Mem_GetHasPlace(memM, alignSize))return NULL;
	void* p = memM->data + memM->staticLength;
	memM->staticLength += alignSize;
	return p;
}

void* Mem_AllocateDynamic(MemManager* memM, uint32_t elementSize) {
	if (memM == NULL)return NULL;
	uint32_t alignSize = Mem_AlignSize(elementSize);
	if (!Mem_GetHasPlace(memM, alignSize))return NULL;
	memM->dynamicLength = memM->dynamicLength + alignSize;
	uint32_t len = memM->memSize - memM->dynamicLength;
	void* p = memM->data + len;
	return p;
}

uint32_t Mem_AlignSize(uint32_t size) {
	uint8_t mod = size % ALIGNBYTE;
	uint8_t fill = (mod == 0) ? 0 : (ALIGNBYTE - mod);
	uint32_t out = size + fill;
	return out;
}

bool Mem_GetHasPlace(MemManager* memM, uint32_t alignSize) {
	uint32_t count = memM->staticLength + memM->dynamicLength + alignSize;
	return count < memM->memSize;
}

uint32_t Mem_GetRemainSize(MemManager* memM) {
	uint32_t remain = memM->memSize - memM->staticLength - memM->dynamicLength;
	return remain;
}

void Mem_ClearDynamic(MemManager* memM) {
	memM->dynamicLength = 0;
}

void Mem_ClearStatic(MemManager* memM) {
	memM->staticLength = 0;
}

void PrintMemManager(MemManager* memM) {
	printf("memM:size:%d,staticIndex:%d,dynamicIndex:%d,remain:%d\n", memM->memSize, memM->staticLength, memM->dynamicLength, Mem_GetRemainSize(memM));
}

void PrintMemData(MemManager* memM) {
	for (size_t i = 0; i < memM->memSize; i++)
	{
		printf("%x ", memM->data[i]);
	}
}