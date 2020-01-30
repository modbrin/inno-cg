#pragma once

#include <linalg.h>
using namespace linalg::aliases;

struct color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	color()
		:
		r(0),
		g(0),
		b(0)
	{}

	color(unsigned char in_r, unsigned char in_g, unsigned char in_b)
		:
		r(in_r),
		g(in_g),
		b(in_b)
	{}

	inline bool operator==(const color& rhs) const
	{
		return r == rhs.r && g == rhs.g && b == rhs.b;
	}
};

#define FACE_VERTEX_COUNT 3
struct face
{
	float4 vertices[FACE_VERTEX_COUNT];
	float3 texCoords[FACE_VERTEX_COUNT];
	float3 normals[FACE_VERTEX_COUNT];
	bool texCoordsPresent;
	bool normalsPresent;
};
