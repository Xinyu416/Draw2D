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

GameInstance* _getGameIns();

void GameIns_Init();

void GameIns_Release();

void GameIns_Tick(float delta);

void GameIns_MouseKeyEvent(VMEVENT eventType,void* key);

void GameIns_OpenLevel(uint8_t levelID);

void GameIns_CreateMesh();

void GameIns_ReleaseMesh();

void GameIns_CreateCamera(float width, float ratio, float rot, Vect2 pos, Vect2 scale);

void GameIns_ReleaseCamera();

void CreateMeshTest();
