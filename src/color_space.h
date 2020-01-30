#pragma once

#include "black_image.h"


class ColorSpace : public BlackImage
{
public:
	ColorSpace(unsigned short width, unsigned short height);

	void DrawScene();

protected:
	void SetPixel(USHORT x, USHORT y, color color);
};
