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
	//7 豆子 8 特殊道具 1-6 地形
	uint32_t data[28 * 31] = {
		3,1,1,1,1,1,1,1,1,1,1,1,1,5,3,1,1,1,1,1,1,1,1,1,1,1,1,5,
		4,7,7,7,7,7,7,7,7,7,7,7,7,4,4,7,7,7,7,7,7,7,7,7,7,7,7,4,
		4,7,3,1,1,5,7,3,1,1,1,5,7,4,4,7,3,1,1,1,5,7,3,1,1,5,7,4,
		4,8,4,6,6,4,7,4,6,6,6,4,7,4,4,7,4,6,6,6,4,7,4,6,6,4,8,4,
		4,7,0,1,1,2,7,0,1,1,1,2,7,0,2,7,0,1,1,1,2,7,0,1,1,2,7,4,
		4,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,
		4,7,3,1,1,5,7,3,5,7,3,1,1,1,1,1,1,5,7,3,5,7,3,1,1,5,7,4,
		4,7,0,1,1,2,7,4,4,7,0,1,1,5,3,1,1,2,7,4,4,7,0,1,1,2,7,4,
		4,7,7,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,7,7,4,
		0,1,1,1,1,5,7,4,0,1,1,5,7,4,4,7,3,1,1,2,4,7,3,1,1,1,1,2,
		6,6,6,6,6,4,7,4,3,1,1,2,7,0,2,7,0,1,1,5,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,7,7,7,7,7,7,7,7,7,4,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,3,1,1,1,1,1,1,5,7,4,4,7,4,6,6,6,6,6,
		1,1,1,1,1,2,7,0,2,7,4,6,6,6,6,6,6,4,7,0,2,7,0,1,1,1,1,1,
		7,7,7,7,7,7,7,7,7,7,4,6,6,6,6,6,6,4,7,7,7,7,7,7,7,7,7,7,
		1,1,1,1,1,5,7,3,5,7,4,6,6,6,6,6,6,4,7,3,5,7,3,1,1,1,1,1,
		6,6,6,6,6,4,7,4,4,7,0,1,1,1,1,1,1,2,7,4,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,7,7,7,7,7,7,7,7,7,4,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,3,1,1,1,1,1,1,5,7,4,4,7,4,6,6,6,6,6,
		3,1,1,1,1,2,7,0,2,7,0,1,1,5,3,1,1,2,7,0,2,7,0,1,1,1,1,5,
		4,7,7,7,7,7,7,7,7,7,7,7,7,4,4,7,7,7,7,7,7,7,7,7,7,7,7,4,
		4,7,3,1,1,5,7,3,1,1,1,5,7,4,4,7,3,1,1,1,5,7,3,1,1,5,7,4,
		4,7,0,1,5,4,7,0,1,1,1,2,6,0,2,6,0,1,1,1,2,7,4,3,1,2,7,4,
		4,8,7,7,4,4,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,4,7,7,8,4,
		0,1,5,7,4,4,7,3,5,7,3,1,1,1,1,1,1,5,7,3,5,7,4,4,7,3,1,2,
		3,1,2,7,0,2,7,4,4,7,0,1,1,5,3,1,1,2,7,4,4,7,0,2,7,0,1,5,
		4,7,7,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,7,7,4,
		4,7,3,1,1,1,1,2,0,1,1,5,7,4,4,7,3,1,1,2,0,1,1,1,1,5,7,4,
		4,7,0,1,1,1,1,1,1,1,1,2,7,0,2,7,0,1,1,1,1,1,1,1,1,2,7,4,
		4,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,
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
	uint32_t wNum = _getMapData()->wNum;
	uint32_t hNum = _getMapData()->hNum;
	uint32_t texWidth = 3;
	uint32_t texHeight = 3;
	Geometry geo = CreateGeometry(wNum * hNum);
	//坐标原点 根据Camera位置和窗口设计换算来的
	Vect2 meshPos = MakeVect2(350.f, 387.5f);
	uint8_t type = 0;
	uint32_t index = 0;
	//以中心对齐的方式建mesh
	for (size_t y = 0; y < hNum; y++)
	{
		for (size_t x = 0; x < wNum; x++)
		{
			type = _getMapData()->typeData[index];
			Vect2* uvs = getUVbyType(type, texWidth, texHeight);
			Vect2* vertices = getVeticesbyCoord(x, y, meshPos, GridSize);
			Quad quad = {
						.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
						 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
						.type = type
			};
			GeometryAddQuad(&geo, quad);
			index++;
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 1 };
	Mesh mesh = CreateMesh(1, meshPos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateBeansData() {
	uint32_t wNum = _getMapData()->wNum;
	uint32_t hNum = _getMapData()->hNum;
	uint8_t type = 0;
	uint32_t count = 0;
	//统计quad数量
	for (size_t i = 0; i < wNum * hNum; i++)
	{
		if (_getMapData()->typeData[i] == 7) {
			count++;
		}
	}
	Geometry geo = CreateGeometry(count);
	float posx = 350.f;
	float posy = 387.5f;
	//以中心对齐的方式建mesh
	uint32_t index = 0;

	for (size_t y = 0; y < hNum; y++)
	{
		for (size_t x = 0; x < wNum; x++)
		{
			type = _getMapData()->typeData[index];

			if (type == 7)
			{
				Vect2* vertices = getVeticesbyCoord(x, y, MakeVect2(posx, posy), GridSize);

				Quad quad = {
					.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
							 .uvs = {0.f,0.f, 1.f,0.f, 1.f,1.f, 1.f,1.f, 0.f,1.f, 0.f,0.f},
							.type = type
				};
				GeometryAddQuad(&geo, quad);
			}
			index++;
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 2 };
	Mesh mesh = CreateMesh(2, MakeVect2(posx, posy), 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateItemData() {
	uint32_t wNum = _getMapData()->wNum;
	uint32_t hNum = _getMapData()->hNum;
	printf("GenerateBeansData:: wNum:%d,hNum%d\n", wNum, hNum);
	uint32_t count = 0;
	//统计quad数量
	for (size_t i = 0; i < wNum * hNum; i++)
	{
		if (_getMapData()->typeData[i] == 8) {
			count++;
		}
	}
	Geometry geo = CreateGeometry(count);
	uint8_t type = 0;
	float posx = 350.f;
	float posy = 387.5f;
	//以中心对齐的方式建mesh
	uint32_t index = 0;
	for (size_t y = 0; y < hNum; y++)
	{
		for (size_t x = 0; x < wNum; x++)
		{
			type = _getMapData()->typeData[index];

			if (type == 8)
			{
				Vect2* vertices = getVeticesbyCoord(x, y, MakeVect2(posx, posy), GridSize);
				Quad quad = {
					.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
							 .uvs = {0.f,0.f, 1.f,0.f, 1.f,1.f, 1.f,1.f, 0.f,1.f, 0.f,0.f},
							.type = type
				};
				GeometryAddQuad(&geo, quad);
			}
			index++;
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 3 };
	Mesh mesh = CreateMesh(3, MakeVect2(posx, posy), 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}