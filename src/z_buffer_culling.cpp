#include "z_buffer_culling.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <limits>


ZCulling::ZCulling(USHORT width, USHORT height, const std::string& obj_file) : TriangleRasterization(width, height, obj_file)
{
    depth_buffer.resize(width * height);
}

void ZCulling::DrawScene()
{
    auto result = parser->Parse();
    if (result != 0) throw std::runtime_error("Parsing Failed");

    float scale = static_cast<float>(0.5 * std::min(width, height));

    auto faces = parser->GetFaces();

    ProjectScene(faces);

    using namespace linalg::ostream_overloads;

    USHORT wShift = width / 2;
    USHORT hShift = height / 2;

    int counter = 0;
    for (const auto& face : faces) {
        float4 triangle[3] = { face.vertices[0] * scale + float4(wShift, hShift, 0, 0),
                              face.vertices[1] * scale + float4(wShift, hShift, 0, 0),
                              face.vertices[2] * scale + float4(wShift, hShift, 0, 0) };
        DrawTriangle(triangle);
    }
}

#define MAX_Z_DEPTH 1000 //std::numeric_limits<float>().max()

void ZCulling::Clear()
{
    TriangleRasterization::Clear();
    std::fill(depth_buffer.begin(), depth_buffer.end(), MAX_Z_DEPTH);
}

void ZCulling::DrawTriangle(float4 triangle[3])
{
    // calculate signed area and cull negative as it indicates backfacing
    auto areaMul2 = EdgeFunction(triangle[0].xy(), triangle[1].xy(), triangle[2].xy());
    if (areaMul2 < 0) return;

    float2 edge0 = triangle[2].xy() - triangle[1].xy();
    float2 edge1 = triangle[0].xy() - triangle[2].xy();
    float2 edge2 = triangle[1].xy() - triangle[0].xy();

    // determine boundaries
    int xMin = std::min({ triangle[0].x, triangle[1].x, triangle[2].x });
    int xMax = std::max({ triangle[0].x, triangle[1].x, triangle[2].x });
    int yMin = std::min({ triangle[0].y, triangle[1].y, triangle[2].y });
    int yMax = std::max({ triangle[0].y, triangle[1].y, triangle[2].y });

    std::vector<color> colors{ color(255,0,0), color(0,255,0), color(0,0,255) };

    for (int x = xMin; x <= xMax; ++x)
        for (int y = yMin; y <= yMax; ++y)
        {
            // check screen boundaries
            if (x >= width || y >= height || x < 0 || y < 0) continue;
            
            // calculate barycentric coefficients
            auto w0 = EdgeFunction(triangle[1].xy(), triangle[2].xy(), float2(x, y));
            auto w1 = EdgeFunction(triangle[2].xy(), triangle[0].xy(), float2(x, y));
            auto w2 = EdgeFunction(triangle[0].xy(), triangle[1].xy(), float2(x, y));

            // overlapping test with top-left rule
            bool overlaps = true;
            overlaps &= (w0 == 0 ? ((edge0.y == 0 && edge0.x > 0) || edge0.y > 0) : (w0 > 0));
            overlaps &= (w1 == 0 ? ((edge1.y == 0 && edge1.x > 0) || edge1.y > 0) : (w1 > 0));
            overlaps &= (w1 == 0 ? ((edge2.y == 0 && edge2.x > 0) || edge2.y > 0) : (w2 > 0));
            
            // draw pixel if it overlaps the triangle
            if (overlaps)
            {
                w0 /= areaMul2;
                w1 /= areaMul2;
                w2 /= areaMul2;
                auto z = (w0 * triangle[0] + w1 * triangle[1] + w2 * triangle[2]).z;
                SetPixelWithZ(x, y, z, color(w0*255, w1*255, w2*255));
            }
        }
}

void ZCulling::SetPixelWithZ(USHORT x, USHORT y, float z, color color)
{
    if (z < depth_buffer[y * width + x]) {
        frame_buffer[y * width + x] = color;
        depth_buffer[y * width + x] = z;
    }
}

void ZCulling::SetDepth(USHORT x, USHORT y, float depth)
{
    depth_buffer[y * width + x] = depth;
}