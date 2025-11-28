#pragma once
#include "Define.h"

typedef struct {
	//一个quad管理两个三角面
	Vect2 vertices[6]; //12*4
	Vect2 uvs[6];	   //12*4	
	Color4 color[6];   //24
	uint8_t type;		//类型换取uv值
}Quad;//120 byte

typedef struct {
	Color4 color;			//4
	uint8_t textureId;		//1
}Material;//5 byte

typedef struct {
	//改为三角面
	float* vertices; //12*quad  顶点数组			8byte(64位操作系统指针是8字节)
	float* uvs;		 //12*quad  顶点uv数组		8byte	
	uint8_t* colors; //24*quad 顶点颜色数组		8byte 
	uint32_t numOfQuad;//当前quad数				4byte		
	uint32_t maxOfQuad;//最大quad数				4byte
}Geometry;//32byte

typedef struct {
	Geometry geo;//32byte
	uint32_t id; //4byte
	Vect2 pos; //8byte
	Matrix tm; //36byte
	float rot; //4byte
	Vect2 scale; //8byte
	Material mat;//5byte
	uint8_t block[7];
}Mesh;//96byte

Geometry CreateGeometry(const uint32_t maxOfQuads);

void ReleaseGeometry(Geometry* geo);

void GeometryAddQuad(Geometry* geo, const Quad quad);

Mesh CreateMesh(const uint32_t id, Vect2 pos,float rot, Vect2 scale, const Geometry geo,const Matrix tm,const Material mat);

Vect2* getUVbyType(uint8_t category,uint8_t type, uint32_t wNum, uint32_t hNum);

Vect2* getVeticesbyCoord(uint32_t x, uint32_t y,Vect2 offset, float gridSize);

/*用点乘的符号判断点跟向量的角度 进而判断点是否在四边形内*/
bool IsPointInQuadDotSign(Vect2 p, Vect2 vertices[4]);

void PrintMesh(Mesh* mesh);