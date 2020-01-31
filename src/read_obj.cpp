#include "read_obj.h"
#include "read_obj.h"
#include "read_obj.h"


#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <array>


ObjParser::ObjParser(const std::string& filename) :
    filename(filename)
{}

int ObjParser::ParseVertex(std::stringstream&& data)
{
    float4 newVec;
    unsigned char i = 0;
    float point;

    while (data >> point)
        if (i <= 4) newVec[i++] = point;
        else break;
    if (i == 3)
        newVec[i] = 1.f; // w is optional, default to 1
    else if (i < 3)
        return 1;
    vertices.push_back(newVec);
    return 0;
}

int ObjParser::ParseTexCoord(std::stringstream&& data)
{
    float3 newCoord;
    unsigned char i = 0;
    float point;
    while (data >> point)
        if (i <= 3) newCoord[i++] = point; // fill u v w, ignore redundant values
        else break;
    if (i == 0)
        return 1;
    if (i == 1)
        newCoord[i++] = 0.f; // v is optional, default is 0
    if (i == 2)
        newCoord[i++] = 0.f; // w is optional, default is 0
    texCoords.push_back(newCoord);
    return 0;
}

int ObjParser::ParseNormal(std::stringstream&& data)
{
    float3 normalVec;
    unsigned char i = 0;
    float point;
    while (data >> point)
        if (i <= 3) normalVec[i++] = point;
        else break;
    if (i < 3) return 1;
    normals.push_back(normalVec);
    return 0;
}

int ObjParser::ParseFace(std::stringstream&& data)
{
    face newFace;
    newFace.texCoordsPresent = true;
    newFace.normalsPresent = true;
    std::string triplet;
    int i = 0;
    while(data >> triplet)
    {
        int vertexIdx, texCoordIdx = 0, normalIdx = 0;
        bool texCoordPresent = true, normalPresent = true;
        std::stringstream ss_triplet(triplet);

        if (ss_triplet >> vertexIdx) {
            if (ss_triplet.peek() == '/') { // ok, waiting next number
                ss_triplet.ignore();
                if (!(ss_triplet >> texCoordIdx)) texCoordPresent = false;
                if (ss_triplet.peek() == '/') {
                    ss_triplet.ignore();
                    if (!(ss_triplet >> normalIdx)) normalPresent = false;
                }
                else {
                    normalPresent = false;
                }
            }
            else {
                texCoordPresent = false;
                normalPresent = false;
            }
        }
        else return 1; // vertex coords are mandatory
        
        if (vertexIdx == 0 || (texCoordPresent && texCoordIdx == 0) || (normalPresent && normalIdx == 0)) return 1;
        float4 vertex;
        vertex = (vertexIdx < 0) ? vertices[vertices.size() + vertexIdx] : vertices[vertexIdx - 1];

        float3 texCoord;
        if (texCoordPresent) {
            texCoord = (texCoordIdx < 0) ? texCoords[texCoords.size() + texCoordIdx] : texCoords[texCoordIdx - 1];
            newFace.texCoordsPresent &= texCoordPresent;
        }

        float3 normal;
        if (normalPresent) {
            normal = (normalIdx < 0) ? normals[normals.size() + normalIdx] : normals[normalIdx - 1];
            newFace.normalsPresent &= normalPresent;
        }

        // TODO: make better triangulation, current is not visually optimal
        // and only works for convex objects
        if (i >= 3)
        {
            newFace.vertices[1] = newFace.vertices[2];
            newFace.vertices[2] = vertex;
            if (texCoordPresent) {
                newFace.texCoords[1] = newFace.texCoords[2];
                newFace.texCoords[2] = texCoord;
            }
            if (normalPresent) {
                newFace.normals[1] = newFace.normals[2];
                newFace.normals[2] = normal;
            }
        }
        else
        {
            newFace.vertices[i] = vertex;
            if (texCoordPresent) newFace.texCoords[i] = texCoord;
            if (normalPresent) newFace.normals[i] = normal;
            ++i;
        }
        if (i >= 3)
            faces.push_back(newFace);
    }
    if (i < 3) return 1; // at least three points required for face
    return 0;
}

int ObjParser::Parse()
{
    std::ifstream infile(filename, std::ifstream::in);
    std::string line, prefix;

    while (getline(infile, line))
    {
        std::stringstream ss{line};
        ss >> prefix;
        if (prefix == "v")
            ParseVertex(std::move(ss));
        else if (prefix == "vt")
            ParseTexCoord(std::move(ss));
        else if (prefix == "vn")
            ParseNormal(std::move(ss));
        else if (prefix == "f")
            ParseFace(std::move(ss));
    }
    return 0;
}

const std::vector<face>& ObjParser::GetFaces()
{
    return faces;
}

std::vector<std::string> ObjParser::Split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}


ReadObj::ReadObj(USHORT width, USHORT height, const std::string& obj_file)
    :
    LineDrawing(width, height),
    parser(new ObjParser(obj_file))
{}

void ReadObj::DrawScene()
{
    auto result = parser->Parse();
    if (result != 0) throw std::runtime_error("Parsing Failed");

    int scale = std::min(width, height);

    std::array<color, 3> colors{ color(255,0,0), color(0,255,0), color(0,0,255) };
    
    using namespace linalg::ostream_overloads;

    for (auto& face : parser->GetFaces())
        for (int i = 0; i < FACE_VERTEX_COUNT; ++i)
        {
            DrawLine(
                0.9 * scale * face.vertices[i].x + width / 2,
                0.9 * scale * face.vertices[i].y + height / 2,
                0.9 * scale * face.vertices[(i + 1) % FACE_VERTEX_COUNT].x + width / 2,
                0.9 * scale * face.vertices[(i + 1) % FACE_VERTEX_COUNT].y + height / 2,
                colors[i % 3]);
        }
}
