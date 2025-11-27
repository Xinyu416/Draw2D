#pragma once
#include "GameInstance.h"
#include "GameEngine.h"

//单元格单位值
#define GridSize 25

typedef struct {
	uint32_t wNum;
	uint32_t hNum;
	uint32_t* typeData;
}MapData;

void GenerateGridData();

void SetMapData();

void GenerateBeansData();

void GenerateEnemyData();

void GenerateRoleData();

void GenerateWallData();
