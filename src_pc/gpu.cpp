#include <gpu.hpp>
#include <GL/glew.h>

namespace {
    const char* vertex_shader = R"(
#version 120
attribute vec2 coord2d;
attribute vec2 texcoord;
varying vec2 f_texcoord;
void main() {
    gl_Position = coord2d;
    f_texcoord = texcoord;
}
        )";
    const char* fragment_shader = R"(
#version 120
varying vec2 f_texcoord;
uniform Sampler2D tex;
void main() {
    gl_FragColor = texture(tex, f_texcoord);
}
    )";
}

GPU::GPU() {

}
