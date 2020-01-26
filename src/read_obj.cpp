#include "read_obj.h"


#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <array>


ObjParser::ObjParser(const std::string& filename) :
    filename(filename)
{}


ObjParser::~ObjParser()
{}

int ObjParser::ParseVertex(std::stringstream&& data)
{
    float4 newVec;
    unsigned char i = 0;
    float point;
    // TODO: consider case when middle value is not parsed and rest is parsed
    // correctly leading to corrupt data => return error in that case
    while (data >> point)
        newVec[i++] = point;
    if (i == 3)
        newVec[i] = 1.f; // default `w` to 1.0
    else if (i < 3 || i > 4)
        return 1;
    vertices.push_back(newVec);
    return 0;
}

int ObjParser::ParseFace(std::stringstream&& data)
{
    face newFace;
    std::string triplet;
    int i = 0;
    while(data >> triplet)
    {
        int idx; std::stringstream ss_triplet(triplet);
        ss_triplet >> idx;
        // TODO: parse optional vt
        // TODO: parse optional vn
        
        float4 vertex;
        if (idx < 0)
            vertex = vertices[vertices.size() + idx];
        else if (idx > 0)
            vertex = vertices[idx - 1];
        // TODO: make better triangulation, current is not visually optimal
        // and only works for convex objects
        if (i >= 3)
        {
            newFace.vertices[1] = newFace.vertices[2];
            newFace.vertices[2] = vertex;
        }
        else newFace.vertices[i++] = vertex;
        if (i >= 3)
            faces.push_back(newFace);
    }
    if (i < 3) return 1; // not a single triangle read
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

ReadObj::~ReadObj()
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
