#pragma once
#include "Define.h"

typedef struct {
	float width;
	float height;
	float rot;
	Vect2 pos;
	Vect2 scale;
	Matrix m;
	Matrix tm;
}Camera;

Camera* CreateCamera(float width,float ratio,float rot,Vect2 pos,Vect2 scale);

void GenerateCameraTM(Camera* pCam);

void Camera_Init();