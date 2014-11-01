#include <fea/render2d.hpp>
#include <fea/userinterface.hpp>
#include <fea/ui/sdl2windowbackend.hpp>
#include <fea/util.hpp>
#include <chrono>
#include <thread>

#include "../../src/utilities/interpolationgrid3d.hpp"
#include "../../src/utilities/interpolators.hpp"

int main(int argc, char *argv[])
{
    fea::Window window(new fea::SDL2WindowBackend());
    window.create(fea::VideoMode(800,800), "interpolation visual");

    fea::Renderer2D renderer(fea::Viewport({800, 800}, {0,0},fea::Camera({400, 400})));
    renderer.setup();

    fea::Texture canvas;
    canvas.create(800, 800, fea::Color::Black, false, true);

    fea::Quad quad({800.0f, 800.0f});
    quad.setTexture(canvas);
    /////////////////
    const uint32_t downSampling = 5;
    float step = std::pow(2, downSampling);
    InterpolationGrid3D<float> interpolationgrid(800 + step, downSampling);
    interpolationgrid.setInterpolator(Interpolator<float>::trilinear);

    fea::Noise noise;

    uint32_t size = interpolationgrid.getInnerSize();

    glm::vec3 noiseCoordinate;
    for(uint32_t x = 0; x < size; x++)
    {
        noiseCoordinate.y = 0;
        for(uint32_t y = 0; y < size; y++)
        {
            noiseCoordinate.z = 0;
            for(uint32_t z = 0; z < size; z++)
            {
                float noiseValue = (noise.simplexOctave2D(noiseCoordinate.x / 800.0f, noiseCoordinate.z / 800.0f) + 1.0f) / 2.0f;
                interpolationgrid.setInner({x, y, z}, noiseValue);
                noiseCoordinate.z += step;
            }
            noiseCoordinate.y += step;
        }
        noiseCoordinate.x += step;
    }

    for(uint32_t x = 0; x < 800; x++)
    {
        for(uint32_t z = 0; z < 800; z++)
        {
            uint8_t color = (uint8_t)(interpolationgrid.get({x, 50u, z}) * 256.0f);
            canvas.setPixel(x, z, fea::Color(color, color, color));
        }
    }

    canvas.update();
    /////////////////
    renderer.clear();
    renderer.queue(quad);
    renderer.render();

    window.swapBuffers();

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}
