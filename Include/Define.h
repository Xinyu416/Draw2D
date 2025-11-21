#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159

typedef struct {
	float x;
	float y;
}Vect2;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
}Color4;

typedef struct {
	float m[9];
}Matrix;


inline float Rad2Deg(float rad) {
	float deg = rad * (180 / PI);
	return deg;
}

inline float Deg2Rad(float deg) {
	float rad = deg * (PI / 180);
	return rad;
}

inline Vect2 MakeVect2(const float x, const float y) {
	Vect2 v = { .x = x,.y = y };
	return v;
}

inline Color4 MakeColor4(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
	Color4 out = { .r = r,.g = g,.b = b,.a = a };
	return out;
}

inline float Vect2Len(const Vect2 v) {
	return (float)sqrt(v.x * v.x + v.y * v.y);
}

inline Vect2 Vect2Normalize(const Vect2 v) {
	//v' = (x/||v||, y/||v||)
	float vlen = Vect2Len(v);
	Vect2 out = { .x = v.x / vlen ,.y = v.y / vlen };
	return out;
}

inline Vect2 Vect2Rot(const Vect2 v, float angleInRad) {
	//x’= cos(β)*x - sin(β)*y
	//y’= sin(β)*x + cos(β)*y
	Vect2 out = { .x = cosf(angleInRad) * v.x - sinf(angleInRad) * v.y,.y = sinf(angleInRad) * v.x + cosf(angleInRad) * v.y };
	return out;
}

//点乘 arcos（v）=(rad2deg) 得到角度
inline float dotVect2(const Vect2 v1, const Vect2 v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

//>0 左侧 <0右侧 =0线上（重合）
inline float crossVect2(const Vect2 v1, const Vect2 v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

inline Vect2 AddVect2(const Vect2 v1, const Vect2 v2) {
	Vect2 v = { .x = v1.x + v2.x,.y = v1.y + v2.y };
	return v;
}

//p1 指向 p2 的向量
inline Vect2 SubVect2(const Vect2 p1, const Vect2 p2) {
	Vect2 v = { .x = p2.x - p1.x,.y = p2.y - p1.y };
	return v;
}


inline PrintVect2(const Vect2 v) {
	printf("[x:%.2f,y:%.2f]\n", v.x, v.y);
}

inline Matrix CreateStandardMatrix() {
	Matrix m = {
		1,0,0,
		0,1,0,
		0,0,1
	};
	return m;
}
//矩阵左乘 m*v 列矩阵
inline Vect2 Vect2MultMatrix(const Vect2 v, float* m) {
	Vect2 out = { 0 };
	out.x = v.x * m[0] + v.y * m[1] + m[2];
	out.y = v.x * m[3] + v.y * m[4] + m[5];
	return out;
}

//移动后的向量变换成矩阵
inline Matrix MakeTranslataMatrix(const float x, const float y) {
	Matrix m = {
		.m = {
		1,0,x,
		0,1,y,
		0,0,1
		}
	};
	return m;
}

//缩放后的向量变换成矩阵
inline Matrix MakeScaMatrix(const float x, const float y) {
	Matrix m = {
		.m = {
		x,0,0,
		0,y,0,
		0,0,1
		}
	};
	return m;
}

//旋转后的向量变换成矩阵
inline Matrix MakeRotMatrix(float angleRad) {
	Matrix m = {
		.m = {
		cosf(angleRad),-sinf(angleRad),0,
		sinf(angleRad),cosf(angleRad),0,
		0,0,1
		}
	};
	return m;
}

//[0,1,2]   [0,1,2]
//[3,4,5] x [3,4,5]
//[6,7,8]   [6,7,8]
//矩阵相乘 父子层级转换
inline void Multi2Matrix(const float* a, const float* b, float* out) {
	out[0] = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
	out[1] = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
	out[2] = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];

	out[3] = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
	out[4] = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
	out[5] = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];

	out[6] = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
	out[7] = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
	out[8] = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
}

inline void PrintMatrix(float* m) {
	printf("[%.2f,%.2f,%.2f]\n[%.2f,%.2f,%.2f]\n[%.2f,%.2f,%.2f]\n", m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
}


