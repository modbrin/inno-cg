#include "draw_line.h"


#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>


LineDrawing::LineDrawing(USHORT width, USHORT height) : ColorSpace(width, height)
{}


void LineDrawing::DrawLine(USHORT x_begin, USHORT y_begin, USHORT x_end, USHORT y_end, color color)
{
    // NAIVE IMPLEMENTATION
    /*auto dy = (static_cast<float>(y_end) - y_begin);
    auto dx = (static_cast<float>(x_end) - x_begin);
    auto slope = dy / dx;
    auto intercept = y_begin - slope * x_begin;
    for (int x = x_begin; x <= x_end; ++x)
        SetPixel(x, slope*x + intercept, color);*/

	// BRESENHAM'S LINE ALGORITHM
	bool steep = fabs(y_end - y_begin) > fabs(x_end - x_begin);

	// convert steep curve into gradual
	if (steep) {
		std::swap(x_begin, y_begin);
		std::swap(x_end, y_end);
	}

	// if direction is not left-to-right then flip
	if (x_begin > x_end) {
		std::swap(x_begin, x_end);
		std::swap(y_begin, y_end);
	}

	// get deltas
	float dx = static_cast<float>(x_end - x_begin);
	float dy = static_cast<float>(y_end - y_begin);

	// initial values
	USHORT y = y_begin;
	float error = dx / 2;

	for (USHORT x = x_begin; x <= x_end; ++x) {

		// translate coordinated for coverted slope
		if (steep) {
			if (x >= height || y >= width || x < 0 || y < 0) continue;
			SetPixel(y, x, color);
		}
		else {
			if (x >= width || y >= height || x < 0 || y < 0) continue;
			SetPixel(x, y, color);
		}

		// decrease error
		error -= fabs(dy);

		if (error < 0) {
			error += dx;
			// make single-pixel move up or down based on direction
			if (dy > 0) ++y;
			else --y;
		}
	}
}

void LineDrawing::DrawScene()
{
    short x_center = width / 2;
    short y_center = height / 2;
    short radius = std::min(x_center, y_center) - 40;

	// perform full revolution over polar coordinates with step of 5 degrees
	for (float angle = 0.f; angle < 360.f; angle += 5.0) {
		DrawLine(
			x_center,
			y_center,
			x_center + radius * cos(angle * M_PI / 180.f),
			y_center + radius * sin(angle * M_PI / 180.f),
			color(
				static_cast<UCHAR>(255.f * sin(angle * M_PI / 180.f)),
				static_cast<UCHAR>(255.f * cos(angle * M_PI / 180.f)),
				static_cast<UCHAR>(255.f)
			)
		);
	}
}

