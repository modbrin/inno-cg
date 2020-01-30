#include "black_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <algorithm>
#include <execution>


BlackImage::BlackImage(USHORT width,
                       USHORT height,
                       color default_fill_color)
    :
    width(width),
    height(height),
    default_fill_color(default_fill_color)
{
    // arithmetic overflow is secured by users knownledge of resolution
    frame_buffer.resize(width * height);
}

void BlackImage::Clear()
{
    std::fill(frame_buffer.begin(), frame_buffer.end(), default_fill_color);
    // `vector::resize` will not delete previous values, only add
    // default for ones that are added due to size increase
}

int BlackImage::Save(const std::string &filename) const
{
    auto result = stbi_write_png(filename.c_str(), width, height, 3, frame_buffer.data(), width * sizeof(color));

    return (result - 1); //STB returns 1 if everything is ok, convering to OS return code
}

std::vector<color> BlackImage::GetFrameBuffer() const
{
    return frame_buffer;
}
