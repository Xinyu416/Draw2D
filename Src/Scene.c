#include "Scene.h"

static MapData* mapData = NULL;

MapData* _getMapData() {
	if (mapData == NULL) {
		mapData = calloc(1, sizeof(MapData));
	}
	return mapData;
}

void SetMapData() {
	uint32_t wNum = _getGameEngine()->width / GridSize;
	uint32_t hNum = _getGameEngine()->height / GridSize;
	uint32_t* typeData = malloc(sizeof(uint32_t) * wNum * hNum);
	uint32_t data[28 * 31] = {
		3,1,1,1,1,1,1,1,1,1,1,1,1,5,3,1,1,1,1,1,1,1,1,1,1,1,1,5,
		4,6,6,6,6,6,6,6,6,6,6,6,6,4,4,6,6,6,6,6,6,6,6,6,6,6,6,4,
		4,6,3,1,1,5,6,3,1,1,1,5,6,4,4,6,3,1,1,1,5,6,3,1,1,5,6,4,
		4,6,4,6,6,4,6,4,6,6,6,4,6,4,4,6,4,6,6,6,4,6,4,6,6,4,6,4,
		4,6,0,1,1,2,6,0,1,1,1,2,6,0,2,6,0,1,1,1,2,6,0,1,1,2,6,4,
		4,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,
		4,6,3,1,1,5,6,3,5,6,3,1,1,1,1,1,1,5,6,3,5,6,3,1,1,5,6,4,
		4,6,0,1,1,2,6,4,4,6,0,1,1,5,3,1,1,2,6,4,4,6,0,1,1,2,6,4,
		4,6,6,6,6,6,6,4,4,6,6,6,6,4,4,6,6,6,6,4,4,6,6,6,6,6,6,4,
		0,1,1,1,1,5,6,4,0,1,1,5,6,4,4,6,3,1,1,2,4,6,3,1,1,1,1,2,
		6,6,6,6,6,4,6,4,3,1,1,2,6,0,2,6,0,1,1,5,4,6,4,6,6,6,6,6,
		6,6,6,6,6,4,6,4,4,6,6,6,6,6,6,6,6,6,6,4,4,6,4,6,6,6,6,6,
		6,6,6,6,6,4,6,4,4,6,3,1,1,1,1,1,1,5,6,4,4,6,4,6,6,6,6,6,
		1,1,1,1,1,2,6,0,2,6,4,6,6,6,6,6,6,4,6,0,2,6,0,1,1,1,1,1,
		6,6,6,6,6,6,6,6,6,6,4,6,6,6,6,6,6,4,6,6,6,6,6,6,6,6,6,6,
		1,1,1,1,1,5,6,3,5,6,4,6,6,6,6,6,6,4,6,3,5,6,3,1,1,1,1,1,
		6,6,6,6,6,4,6,4,4,6,0,1,1,1,1,1,1,2,6,4,4,6,4,6,6,6,6,6,
		6,6,6,6,6,4,6,4,4,6,6,6,6,6,6,6,6,6,6,4,4,6,4,6,6,6,6,6,
		6,6,6,6,6,4,6,4,4,6,3,1,1,1,1,1,1,5,6,4,4,6,4,6,6,6,6,6,
		3,1,1,1,1,2,6,0,2,6,0,1,1,5,3,1,1,2,6,0,2,6,0,1,1,1,1,5,
		4,6,6,6,6,6,6,6,6,6,6,6,6,4,4,6,6,6,6,6,6,6,6,6,6,6,6,4,
		4,6,3,1,1,5,6,3,1,1,1,5,6,4,4,6,3,1,1,1,5,6,3,1,1,5,6,4,
		4,6,0,1,5,4,6,0,1,1,1,2,6,0,2,6,0,1,1,1,2,6,4,3,1,2,6,4,
		4,6,6,6,4,4,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,6,6,6,4,
		0,1,5,6,4,4,6,3,5,6,3,1,1,1,1,1,1,5,6,3,5,6,4,4,6,3,1,2,
		3,1,2,6,0,2,6,4,4,6,0,1,1,5,3,1,1,2,6,4,4,6,0,2,6,0,1,5,
		4,6,6,6,6,6,6,4,4,6,6,6,6,4,4,6,6,6,6,4,4,6,6,6,6,6,6,4,
		4,6,3,1,1,1,1,2,0,1,1,5,6,4,4,6,3,1,1,2,0,1,1,1,1,5,6,4,
		4,6,0,1,1,1,1,1,1,1,1,2,6,0,2,6,0,1,1,1,1,1,1,1,1,2,6,4,
		4,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,
		0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2
		
	};
	for (size_t i = 0; i < wNum * hNum; i++)
	{
		typeData[i] = data[i];
	}
	MapData* mapData = _getMapData();
	mapData->wNum = wNum;
	mapData->hNum = hNum;
	mapData->typeData = typeData;
}

