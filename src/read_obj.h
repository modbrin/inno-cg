#pragma once


#include "draw_line.h"


class ObjParser
{
public:
	ObjParser(const std::string &filename);
	virtual ~ObjParser();

	int Parse();

	const std::vector<face>& GetFaces();
private:
	int ObjParser::ParseVertex(std::stringstream&& data);
	int ObjParser::ParseFace(std::stringstream&& data);
protected:
	std::string filename;

	std::vector<float4> vertices;
	std::vector<face> faces;

	std::vector<std::string> Split(const std::string& s, char delimiter);
};


class ReadObj: public LineDrawing
{
public:
	ReadObj(USHORT width, USHORT height, const std::string& obj_file);
	virtual ~ReadObj();

	void DrawScene();

protected:
	std::unique_ptr<ObjParser> parser;
};
