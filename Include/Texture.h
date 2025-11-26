#pragma once
#include "Define.h"

typedef struct {
	uint8_t* data;
	uint32_t width;
	uint32_t height;
	uint32_t tID;
	uint8_t bpp;
}Texture;

Texture TextureCreate(const uint32_t width, const uint32_t height, const uint8_t bpp, const uint8_t* pixel, uint32_t textureID);

void TextureRelease(Texture* tex);
