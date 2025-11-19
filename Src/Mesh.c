#include "Mesh.h"

Geometry CreateGeometry(const uint32_t maxOfQuads) {
	Geometry geo = { .maxOfQuad = maxOfQuads,.numOfQuad = 0};
	geo.vertices = (float*)malloc(sizeof(float)*maxOfQuads*8);
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
	
	geo->numOfQuad++;
}

Mesh CreateMesh(const uint32_t id, const Geometry geo, const Matrix tm, const Material mat) {

}