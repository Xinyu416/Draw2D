#include "Mesh.h"

Geometry CreateGeometry(const uint32_t maxOfQuads) {
	Geometry geo = { .maxOfQuad = maxOfQuads,.numOfQuad = 0 };
	geo.vertices = (float*)malloc(sizeof(float) * maxOfQuads * 8);
	geo.uvs = (float*)malloc(sizeof(float) * maxOfQuads * 8);
	geo.colors = (uint8_t*)malloc(sizeof(uint8_t) * maxOfQuads * 16);
	return geo;
}

void ReleaseGeometry(Geometry* geo) {
	if (!geo->maxOfQuad == 0)
	{
		free(geo->vertices);
		free(geo->uvs);
		free(geo->colors);
	}
}

void GeometryAddQuad(Geometry* geo, const Quad quad) {
	if (geo->numOfQuad >= geo->maxOfQuad)
	{
		printf("out of limit\n");
		return;
	}
	memcpy(geo->vertices + (geo->numOfQuad * 8), quad.vertices, sizeof(quad.vertices));
	memcpy(geo->uvs + (geo->numOfQuad * 8), quad.uvs, sizeof(quad.uvs));
	memcpy(geo->colors + (geo->numOfQuad * 16), quad.color, sizeof(quad.color));
	geo->numOfQuad++;
}

Mesh CreateMesh(const uint32_t id, Vect2 pos, float rot, const Geometry geo, const Matrix tm, const Material mat) {
	Mesh mesh = { .id = id,.pos = pos,.rot = rot,.geo = geo,.tm = tm,.mat = mat };
	return mesh;
}

bool IsPointOnSegment(Vect2 p, Vect2 a, Vect2 b) {
	// 检查点p是否在线段ab上
	float cross = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
	float epsilon = 0.001;
	if (fabsf(cross) > epsilon) return false; // 不共线

	// 在线段范围内
	return (p.x >= fminf(a.x, b.x) && p.x <= fmaxf(a.x, b.x) &&
		p.y >= fminf(a.y, b.y) && p.y <= fmaxf(a.y, b.y));
}

bool IsPointInQuadTotalAngle(Vect2 p, Quad quad) {
	//1.点跟四个顶点做连线点乘求角度和 等于360度必在里面 不等于在外面
	//点乘 arcos（v）=(rad2deg) 得到角度

	//printf("p sides:\n");
	//Vect2 pSide0 = SubVect2(p, quad.vertices[0]);
	//PrintVect2(pSide0);
	//Vect2 pSide1 = SubVect2(p, quad.vertices[1]);
	//PrintVect2(pSide1);
	//Vect2 pSide2 = SubVect2(p, quad.vertices[2]);
	//PrintVect2(pSide2);
	//Vect2 pSide3 = SubVect2(p, quad.vertices[3]);
	//PrintVect2(pSide3);

	//float deg0 = Rad2Deg(acosf(dotVect2(Vect2Normalize(pSide0), Vect2Normalize(pSide1))));
	//float deg1 = Rad2Deg(acosf(dotVect2(Vect2Normalize(pSide1), Vect2Normalize(pSide2))));
	//float deg2 = Rad2Deg(acosf(dotVect2(Vect2Normalize(pSide2), Vect2Normalize(pSide3))));
	//float deg3 = Rad2Deg(acosf(dotVect2(Vect2Normalize(pSide3), Vect2Normalize(pSide0))));

	float total = 0;
	for (int i = 0; i < 4; i++) {

		Vect2 v1 = SubVect2(p, quad.vertices[i]);
		Vect2 v2 = SubVect2(p, quad.vertices[(i + 1) % 4]);

		float angle = Rad2Deg(acosf(dotVect2(Vect2Normalize(v1), Vect2Normalize(v2))));
		total += angle;
	}
	//printf("total:%.2f\n", total);
	float epsilon = 0.001; // 定义一个很小的数作为误差容忍度
	if (fabsf(total - 360.f) < epsilon)
	{
		printf("点在框内");
		return true;
	}

	printf("点在框外");
	return false;
}

bool IsPointInConvexQuad(Vect2 p, Quad quad) {
	//2.点跟四个边做叉乘 确定方向
	// 计算叉积的z分量 >0 左侧 <0右侧 =0线上（重合）
	// 四边形顶点按逆时针顺序
	for (int i = 0; i < 4; i++) {
		Vect2 a = quad.vertices[i];
		Vect2 b = quad.vertices[(i + 1) % 4];
		Vect2 ab = SubVect2(a, b);
		Vect2 ap = SubVect2(a, p);
		float cross = crossVect2(Vect2Normalize(ab), Vect2Normalize(ap));
		// 如果点在边的左侧，则不在内部
		if (cross > 0) {
			return false;
		}
	}
	return true;
}

bool IsPointInQuadRayCast(Vect2 p, Quad quad) {
	//3.射线法，奇数个相交为内 偶数个为外
	int crossCount = 0;
	for (int i = 0; i < 4; i++) {
		Vect2 v1 = quad.vertices[i];
		Vect2 v2 = quad.vertices[(i + 1) % 4];

		// 检查点是否在边上
		if (IsPointOnSegment(p, v1, v2)) {
			return true;
		}

		// 射线与边相交检测
		if ((v1.y > p.y) != (v2.y > p.y) &&
			(p.x < (v2.x - v1.x) * (p.y - v1.y) / (v2.y - v1.y) + v1.x)) {
			crossCount++;
		}
	}
	return (crossCount % 2) == 1;
}

bool IsPointInQuad(Vect2 p, Quad quad) {

	//return IsPointInQuadTotalAngle(p,quad);
	//return IsPointInConvexQuad(p,quad);
	return IsPointInConvexQuad(p, quad);
}