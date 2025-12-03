#include "Camera.h"

Camera* CreateCamera(float width,float ratio, float rot, Vect2 pos, Vect2 scale) {
	Camera* cam = (Camera*)malloc(sizeof(Camera));
	cam->width = width;
	cam->height = width/ratio;
	cam->scale = scale;
	cam->rot = rot;
	cam->pos = pos;
	GenerateCameraTM(cam);
	return cam;
}

//生成新的矩阵
void GenerateCameraTM(Camera * pCam) {
	Matrix ms = MakeScaMatrix(pCam->scale.x, pCam->scale.y);
	Matrix mr = MakeRotMatrix(Deg2Rad(pCam->rot));
	Matrix mt = MakeTranslataMatrix(pCam->pos.x, pCam->pos.y);

	Matrix msr = CreateStandardMatrix();
	Matrix msrt = CreateStandardMatrix();

	Multi2Matrix(mr.m, ms.m, msr.m);
	Multi2Matrix(mt.m, msr.m, msrt.m);

	pCam->m = msrt;
	pCam->tm = InversMatrix(pCam->m.m);
}

void Camera_Init() {
	//Camera* cam = CreateCamera(600, 600, 50, MakeVect2(100, 100), MakeVect2(1, 1));
}
