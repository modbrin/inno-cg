#include "read_obj.h"

int main(int argc, char* argv[])
{
     ReadObj* render = new ReadObj(1920, 1080, "models/uv_sphere.obj");
    render->Clear();
    render->DrawScene();
    int result = render->Save("results/read_obj.png");
    return result;
}