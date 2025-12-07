#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
	//静态数据 切换场景清除
	uint8_t* staticData;
	uint32_t staticIndex;
	uint32_t staticDataSize;

	//动态数据 每帧清除
	uint8_t* dynamicData;
	uint32_t dynamicIndex;
	uint32_t dynamicDataSize;

	uint32_t totalSize;

}VMManager;

void CreateManager();

