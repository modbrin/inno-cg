#pragma once


#include "read_obj.h"


class Projection : public ReadObj
{
public:
    Projection(USHORT width, USHORT height, const std::string& obj_file);

    void ProjectScene(std::vector<face>& faces);

    void DrawScene();

};
