#include <gpu.hpp>
#include <3ds.h>
#include <citro3d.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <png.hpp>

#define CLEAR_COLOR 0x68B0D8FF
#define DISPLAY_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
     GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define TEXTURE_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
     GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))
namespace {
    constexpr int tri_max = 1024;
    static_assert(tri_max % 8 == 0, "Triangle maximum has to be a multiple of 8");

    uint8_t bitmap[tri_max/8];
    Triangle *vbo_buf;
    C3D_RenderTarget *target;
    uint8_t *shader;
    DVLB_s* vshader_dvlb;
    shaderProgram_s program;
    C3D_Mtx projection;
}


GPU::GPU() {

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C3D_CullFace(GPU_CULL_NONE);
    target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetClear(target, C3D_CLEAR_ALL, 0x000000FF, 0);
    C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
                GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
                GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO)));

    Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, 0.0, 1.0, true);
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, 0, 0);
    C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

    std::ifstream f("romfs:/shaders/vshader.v.pica", std::ios::ate | std::ios::binary);
    size_t size = f.tellg();
    shader = new uint8_t[size];
    f.seekg(0);
    f.read((char*)shader, size);
    vshader_dvlb = DVLB_ParseFile((u32*)shader, size);
    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 4); // pos

    vbo_buf = (Triangle*)linearAlloc(sizeof(Triangle) * tri_max);
    memset(vbo_buf, 0, sizeof(Triangle) * tri_max);
    GSPGPU_FlushDataCache(vbo_buf, sizeof(Triangle) * tri_max);

    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vbo_buf, sizeof(Point), 1, 0);

    memset(bitmap, 0xFF, sizeof(bitmap));
}

GPU::~GPU() {
    linearFree(vbo_buf);
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
    delete[] shader;
    for(auto &vbo : VBOs) {
        delete vbo;
    }
}

void GPU::startRender() {
    for(auto &vbo : VBOs) {
        if(vbo->changed) {
            GSPGPU_FlushDataCache(vbo->tris, sizeof(Triangle) * vbo->triangle_count);
            vbo->changed = false;
        }
    }
    C3D_BindProgram(&program);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(target);
}

void GPU::endRender() {
    C3D_FrameEnd(0);
}

void mark_used(int page) {
    int bit = page & 7;
    int index = page / 8;
    bitmap[index] &= ~((uint8_t)(1<<bit));
}

void mark_used(int page, int len) {
    for(int i=page; i < page+len; i++)
        mark_used(i);
}

void mark_free(int page) {
    int bit = page & 7;
    int index = page / 8;
    bitmap[index] |= (uint8_t)(1<<bit);
}

void mark_free(int page, int len) {
    for(int i=page; i < page+len; i++)
        mark_free(i);
}

bool is_free(int page) {
    int bit = page & 7;
    int index = page / 8;
    return (bitmap[index] & (1<<bit)) != 0;
}

void defrag(std::vector<VBO *> &VBOs) {
    std::vector<Triangle *> tri_temp;
    for(auto &vbo : VBOs) {
        Triangle *temp = new Triangle[vbo->triangle_count];
        memcpy(temp, vbo->tris, vbo->triangle_count * sizeof(Triangle));
    }
    memset(bitmap, 0xFF, sizeof(bitmap));
    int idx = 0;
    int vbo_off = 0;
    for(auto &vbo : VBOs) {
        memcpy(vbo_buf + vbo_off, tri_temp[idx], vbo->triangle_count * sizeof(Triangle));
        delete[] tri_temp[idx++];
        mark_used(vbo_off, vbo->triangle_count);
        vbo->changed=true;
    }
}

int allocate() {
    for(int i = 0; i < tri_max; i++) {
        if(!is_free(i))
            continue;
        mark_used(i);
        return i;
    }
    std::cerr << "Not enough space in VBO available" << std::endl;
    abort();
}

