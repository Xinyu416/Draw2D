#include "Scene.h"

void InitGridData() {
	uint32_t wNum = _getGameEngine()->width / GridSize;
	uint32_t hNum = _getGameEngine()->height / GridSize;
	float* vertices = (float*)malloc(wNum * hNum * 6);
	printf("wNum:%d,hNum%d\n", wNum, hNum);
	Geometry geo = CreateGeometry(wNum * hNum);

	for (size_t y = 0; y < 3; y++)
	{
		for (size_t x = 0; x < 3; x++)
		{
			Vect2 p0 = MakeVect2(x * GridSize, (y + 1) * GridSize);
			PrintVect2(p0);
			Vect2 p1 = MakeVect2((x + 1) * GridSize, (y + 1) * GridSize);
			Vect2 p2 = MakeVect2((x + 1) * GridSize, y * GridSize);
			Vect2 p2_2 = MakeVect2((x + 1) * GridSize, y * GridSize);
			Vect2 p3 = MakeVect2(x * GridSize, y * GridSize);
			Vect2 p0_0 = MakeVect2(x * GridSize, (y + 1) * GridSize);
			Quad quad = { .vertices = {p0,p1,p2,p2_2,p3,p0_0,} };
			GeometryAddQuad(&geo, quad);
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 2 };
	Mesh mesh = CreateMesh(3, MakeVect2(60.f, -50.f), 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}