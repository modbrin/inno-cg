#include "lighting.h"

#include <iostream>
#include <execution>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb.h"


Lighting::Lighting(USHORT width, USHORT height, const std::string& obj_file)
    : ZCulling(width, height, obj_file)
{
}

// this duplicate is needed for quick linking error fix
bool validate_framebuffer(std::string reference_file, std::vector<color> frame_buffer)
{
    // Load a reference image
    int width, height, channels;
    unsigned char* img = stbi_load(reference_file.c_str(), &width, &height, &channels, 0);

    if (!img) return false;

    // Convert the reference to vector of colors
    std::vector<color> reference;
    for (int i = 0; i < width * height; i++)
    {
        color pixel;
        pixel.r = img[channels * i];
        pixel.g = img[channels * i + 1];
        pixel.b = img[channels * i + 2];
        reference.push_back(pixel);
    }

    // Compare with a frame buffer
    bool result = (reference.size() == frame_buffer.size());
    for (unsigned int i = 0; i < 1920 * 1080; i++)
    {
        result &= (reference[i] == frame_buffer[i]);
    }

    return result;

}


int Lighting::LoadTexture(const std::string& path)
{
    // Load a reference image
    int channels;
    unsigned char* img = stbi_load(path.c_str(), &textureWidth, &textureHeight, &channels, 0);

    if (!img)
        return 1;

    // Convert the reference to vector of colors
    for (int i = 0; i < textureWidth * textureHeight; ++i)
    {
        color pixel;
        pixel.r = img[channels * i];
        pixel.g = img[channels * i + 1];
        pixel.b = img[channels * i + 2];
        texture.push_back(pixel);
    }

    return 0;
}

// rotation angles are in degrees [0..360), order: scale->rotate->move
void Lighting::TransformObject(std::vector<face>& faces, float moveX, float moveY, float moveZ, float rotateX, float rotateY, float rotateZ, float scale)
{

    // matrices are taken from https://docs.microsoft.com/en-us/windows/win32/direct3d9/transforms
    
    float4x4 scaleM{
        {scale, 0, 0, 0},
        {0, scale, 0, 0},
        {0, 0, scale, 0},
        {0, 0, 0, 1}
    };

    auto angleX = rotateX * M_PI / 180.f;

    float4x4 rotateXM{
        {1, 0, 0, 0},
        {0, static_cast<float>(cos(angleX)), static_cast<float>(-sin(angleX)), 0},
        {0, static_cast<float>(sin(angleX)), static_cast<float>(cos(angleX)), 0},
        {0, 0, 0, 1}
    };

    auto angleY = rotateY * M_PI / 180.f;

    float4x4 rotateYM{
        {static_cast<float>(cos(angleY)), 0, static_cast<float>(sin(angleY)), 0},
        {0, 1, 0, 0},
        {static_cast<float>(-sin(angleY)), 0, static_cast<float>(cos(angleY)), 0},
        {0, 0, 0, 1}
    };

    auto angleZ = rotateZ * M_PI / 180.f;

    float4x4 rotateZM{
        {static_cast<float>(cos(angleZ)), static_cast<float>(-sin(angleZ)), 0, 0},
        {static_cast<float>(sin(angleZ)), static_cast<float>(cos(angleZ)), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float4x4 translateM{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {moveX, moveY, moveZ, 1}
    };

    auto transformM = mul(translateM, mul(rotateZM, mul(rotateYM, mul(rotateXM, scaleM))));

    std::for_each(std::execution::par, faces.begin(), faces.end(), [&](auto& face) {
        for (auto& vtx : face.vertices)
            // chained assignment with operator `,`
            vtx = (vtx = mul(transformM, vtx), vtx / vtx.w);
        });
}

float4x4 Lighting::MakeCamera(float3 eyeV, float3 atV, float3 upV = { 0.f,1.f,0.f })
{
    // View Matrix https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
    float3 zaxis{ normalize(atV - eyeV) };
    float3 xaxis{ normalize(cross(upV, zaxis)) };
    float3 yaxis{ cross(zaxis, xaxis) };

    float4x4 viewM{
        {xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eyeV)},
        {yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eyeV)},
        {zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eyeV)},
        {0, 0, 0, 1}
    };


    // Projection Matrix https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
    float fovY = 90.f * M_PI / 180.f; // field of view
    float zn = -3; // z near
    float zf = 6; // z far

    float aspect = static_cast<float>(width / height);
    float yScale = 1 / tan(fovY / 2);
    float xScale = yScale / aspect;

    float4x4 projectionM{
        {xScale, 0, 0, 0},
        {0, yScale, 0, 0},
        {0, 0, zf / (zf - zn), -zn * zf / (zf - zn)},
        {0, 0, 1, 0}
    };

    auto finalM = mul(projectionM, viewM);
    return finalM;
}

float4x4 Lighting::MakeLight(float3 eyeV, float3 atV, float3 upV = { 0.f,1.f,0.f })
{
    // View Matrix https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
    float3 zaxis{ normalize(atV - eyeV) };
    float3 xaxis{ normalize(cross(upV, zaxis)) };
    float3 yaxis{ cross(zaxis, xaxis) };

    float4x4 viewM{
        {xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eyeV)},
        {yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eyeV)},
        {zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eyeV)},
        {0, 0, 0, 1}
    };

    auto lightWidth = 1.f;
    auto lightHeight = 1.f;

    // Projection Matrix https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixortholh
    float zn = -3; // z near
    float zf = 6; // z far

    float4x4 projectionM{
        {2 / lightWidth, 0, 0, 0},
        {0, 2 / lightHeight, 0, 0},
        {0, 0, 1 / (zf - zn), -zn / (zf - zn)},
        {0, 0, 0, 1}
    };

    //auto finalM = mul(projectionM, viewM);
    return viewM;
}

