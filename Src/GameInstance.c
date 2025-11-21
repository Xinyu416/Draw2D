#include "GameInstance.h"
#include "Mesh.h"

static GameInstance* _gameIns = NULL;
void GameIns_Init() {
	GameInstance* gameIns = _getGameIns();
	gameIns->meshs = ArrayCreate(sizeof(Mesh));
	CreateMeshTest();
}

void CreateMeshTest() {
	Quad quad1 = { 0 };
	quad1.vertices[0] = MakeVect2(-50, -10);
	quad1.vertices[1] = MakeVect2(50, -10);
	quad1.vertices[2] = MakeVect2(50, -50);
	quad1.vertices[3] = MakeVect2(-50, -50);
	quad1.uvs[0] = MakeVect2(0, 0);
	quad1.uvs[1] = MakeVect2(0.2, 0.3);
	quad1.uvs[2] = MakeVect2(0.8, 0.5);
	quad1.uvs[3] = MakeVect2(1, 1);
	quad1.color[0] = MakeColor4(100, 100, 100, 100);
	quad1.color[1] = MakeColor4(200, 200, 200, 200);
	quad1.color[2] = MakeColor4(150, 150, 150, 150);
	quad1.color[3] = MakeColor4(10, 10, 10, 10);

	Quad quad2 = { 0 };
	quad2.vertices[0] = MakeVect2(-50, 50);
	quad2.vertices[1] = MakeVect2(-20, 50);
	quad2.vertices[2] = MakeVect2(-20, 20);
	quad2.vertices[3] = MakeVect2(-50, 20);
	quad2.color[0] = MakeColor4(100, 100, 100, 100);
	quad2.color[1] = MakeColor4(200, 200, 200, 200);
	quad2.color[2] = MakeColor4(150, 150, 150, 150);
	quad2.color[3] = MakeColor4(10, 10, 10, 10);

	Geometry geo = CreateGeometry(4);
	GeometryAddQuad(&geo, quad1);
	GeometryAddQuad(&geo, quad2);

	printf("num:%d\n", geo.numOfQuad);

	//test point in or out
	Quad quad3 = { 0 };
	quad3.vertices[0] = MakeVect2(30, 60);
	quad3.vertices[1] = MakeVect2(60, 60);
	quad3.vertices[2] = MakeVect2(60, 30);
	quad3.vertices[3] = MakeVect2(30, 30);
	quad3.color[0] = MakeColor4(100, 100, 100, 100);
	quad3.color[1] = MakeColor4(200, 200, 200, 200);
	quad3.color[2] = MakeColor4(150, 150, 150, 150);
	quad3.color[3] = MakeColor4(10, 10, 10, 10);

	GeometryAddQuad(&geo, quad3);
	bool isIn = IsPointInQuad(MakeVect2(2.5f, 2), quad3);
	printf("isIn:%d\n", isIn);

	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 1 };
	Mesh mesh = CreateMesh(1, MakeVect2(400.f, 300.f), 0, MakeVect2(1.f, 1.f), geo, tm, mat);

	ArrayPush(&_getGameIns()->meshs, &mesh);

	PrintMesh(&mesh);
	PrintArray(&_getGameIns()->meshs);

	/*Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs,0);
	PrintMesh(pmesh);*/

	//缩放旋转结果矩阵
	Matrix srm = CreateStandardMatrix();
	//缩放旋转位移结果矩阵
	Matrix srtm = CreateStandardMatrix();
	//缩放
	Matrix ms = MakeScaMatrix(mesh.scale.x, mesh.scale.y);
	printf("mesh.scale.x:%.2f,mesh.scale.y:%.2f \n",mesh.scale.x,mesh.scale.y);
	printf("\nmesh ms martix:------------\n");
	PrintMatrix(ms.m);
	//旋转
	Matrix mr = MakeRotMatrix(Deg2Rad(mesh.rot));
	printf("mesh mrs martix:------------\n");
	PrintMatrix(mr.m);
	//位移
	Matrix mt = MakeTranslataMatrix(mesh.pos.x, mesh.pos.y);
	printf("mesh mt martix:------------\n");
	PrintMatrix(mt.m);
	//一起算
	Multi2Matrix(mr.m, ms.m, srm.m);
	Multi2Matrix(mt.m, srm.m, srtm.m);
	printf("mesh srm martix:------------\n");
	PrintMatrix(srm.m);
	printf("mesh srtm martix:------------\n");
	PrintMatrix(srtm.m);

	mesh.tm = srtm;
	printf("mesh martix:------------\n");
	PrintMatrix(mesh.tm.m);

	for (size_t v = 0; v < mesh.geo.numOfQuad; v++)
	{
		uint32_t vi = v * 8;

		for (size_t i = 0; i < 8; i++)
		{
			printf("vertices[%d]: %.2f\n", i, mesh.geo.vertices[i]);
		}
		Vect2 vertices[4] = {
		   Vect2MultMatrix(MakeVect2(mesh.geo.vertices[vi + 0], mesh.geo.vertices[vi + 1]), srtm.m),
		   Vect2MultMatrix(MakeVect2(mesh.geo.vertices[vi + 2], mesh.geo.vertices[vi + 3]),srtm.m),
		   Vect2MultMatrix(MakeVect2(mesh.geo.vertices[vi + 4], mesh.geo.vertices[vi + 5]) ,srtm.m),
		   Vect2MultMatrix(MakeVect2(mesh.geo.vertices[vi + 6], mesh.geo.vertices[vi + 7]),srtm.m)
		};

		Vect2 p1 = MakeVect2(mesh.geo.vertices[vi + 0], mesh.geo.vertices[vi + 1]);
		Vect2 mp1 = Vect2MultMatrix(p1, srtm.m);
		printf("------- p1 (%.2f,%.2f)\n", p1.x,p1.y);
		printf("mp1 (%.2f,%.2f)", mp1.x, mp1.y);

		printf("\n");
		for (size_t i = 0; i < 4; i++)
		{
			printf("vertices[%d]: (%.2f,%.2f)", i, vertices[i].x, vertices[i].y);
		}
	}
}

void GameIns_Release() {
	ArrayRelease(&_getGameIns()->meshs);
}

void GameIns_Tick(float delta) {

}

GameInstance* _getGameIns() {
	if (_gameIns == NULL) {
		_gameIns = calloc(1, sizeof(GameInstance));
	}
	return _gameIns;
}