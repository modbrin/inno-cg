#include "lighting.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Lighting::Lighting(USHORT width, USHORT height, const std::string& obj_file)
    : ZCulling(width, height, obj_file)
{

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

void Lighting::DrawScene()
{
    auto result = parser->Parse();
    if (result != 0) throw std::runtime_error("Parsing Failed");

    float scale = static_cast<float>(0.5 * std::min(width, height));

    auto faces = parser->GetFaces();

    ProjectScene(faces);

    using namespace linalg::ostream_overloads;

    USHORT wShift = width / 2;
    USHORT hShift = height / 2;

    for (auto& face : faces) {
        face.vertices[0] = face.vertices[0] * scale + float4(wShift, hShift, 0, 1);
        face.vertices[1] = face.vertices[1] * scale + float4(wShift, hShift, 0, 1);
        face.vertices[2] = face.vertices[2] * scale + float4(wShift, hShift, 0, 1);
        DrawFace(face);
    }
}

void Lighting::DrawFace(face face)
{
    // calculate signed area and cull negative as it indicates backfacing
    auto areaMul2 = EdgeFunction(face.vertices[0].xy(), face.vertices[1].xy(), face.vertices[2].xy());
    if (areaMul2 < 0) return;

    float2 edge0 = face.vertices[2].xy() - face.vertices[1].xy();
    float2 edge1 = face.vertices[0].xy() - face.vertices[2].xy();
    float2 edge2 = face.vertices[1].xy() - face.vertices[0].xy();

    // determine boundaries
    int xMin = std::min({ face.vertices[0].x, face.vertices[1].x, face.vertices[2].x });
    int xMax = std::max({ face.vertices[0].x, face.vertices[1].x, face.vertices[2].x });
    int yMin = std::min({ face.vertices[0].y, face.vertices[1].y, face.vertices[2].y });
    int yMax = std::max({ face.vertices[0].y, face.vertices[1].y, face.vertices[2].y });

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
            overlaps &= (w1 == 0 ? ((edge2.y == 0 && edge2.x > 0) || edge2.y > 0) : (w2 > 0));

            // draw pixel if it overlaps the triangle
            if (overlaps)
            {
                w0 /= areaMul2;
                w1 /= areaMul2;
                w2 /= areaMul2;
                auto z = (w0 * face.vertices[0] + w1 * face.vertices[1] + w2 * face.vertices[2]).z;
                float2 texel = (w0 * face.texCoords[0] + w1 * face.texCoords[1] + w2 * face.texCoords[2]).xy();
                USHORT texScaledX = texel.x * textureWidth, texScaledY = texel.y * textureHeight;
                SetPixelWithZ(x, y, z, texture[textureWidth * texScaledY + texScaledX]);
            }
        }
}


