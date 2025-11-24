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

void Camera_Init();