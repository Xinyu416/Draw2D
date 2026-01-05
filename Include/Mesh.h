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
	uint32_t id;
	float* vertices; //12*quad  顶点数组			8byte(64位操作系统指针是8字节)
	float* uvs;		 //12*quad  顶点uv数组		8byte	
	uint8_t* colors; //24*quad 顶点颜色数组		8byte 
	uint32_t numOfVertex;//当前quad数				4byte		
	uint32_t maxOfTriangle;//最大quad数				4byte

	//三角面边界盒
	float* triangleBBox;
	//顶点缓存备份
	float* verticesInClipForRender;
}Geometry;//32byte

typedef struct {
	Geometry geo;//32byte
	uint32_t id; //4byte
	Vect2 pos; //8byte
	Matrix tm; //36byte

	//变换矩阵缓存
	Matrix modelTMForRender;
	Matrix cameraTMForRender;
	float rot; //4byte
	Vect2 scale; //8byte
	Material mat;//5byte
	uint8_t block[7];

}Mesh;//96byte

Geometry CreateGeometry(const uint32_t maxOfTriangle);

void ReleaseGeometry(Geometry* geo);

void GeometryAddQuad(Geometry* geo, const Quad quad);

Mesh CreateMesh(const uint32_t id, Vect2 pos, float rot, Vect2 scale, const Geometry geo, const Matrix tm, const Material mat);

void MeshSendToRenderer(Mesh* m);

/*模型空间TM*/
void UpdateModelTM(Mesh* m);

/*相机空间TM*/
void UpdateCameraTM(Mesh* m, Matrix camMatrix);

Vect2* getUVbyType(uint8_t category, uint8_t type, uint32_t wNum, uint32_t hNum);

/*用点乘的符号判断点跟向量的角度 进而判断点是否在四边形内*/
bool IsPointInQuadDotSign(Vect2 p, Vect2 vertices[4]);

void PrintMesh(Mesh* mesh);