int allocate(int count, std::vector<VBO*> &VBOs, bool initial=true) {
    for(int i = 0; i <= tri_max - count; i++) {
        if(!is_free(i + count)) {
            i += count - 1;
            continue;
        }
        if(!is_free(i))
            continue;
        bool found = true;
        for(int j = 0; j < count; j++) {
            if(!is_free(i + j)) {
                i += j - 1;
                found=false;
                break;
            }
        }
        if(!found)
            continue;
        mark_used(i, count);
        return i;
    }
    if(initial) {
        defrag(VBOs);
        return allocate(count, VBOs, false);
    }
    std::cerr << "Could not allocate space in VBO" << std::endl;
    abort();
}

VBO *GPU::createVBO(int tris) {
    VBO *b = new VBO;
    VBO &buf = *b;
    int page = allocate(tris, VBOs);
    buf = {tris, vbo_buf + page, true, nullptr};
    VBOs.push_back(b);
    return b;
}

void GPU::destroyVBO(VBO *vbo) {
    auto pos = std::find(VBOs.begin(), VBOs.end(), vbo);
    if(pos == VBOs.end())
        return;
    VBOs.erase(pos);
    int page = std::distance(vbo_buf, vbo->tris);
    mark_free(page, vbo->triangle_count);
    delete vbo;
}

void VBO::render(int from, int len) {
    for(int i = from; i < from + len; i++) {
        Triangle &item = tris[i];
        std::cout << "Rendering " << i << " of VBO." << std::endl;
        std::cout << "Point A: (" << item.a.pos.x << ", " << item.a.pos.y << "), (" << item.a.texcoords.x << ", " << item.a.texcoords.y << ")" << std::endl;
        std::cout << "Point B: (" << item.b.pos.x << ", " << item.b.pos.y << "), (" << item.b.texcoords.x << ", " << item.b.texcoords.y << ")" << std::endl;
        std::cout << "Point C: (" << item.c.pos.x << ", " << item.c.pos.y << "), (" << item.c.texcoords.x << ", " << item.c.texcoords.y << ")" << std::endl;
    }
    int page = std::distance(vbo_buf, tris);
    C3D_DrawArrays(GPU_TRIANGLES, (page+from) * 3, len * 3);
}

Triangle VBO::get(int index) const {
    return tris[index];
}

void VBO::set(int index, Triangle item) {
    std::cout << "Setting " << index << " of VBO." << std::endl;
    std::cout << "Point A: (" << item.a.pos.x << ", " << item.a.pos.y << "), (" << item.a.texcoords.x << ", " << item.a.texcoords.y << ")" << std::endl;
    std::cout << "Point B: (" << item.b.pos.x << ", " << item.b.pos.y << "), (" << item.b.texcoords.x << ", " << item.b.texcoords.y << ")" << std::endl;
    std::cout << "Point C: (" << item.c.pos.x << ", " << item.c.pos.y << "), (" << item.c.texcoords.x << ", " << item.c.texcoords.y << ")" << std::endl;
    memcpy(tris + index, &item, sizeof(item));
    changed = true;
}

void *GPU::load_texture(std::string tex) {
    PNG png(tex);
    char *raw_png = new char[png.height * png.width * 4];
    png.read_to(raw_png);
    char *gpusrc = (char *)linearAlloc(png.height * png.width * 4);
    memcpy(gpusrc, raw_png, png.height * png.width * 4);
    GSPGPU_FlushDataCache(gpusrc, png.width*png.height*4);
    C3D_Tex *texp = new C3D_Tex;
    C3D_TexInit(texp, png.width, png.height, GPU_RGBA8);
    C3D_SafeDisplayTransfer((u32*)gpusrc, GX_BUFFER_DIM(png.width, png.height), (u32*)texp->data, GX_BUFFER_DIM(png.width, png.height), TEXTURE_TRANSFER_FLAGS);
    gspWaitForPPF();
    C3D_TexSetFilter(texp, GPU_LINEAR, GPU_NEAREST);
    delete[] raw_png;
    linearFree(gpusrc);
    return texp;
}
void GPU::destroy_texture(void *p) {
    C3D_TexDelete((C3D_Tex*)p);
    delete (C3D_Tex*)p;
}
void GPU::use_texture(void *p) {
    C3D_TexBind(0, (C3D_Tex*)p);
}
