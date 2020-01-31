#include "lighting.h"

int main(int argc, char* argv[])
{
    Lighting* render = new Lighting(1920, 1080, "models/head.obj");
    int texLoaded = render->LoadTexture("models/lambertian.jpg");
    if (texLoaded != 0) return 1;
    render->Clear();
    render->DrawScene();
    int result = render->Save("results/lighting.png");
    return result;
}