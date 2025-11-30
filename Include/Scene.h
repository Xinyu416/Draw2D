#pragma once
#include "GameInstance.h"
#include "GameEngine.h"

//单元格单位值
#define GridSize 25.f


typedef enum {
	BG = 0,		//背景
	BEAN = 1,	//豆子
	ITEM = 2,	//道具
	ROLE = 3	//角色
}MESHTYPE;


typedef struct {
	uint32_t wNum;
	uint32_t hNum;
	uint32_t* typeData;
}MapData;

void Scene_Init();

void CreateMeshData(MESHTYPE meshType, uint32_t texWidth, uint32_t texHeight,uint8_t textureId);

void SetMapData();

Vect2* getVeticesbyCoord(uint32_t x, uint32_t y, Vect2 offset, float gridSize);

int getMapDataByPos(Vect2 pos);

void ChangeBeanColor(uint32_t mapIndex);

void GenerateRoleData();

