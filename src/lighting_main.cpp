#include "lighting.h"

int main(int argc, char* argv[])
{
    Lighting* render = new Lighting(1920, 1080, "models/uv_sphere.obj");
    int texLoaded = render->LoadTexture("models/texel_density_2048.png");
    if (texLoaded != 0) return 1;
    render->Clear();
    render->DrawScene();
    int result = render->Save("results/lighting.png");
    system("start results/lighting.png");
    return result;
}