void Lighting::DrawScene()
{
    auto result = parser->Parse();
    if (result != 0) throw std::runtime_error("Parsing Failed");

    float screenScale = static_cast<float>(0.5 * std::min(width, height));
    auto faces = parser->GetFaces();

    TransformObject(faces, 0, 0, 1.5f, 0, 0, 0, 1);
    auto camera = MakeCamera({ 0.f, .0f, 0.0f }, { 0.f, 0.f, .5f }, { 0.f, 1.f, 0.f });
    auto light = MakeCamera({ -0.f, 0.f, 0.f }, { 0.f, 0.f, .5f }, {0.f, 1.f, 0.f});

    using namespace linalg::ostream_overloads;

    USHORT wShift = width / 2;
    USHORT hShift = height / 2;

    for (auto& face : faces) {
        DrawFace(face, camera, light, screenScale, float4(wShift, hShift, 0, 0));
    }
}

// triangles are passed in object transform, camera and light are passed to them
void Lighting::DrawFace(
    face objFace,
    const float4x4& camera,
    const float4x4& light,
    const float screenScale,
    const float4 screenShift)
{
    auto face = objFace;
    for (auto& vtx : face.vertices)
        // chained assignment with operator `,`
        vtx = (vtx = mul(camera, vtx), vtx /= vtx.w, vtx * screenScale + screenShift);

    // calculate signed area and cull negative as it indicates backfacing
    const auto areaMul2 = EdgeFunction(face.vertices[0].xy(), face.vertices[1].xy(), face.vertices[2].xy());
    if (areaMul2 < 0) return;

    const float2 edge0 = face.vertices[2].xy() - face.vertices[1].xy();
    const float2 edge1 = face.vertices[0].xy() - face.vertices[2].xy();
    const float2 edge2 = face.vertices[1].xy() - face.vertices[0].xy();

    // determine boundaries
    const int xMin = std::min({ face.vertices[0].x, face.vertices[1].x, face.vertices[2].x });
    const int xMax = std::max({ face.vertices[0].x, face.vertices[1].x, face.vertices[2].x });
    const int yMin = std::min({ face.vertices[0].y, face.vertices[1].y, face.vertices[2].y });
    const int yMax = std::max({ face.vertices[0].y, face.vertices[1].y, face.vertices[2].y });

    std::vector<color> colors{ color(255,0,0), color(0,255,0), color(0,0,255) };

    for (int x = xMin; x <= xMax; ++x)
        for (int y = yMin; y <= yMax; ++y)
        {
            // check screen boundaries
            if (x >= width || y >= height || x < 0 || y < 0) continue;

            // calculate barycentric coefficients
            auto w0 = EdgeFunction(face.vertices[1].xy(), face.vertices[2].xy(), float2(x, y));
            auto w1 = EdgeFunction(face.vertices[2].xy(), face.vertices[0].xy(), float2(x, y));
            auto w2 = EdgeFunction(face.vertices[0].xy(), face.vertices[1].xy(), float2(x, y));

            // overlapping test with top-left rule
            bool overlaps = true;
            overlaps &= (w0 == 0 ? ((edge0.y == 0 && edge0.x > 0) || edge0.y > 0) : (w0 > 0));
            overlaps &= (w1 == 0 ? ((edge1.y == 0 && edge1.x > 0) || edge1.y > 0) : (w1 > 0));
            overlaps &= (w2 == 0 ? ((edge2.y == 0 && edge2.x > 0) || edge2.y > 0) : (w2 > 0));

            // draw pixel if it overlaps the triangle
            if (overlaps)
            {
                // barycentrics
                w0 /= areaMul2;
                w1 /= areaMul2;
                w2 /= areaMul2;

                // depth value interpolated from vertices
                auto z = (w0 * face.vertices[0] + w1 * face.vertices[1] + w2 * face.vertices[2]).z;

                // interpolated texture coordinates
                float2 texel = (w0 * face.texCoords[0] + w1 * face.texCoords[1] + w2 * face.texCoords[2]).xy();
                texel.y = 1.f - texel.y; // FIXME: texture is flipped on the y axis
                USHORT texScaledX = texel.x * textureWidth,
                    texScaledY = texel.y * textureHeight;

                // LIGHT CALCULATION
                color Ka = color(255, 255, 255);  // ambient color
                color Kd = texture[std::clamp(textureWidth * texScaledY + texScaledX, 0, (int)texture.size() - 1)]; // diffuse color
                color Ks = color(255, 255, 255);  // specular color
                const float Ns = 47.f;            // specular exponent
                float3 N = normalize(w0 * face.normals[0] + w1 * face.normals[1] + w2 * face.normals[2]); // surface normal vector
                float3 L = normalize(float3(-0.7, 1, -0.7));   // global light vector
                float3 V = float3(0, 0, -1);                   // view vector
                auto R = 2 * dot(L, N) * N - L;
                float ia = .1f;  // ambient blend factor
                float id = .9f;  // diffuse blend factor
                float is = 1.f;  // specular blend factor

                auto applyPhong = [&](auto Ka, auto Kd, auto Ks, auto Ns) {
                    return std::clamp(Ka * ia // ambient component
                        + Kd * dot(L, N) * id // diffuse component
                        + Ks * pow(std::max(0.f, dot(R, V)), Ns), 0.f, 255.f) * is; // specular component
                };
                color phong = color(applyPhong((float)Kd.r, (float)Kd.r, (float)Ks.r, Ns),
                    applyPhong((float)Kd.g, (float)Kd.g, (float)Ks.g, Ns),
                    applyPhong((float)Kd.b, (float)Kd.b, (float)Ks.b, Ns));

                SetPixelWithZ(x,y,z, phong);
            }
        }
}
