#pragma once
#include <vec.hpp>
#include <vector>

struct Point {
    Vec2 pos;
    Vec2 texcoords;
}__attribute__((packed));

struct Triangle {
    Point a, b, c;
}__attribute__((packed));

struct VBO {
    int triangle_count;
    Triangle *tris;
    void render(int from, int len);
    bool changed;
    Triangle get(int index) const;
    void set(int index, Triangle item);
    void *native;
};

struct GPU {
    std::vector<VBO*> VBOs;
    GPU();
    ~GPU();
    void startRender();
    void endRender();
    VBO *createVBO(int tris);
    void destroyVBO(VBO *vbo);
};
