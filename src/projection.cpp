#include "projection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>


Projection::Projection(USHORT width, USHORT height, const std::string& obj_file)
    :
    ReadObj(width, height, obj_file)
{}

Projection::~Projection()
{}

void Projection::DrawScene()
{
    
}
