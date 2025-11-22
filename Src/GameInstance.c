#include "GameInstance.h"
#include "Mesh.h"

static GameInstance* _gameIns = NULL;
void GameIns_Init() {
	GameInstance* gameIns = _getGameIns();
	gameIns->meshs = ArrayCreate(sizeof(Mesh));
	gameIns->cMesh = NULL;
	CreateMeshTest();
}

void CreateMeshTest() {
	Quad quad1 = {
		.vertices = {-100, -10, 100, -10, 100, -100, -100, -100},
		.uvs = {0, 0, 0.2f, 0.3f, 0.8f, 0.5f, 1, 1},
		.color = {100, 100, 100, 100, 200, 200, 200, 200, 150, 150, 150, 150, 10, 10, 10, 10}
	};
	//quad1.vertices[0] = MakeVect2(-50, -10);
	//quad1.vertices[1] = MakeVect2(50, -10);
	//quad1.vertices[2] = MakeVect2(50, -50);
	//quad1.vertices[3] = MakeVect2(-50, -50);
	//quad1.uvs[0] = MakeVect2(0, 0);
	//quad1.uvs[1] = MakeVect2(0.2f, 0.3f);
	//quad1.uvs[2] = MakeVect2(0.8f, 0.5f);
	//quad1.uvs[3] = MakeVect2(1, 1);
	//quad1.color[0] = MakeColor4(100, 100, 100, 100);
	//quad1.color[1] = MakeColor4(200, 200, 200, 200);
	//quad1.color[2] = MakeColor4(150, 150, 150, 150);
	//quad1.color[3] = MakeColor4(10, 10, 10, 10);

	Quad quad2 = { .vertices = {-50, 50,-20, 50,-20, 20,-50, 20},
				.color = {100, 100, 100, 100,200, 200, 200, 200,150, 150, 150, 150,10, 10, 10, 10}
	};

	Quad quad3 = { .vertices = {30, 60,60, 60,60, 30, 30, 30},
				.color = {100, 100, 100, 100,200, 200, 200, 200,150, 150, 150, 150,10, 10, 10, 10}
	};

	Quad quad4 = { .vertices = { -4, 4, 4, 4, 4, -4, -4, -4},
				.color = {100, 100, 100, 100,200, 200, 200, 200,150, 150, 150, 150,10, 10, 10, 10}
	};

	Geometry geo = CreateGeometry(4);
	GeometryAddQuad(&geo, quad1);
	GeometryAddQuad(&geo, quad2);
	GeometryAddQuad(&geo, quad3);
	GeometryAddQuad(&geo, quad4);
	printf("numOfQuad:%d\n", geo.numOfQuad);

	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 1 };
	Mesh mesh = CreateMesh(1, MakeVect2(400.f, 300.f), 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);

	Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 0);
	_getGameIns()->cMesh = pmesh;

	//PrintMesh(&mesh);
	//PrintArray(&_getGameIns()->meshs);
}

void GameIns_Release() {
	ArrayRelease(&_getGameIns()->meshs);
}

void GameIns_Tick(float delta) {
	//Ðý×ªmesh
	_getGameIns()->cMesh->rot += 2.f;
}

GameInstance* _getGameIns() {
	if (_gameIns == NULL) {
		_gameIns = calloc(1, sizeof(GameInstance));
	}
	return _gameIns;
}