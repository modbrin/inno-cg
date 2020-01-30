#include "color_space.h"


ColorSpace::ColorSpace(unsigned short width, unsigned short height) : BlackImage(width, height)
{}

void ColorSpace::DrawScene()
{
    for (USHORT x = 0; x < width; ++x)
        for (USHORT y = 0; y < height; ++y)
            SetPixel(x, y, color(
                static_cast<UCHAR>(255 * static_cast<float>(y) / height),
                static_cast<UCHAR>(255 * static_cast<float>(x) / width),
                static_cast<UCHAR>(0)
            ));

}

void ColorSpace::SetPixel(USHORT x, USHORT y, color color)
{
    frame_buffer[y * width + x] = color;
}

