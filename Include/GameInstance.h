#pragma once
//#include "GameEngine.h"

//typedef struct {
//
//}GameInstance;

void Game_Init();

void Game_Release();

void Game_Tick(float delta);

void RenderMesh(uint8_t* buffer, uint32_t height, uint32_t width, uint8_t bytepp, Mesh mesh);