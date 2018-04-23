#include <iostream>
#include <init.hpp>
#include <gpu.hpp>
#include <paths.hpp>
GPU *g;
int main(int argc, const char ** argv) {
    init(argc, argv);
    GPU gpu;
    g = &gpu;

    VBO *vbo = gpu.createVBO(2);
    vbo->set(0, {
        {
            { 0.0f, 1.0f },
            { 0.0f, 1.0f }
        },
        {
            { 0.0f, 0.0f },
            { 0.0f, 0.0f }
        },
        {
            { 1.0f, 0.0f },
            { 1.0f, 0.0f }
        }
    });

    vbo->set(1, {
        {
            { 1.0f, 0.0f },
            { 1.0f, 0.0f }
        },
        {
            { 1.0f, 1.0f },
            { 1.0f, 1.0f }
        },
        {
            { 0.0f, 1.0f },
            { 0.0f, 1.0f }
        }
    });

    void *tex = gpu.load_texture(get_ro_data_dir() + "/imgs/gorb.png");

    
    while(processEvents()) {
        gpu.startRender();
        gpu.use_texture(tex);
        vbo->render(0, 2);
        gpu.endRender();
        swapFB();
    }
    gpu.destroyVBO(vbo);
    gpu.destroy_texture(tex);
    deinit();
    return 0;
}
