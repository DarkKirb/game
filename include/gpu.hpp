#pragma once
#include <vec.hpp>
#include <vector>
#include <string>

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
    void setQuad(int i, Vec2 tl, Vec2 br, int spriteno, int width) {
        Vec2 spritetl{(spriteno % width)*(1.0f/width), (spriteno / width)*(1.0f/width)};
        Vec2 spritebr = spritetl + (1.0f/width);
        set(i, {
                {
                    tl,
                    spritetl
                },
                {
                    {br.x, tl.y},
                    {spritebr.x, spritetl.y}
                },
                {
                    {tl.x, br.y},
                    {spritetl.x, spritebr.y}
                }
               });
        set(i+1, {
                    {
                        {br.x, tl.y},
                        {spritebr.x, spritetl.y}
                    },
                    {
                        br,
                        spritebr
                    },
                    {
                        {tl.x, br.y},
                        {spritetl.x, spritebr.y}
                    }
                }
            );
    }
};

struct GPU {
    std::vector<VBO*> VBOs;
    GPU();
    ~GPU();
    void startRender();
    void endRender();
    VBO *createVBO(int tris);
    void destroyVBO(VBO *vbo);
    void *load_texture(std::string tex);
    void destroy_texture(void *);
    void use_texture(void *);
};
