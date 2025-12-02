#pragma once
#include "containers.h"
#include "Mesh.h"
#include "Camera.h"


typedef struct {
	Array meshs;
	Mesh* cMesh;
	Camera* pCam;
	Level* pLevel;
}GameInstance;

void GameIns_Init();

void GameIns_Release();

void GameIns_Tick(float delta);

void GameIns_MouseKeyEvent(VMEVENT eventType,void* key);

GameInstance* _getGameIns();

void GameIns_OpenLevel(uint8_t levelID);

void CreateMeshTest();
