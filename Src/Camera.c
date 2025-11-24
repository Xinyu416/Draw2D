#include "Camera.h"

void Camera_Init() {
	Camera cam = { .width = 600, .height = 600,.pos = 100.f,100.f,.rot = 50.f,.scale = {1,1} };
	//缩放旋转结果矩阵
	Matrix srm = CreateStandardMatrix();
	//缩放旋转位移结果矩阵
	Matrix srtm = CreateStandardMatrix();
	//缩放
	Matrix ms = MakeScaMatrix(cam.scale.x, cam.scale.y);
	//旋转
	Matrix mr = MakeRotMatrix(Deg2Rad(cam.rot));
	//位移
	Matrix mt = MakeTranslataMatrix(cam.pos.x, cam.pos.y);
	//一起算
	Multi2Matrix(mr.m, ms.m, srm.m);
	Multi2Matrix(mt.m, srm.m, srtm.m);
	cam.m = srtm;

	cam.tm = InversMatrix(cam.m.m);
	Matrix stadard = CreateStandardMatrix();
	Multi2Matrix(cam.m.m, cam.tm.m, stadard.m);

	printf("Camera矩阵:\n");
	PrintMatrix(cam.m.m);
	printf("逆矩阵:\n");
	PrintMatrix(cam.tm.m);
	printf("标准矩阵:\n");
	PrintMatrix(stadard.m);
}
