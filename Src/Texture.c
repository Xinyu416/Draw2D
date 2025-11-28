#include "Texture.h"

Texture TextureCreate(const uint32_t width, const uint32_t height, const uint8_t bpp, const uint8_t* pixel,uint32_t textureID) {
	Texture texture = { .data = pixel,.width = width,.height = height,.bpp = bpp,.tID = textureID };
	return texture;
}

void TextureRelease(Texture *tex) {
	if (tex->data != NULL)
	{
		free(tex->data);
		tex = NULL;
	}
}
