#pragma once


#include "color_space.h"



class LineDrawing: public ColorSpace
{
public:
	LineDrawing(USHORT width, USHORT height);

	void DrawLine(USHORT x_begin, USHORT y_begin, USHORT x_end, USHORT y_end, color color);

	void DrawScene();

};
