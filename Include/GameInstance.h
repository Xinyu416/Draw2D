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

GameInstance* _getGameIns();

void CreateMeshTest();
