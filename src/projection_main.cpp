#include "projection.h"

int main(int argc, char* argv[])
{
    Projection* render = new Projection(1920, 1080, "models/uv_sphere.obj");
    render->Clear();
    render->DrawScene();
    int result = render->Save("results/projection.png");
    return result;
}