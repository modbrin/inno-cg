#include "z_buffer_culling.h"

int main(int argc, char* argv[])
{
    ZCulling* render = new ZCulling(1920, 1080, "models/intersect.obj");
    render->Clear();
    render->DrawScene();
    int result = render->Save("results/z_buffer_culling.png");
    return result;
}