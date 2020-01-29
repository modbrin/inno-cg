#include "triangle_rasterization.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <numeric>


TriangleRasterization::TriangleRasterization(USHORT width, USHORT height, const std::string& obj_file) : Projection(width, height, obj_file)
{
}

TriangleRasterization::~TriangleRasterization()
{
}

void TriangleRasterization::DrawScene()
{
    auto result = parser->Parse();
    if (result != 0) throw std::runtime_error("Parsing Failed");

    float scale = static_cast<float>(0.5 * std::min(width, height));

    auto faces = parser->GetFaces();

    ProjectScene(faces);

    using namespace linalg::ostream_overloads;

    USHORT wShift = width / 2;
    USHORT hShift = height / 2;

    for (const auto& face : faces) {
        float4 triangle[3] = {face.vertices[0] * scale + float4(wShift, hShift, 0, 1),
                              face.vertices[1] * scale + float4(wShift, hShift, 0, 1),
                              face.vertices[2] * scale + float4(wShift, hShift, 0, 1) };
        DrawTriangle(triangle);
    }

    /*for (const auto& face : faces)
        for (int i = 0; i < FACE_VERTEX_COUNT; ++i)
        {
            DrawLine(
                scale * face.vertices[i].x + wShift,
                scale * face.vertices[i].y + hShift,
                scale * face.vertices[(i + 1) % FACE_VERTEX_COUNT].x + wShift,
                scale * face.vertices[(i + 1) % FACE_VERTEX_COUNT].y + hShift,
                color(255, 255, 255));
        }*/
}

void TriangleRasterization::DrawTriangle(float4 triangle[3])
{
    static int color_selector = 0;

    // backface culling implemented in z_buffer_culling part

    // determine boundaries
	USHORT xMin = std::min({ triangle[0].x, triangle[1].x, triangle[2].x });
	USHORT xMax = std::max({ triangle[0].x, triangle[1].x, triangle[2].x });
    USHORT yMin = std::min({ triangle[0].y, triangle[1].y, triangle[2].y });
    USHORT yMax = std::max({ triangle[0].y, triangle[1].y, triangle[2].y });

    std::vector<color> colors{ color(255,0,0), color(0,255,0), color(0,0,255) };

	for (USHORT x = xMin; x <= xMax; ++x)
		for (USHORT y = yMin; y <= yMax; ++y)
		{
			if (EdgeFunction(triangle[0].xy(), triangle[1].xy(), float2(x, y)) > 0 &&
				EdgeFunction(triangle[1].xy(), triangle[2].xy(), float2(x, y)) > 0 &&
				EdgeFunction(triangle[2].xy(), triangle[0].xy(), float2(x, y)) > 0)
				SetPixel(x, y, colors[color_selector%colors.size()]);
		}

    ++color_selector;
}

float TriangleRasterization::EdgeFunction(float2 a, float2 b, float2 c)
{
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}


