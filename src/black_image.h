#pragma once

#include <string>
#include <vector>

#include "structs.h"


class BlackImage
{
protected:
    using USHORT = unsigned short;
    using UCHAR = unsigned char;
public:
    BlackImage(USHORT width, USHORT height, color default_fill_color = color(0, 0, 0));

    void Clear();
    int Save(const std::string& filename) const;

    std::vector<color> GetFrameBuffer() const; 
protected:
    unsigned short height;
    unsigned short width;

    std::vector<color> frame_buffer;
    color default_fill_color;
};
