#pragma once


#include "z_buffer_culling.h"


class Lighting : public ZCulling
{
public:
	Lighting(USHORT width, USHORT height, const std::string& obj_file);

	void DrawScene();
	int LoadTexture(const std::string& path);

protected:
	void DrawFace(face face);
	std::vector<color> texture;
	int textureHeight, textureWidth;
};