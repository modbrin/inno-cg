#pragma once


#include "triangle_rasterization.h"


class ZCulling: public TriangleRasterization
{
public:
	ZCulling(USHORT width, USHORT height, const std::string& obj_file);

	void DrawScene();
	void Clear();

protected:
	void SetPixelWithZ(USHORT x, USHORT y, float z, color color);
	void DrawTriangle(float4 triangle[3]);
	void SetDepth(USHORT x, USHORT y, float depth);
	std::vector<float> depth_buffer;
};
