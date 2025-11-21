#pragma once
#include "containers.h"

typedef struct {
	Array meshs;
}GameInstance;

void GameIns_Init();

void GameIns_Release();

void GameIns_Tick(float delta);

GameInstance* _getGameIns();

void CreateMeshTest();

//void RenderMesh(uint8_t* buffer, uint32_t height, uint32_t width, uint8_t bytepp, Mesh mesh);