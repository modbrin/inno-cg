#include "projection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <execution>


Projection::Projection(USHORT width, USHORT height, const std::string& obj_file)
    :
    ReadObj(width, height, obj_file)
{}

Projection::~Projection()
{}

void Projection::ProjectScene(std::vector<face>& faces)
{
    auto angle = 30.f * M_PI / 180.f;

    // World Matrix
    float4x4 rotateYM{
        {static_cast<float>(cos(angle)), 0, static_cast<float>(-sin(angle)), 0},
        {0, 1, 0, 0},
        {static_cast<float>(sin(angle)), 0, static_cast<float>(cos(angle)), 0},
        {0, 0, 0, 1}
    };

    float4x4 scaleM{
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    };

    float4x4 translateM{
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,1,1}
    };

    float4x4 worldM = mul(translateM, mul(rotateYM, scaleM));

    // View Matrix
    float3 eyeV{ 0, 0, 0 }, atV{ 0, 0, 1 }, upV{ 0, 1, 0 };
    float3 zaxis{ normalize(atV - eyeV) };
    float3 xaxis{ normalize(cross(upV, zaxis)) };
    float3 yaxis{ cross(zaxis, xaxis) };

    float4x4 viewM{
        {xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eyeV)},
        {yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eyeV)},
        {zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eyeV)},
        {0,0,0,1}
    };

    // Projection Matrix
    float fovY = 90.f * M_PI / 180.f;
    float zn = 1; // z near
    float zf = 10; // z far

    float aspect = static_cast<float>(width / height);
    float yScale = 1 / tan(fovY / 2);
    float xScale = yScale / aspect;

    float4x4 projectionM{
        {xScale,0,0,0},
        {0,yScale,0,0},
        {0,0,zf / (zf - zn),-zn * zf / (zf - zn)},
        {0,0,2,0}
    };

    float4x4 finalM = mul(projectionM, mul(viewM, worldM));

    // apply all matrices
    std::for_each(std::execution::par, faces.begin(), faces.end(), [&](auto& face) {
        for (auto& elem : face.vertices)
            // chained assignment with operator `,`
            elem = (elem = mul(finalM, elem), elem / elem.w);
        });
}

void Projection::DrawScene()
{
    auto result = parser->Parse();
    if (result != 0) throw std::runtime_error("Parsing Failed");

    float scale = static_cast<float>(0.5 *  std::min(width, height));

    auto faces = parser->GetFaces();

    ProjectScene(faces);

    std::array<color, 3> colors{ color(255,0,0), color(0,255,0), color(0,0,255) };

    using namespace linalg::ostream_overloads;

    USHORT wShift = width / 2;
    USHORT hShift = height / 2;

    for (const auto& face : faces)
        for (int i = 0; i < FACE_VERTEX_COUNT; ++i)
        {
            DrawLine(
                scale * face.vertices[i].x + wShift,
                scale * face.vertices[i].y + hShift,
                scale * face.vertices[(i + 1) % FACE_VERTEX_COUNT].x + wShift,
                scale * face.vertices[(i + 1) % FACE_VERTEX_COUNT].y + hShift,
                colors[i % 3]);
        }
}