void GenerateGridData() {
	uint32_t wNum = _getGameEngine()->width / GridSize;
	uint32_t hNum = _getGameEngine()->height / GridSize;
	printf("wNum:%d,hNum%d\n", wNum, hNum);
	Geometry geo = CreateGeometry(wNum * hNum);

	float posx = 350.f;
	float posy = 387.5f;
	uint8_t type = 0;
	//以中心对齐的方式建mesh
	uint32_t index = 0;
	for (size_t y = 0; y < hNum; y++)
	{
		for (size_t x = 0; x < wNum; x++)
		{
			type = _getMapData()->typeData[index];
			/*	if (x == 0 && y == 0)
				{
					type = 3;
				}
				if (y == 0)
				{
					if ((x > 0 && x < 12) || (x > 15 && x < wNum - 1))
					{
						type = 1;
					}
					if (x == 14 || x == wNum - 1)
					{
						type = 5;
					}
					if (x == 15)
					{
						type = 3;
					}
				}*/
			Vect2* uvs = getUVbyType(type, 3, 3);
			Vect2 p0 = MakeVect2(x * GridSize - posx, (y + 1) * GridSize - posy);
			Vect2 p1 = MakeVect2((x + 1) * GridSize - posx, (y + 1) * GridSize - posy);
			Vect2 p2 = MakeVect2((x + 1) * GridSize - posx, y * GridSize - posy);
			Vect2 p2_2 = MakeVect2((x + 1) * GridSize - posx, y * GridSize - posy);
			Vect2 p3 = MakeVect2(x * GridSize - posx, y * GridSize - posy);
			Vect2 p0_0 = MakeVect2(x * GridSize - posx, (y + 1) * GridSize - posy);

			Quad quad = { .vertices = {p0,p1,p2,p2_2,p3,p0_0,},
						 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]} };
			GeometryAddQuad(&geo, quad);
			index++;
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 5 };
	Mesh mesh = CreateMesh(3, MakeVect2(posx, posy), 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateBeansData() {
	uint32_t wNum = _getGameEngine()->width / GridSize;
	uint32_t hNum = _getGameEngine()->height / GridSize;
	printf("wNum:%d,hNum%d\n", wNum, hNum);
	Geometry geo = CreateGeometry(wNum * hNum);

	float posx = 350.f;
	float posy = 387.5f;
	//以中心对齐的方式建mesh
	uint32_t index = 0;
	for (size_t y = 0; y < hNum; y++)
	{
		for (size_t x = 0; x < wNum; x++)
		{
			
			Vect2 p0 = MakeVect2(x * GridSize - posx, (y + 1) * GridSize - posy);
			Vect2 p1 = MakeVect2((x + 1) * GridSize - posx, (y + 1) * GridSize - posy);
			Vect2 p2 = MakeVect2((x + 1) * GridSize - posx, y * GridSize - posy);
			Vect2 p2_2 = MakeVect2((x + 1) * GridSize - posx, y * GridSize - posy);
			Vect2 p3 = MakeVect2(x * GridSize - posx, y * GridSize - posy);
			Vect2 p0_0 = MakeVect2(x * GridSize - posx, (y + 1) * GridSize - posy);

			Quad quad = { .vertices = {p0,p1,p2,p2_2,p3,p0_0,},
						 .uvs = {0.f,0.f, 1.f,0.f, 1.f,1.f, 1.f,1.f, 0.f,1.f, 0.f,0.f}
			};
			GeometryAddQuad(&geo, quad);
			index++;
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 4 };
	Mesh mesh = CreateMesh(4, MakeVect2(posx, posy), 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}
