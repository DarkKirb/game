#include <gpu.hpp>
#include <GL/glew.h>
#include <algorithm>
#include <png.hpp>
#include <iostream>
namespace {
    const char* vertex_shader = R"(
#version 120
attribute vec2 coord2d;
attribute vec2 texcoord;
varying vec2 f_texcoord;
void main() {
    gl_Position = vec4(coord2d, 0.0, 1.0);
    f_texcoord = texcoord;
}
        )";
    const char* fragment_shader = R"(
#version 120
varying vec2 f_texcoord;
uniform sampler2D tex;
void main() {
    gl_FragColor = texture2D(tex, f_texcoord);
}
    )";
    GLuint vshader, fshader, program, coord2d, texcoord, tex;
}

GPU::GPU() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *) {
            std::cout << "source: " << source << ", type: " << type << ", id: " << id << ", severity: " << severity << ", length: " << length << ", message: " << message << std::endl;
            }, nullptr);
    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex_shader, nullptr);
    glCompileShader(vshader);

    GLint compile_ok;
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
        std::cout << "Error in vertex shader" << std::endl;
        GLint log_length = 0;
        glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &log_length);
        char *log = new char[log_length];
        glGetShaderInfoLog(vshader, log_length, NULL, log);
        std::cout << log << std::endl;
        delete[] log;
        throw nullptr;
	}

    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragment_shader, nullptr);
    glCompileShader(fshader);


    glGetShaderiv(fshader, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
        std::cout << "Error in fragment shader" << std::endl;
        GLint log_length = 0;
        glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &log_length);
        char *log = new char[log_length];
        glGetShaderInfoLog(fshader, log_length, NULL, log);
        std::cout << log << std::endl;
        delete[] log;
        throw nullptr;
	}

    program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

    coord2d = glGetAttribLocation(program, "coord2d");
    texcoord = glGetAttribLocation(program, "texcoord");
    tex = glGetUniformLocation(program, "tex");
}

GPU::~GPU() {
    glDeleteProgram(program);
    for(auto &vbo : VBOs) {
        auto n = (GLuint*) vbo->native;
        glDeleteBuffers(1, n);
        delete n;
        delete[] vbo->tris;
        delete vbo;
    }
}

void GPU::startRender() {
    for(auto &vbo : VBOs) {
        if(vbo->changed) {
            glBindBuffer(GL_ARRAY_BUFFER, *(GLuint*)vbo->native);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle)*vbo->triangle_count, vbo->tris, GL_STATIC_DRAW);
            vbo->changed=false;
        }
    }
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glEnableVertexAttribArray(coord2d);
    glEnableVertexAttribArray(texcoord);
    glVertexAttribPointer(
		coord2d,             // attribute
		2,                   // number of elements per vertex, here (x,y)
		GL_FLOAT,            // the type of each element
		GL_FALSE,            // take our values as-is
		4 * sizeof(GLfloat), // extra data between each position
		0                    // VBO
    );
    glVertexAttribPointer(
        texcoord,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(GLfloat),
        (GLvoid*) (2 * sizeof(GLfloat))
    );
}

void GPU::endRender() {
    glDisableVertexAttribArray(coord2d);
    glDisableVertexAttribArray(texcoord);
}

VBO *GPU::createVBO(int tris) {
    VBO *b = new VBO;
    VBO &buf = *b;
    buf = {tris, new Triangle[tris], true, (void*)new GLuint};
    glCreateBuffers(1, (GLuint*)buf.native);
    VBOs.push_back(b);
    return b;
}
void GPU::destroyVBO(VBO *vbo) {
    auto entry = std::find(VBOs.begin(), VBOs.end(), vbo);
    if(entry == VBOs.end())
        return;
    VBOs.erase(entry);
    delete[] vbo->tris;
    glDeleteBuffers(1, (GLuint*) vbo->native);
    delete (GLuint*)vbo->native;
    delete vbo;
}

void VBO::render(int from, int len) {
    glBindBuffer(GL_ARRAY_BUFFER, *(GLuint*)native);
    glDrawArrays(GL_TRIANGLES, from * 3, len * 3);
}
Triangle VBO::get(int index) const {
    return tris[index];
}
void VBO::set(int index, Triangle item) {
    tris[index] = item;
    changed = true;
}

void *GPU::load_texture(std::string tex) {
    PNG png(tex);
    char *raw_png = new char[png.height * png.width * 4];
    png.read_to(raw_png);
    GLuint *tex_id = new GLuint;
    glGenTextures(1, tex_id);
    glBindTexture(GL_TEXTURE_2D, *tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA,
            png.width,
            png.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            raw_png
            );
    delete[] raw_png;
    return tex_id;
}

void GPU::destroy_texture(void *p) {
    GLuint *tex_id = (GLuint*)p;
    glDeleteTextures(1, tex_id);
    delete tex_id;
}

void GPU::use_texture(void *p) {
    GLuint *tex_id = (GLuint*)p;
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(tex, 0);
    glBindTexture(GL_TEXTURE_2D, *tex_id);
}
