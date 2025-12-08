#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	uint32_t staticIndex;
	uint32_t dynamicIndex;
	uint32_t memSize;
	uint8_t* data;
}MemManager;

MemManager CreateMemManager(uint32_t memSize);

uint8_t* PushToStaticBlock(MemManager memM, uint32_t elementSize);

uint8_t* PushToDynamicBlock(MemManager memM, uint32_t elementSize);

uint8_t _getAliagnOffset(uint32_t index);

bool GetHasPlace(MemManager memM,uint32_t elementSize);

















typedef struct {
	//总数据指针
	uint8_t* data;
	//静态数据 切换场景清除
	uint8_t* staticData;
	uint32_t staticCount;
	uint32_t staticDataSize;

	//动态数据 每帧清除
	uint8_t* dynamicData;
	uint32_t dynamicCount;
	uint32_t dynamicDataSize;

	uint32_t totalSize;

}VMManager;

VMManager CreateManager(uint32_t totalSize, uint32_t staticDataSize, uint32_t dynamicDataSize);

void SetDynamicData(VMManager* vmM, void* dynamicElement);

void SetStaticData(VMManager* vmM, void* staticElement);

uint8_t* GetDynamicData(VMManager* vmM, uint32_t index);

uint8_t* GetStaticData(VMManager* vmM, uint32_t index);

bool GetHasPlace_Dynamic(VMManager* vmM);

bool GetHasPlace_Static(VMManager* vmM);

void ClearDynamicData(VMManager* vmM);

void ClearStaticData(VMManager* vmM);

void PrintManager(VMManager* vmM);
