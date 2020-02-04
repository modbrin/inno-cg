#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

//#include "utils.h"

#include "lighting.h"


TEST_CASE("Triangle rasterization test") {
    Lighting* image = new Lighting(1920, 1080, "models/uv_sphere.obj");
    image->Clear();
    int texLoaded = image->LoadTexture("models/texel_density_2048.png");

    BENCHMARK("Draw scene")
    {
        image->DrawScene();
    };

    REQUIRE(validate_framebuffer("references/lighting.png", image->GetFrameBuffer()));
}