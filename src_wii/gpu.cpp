#include <gpu.hpp>
#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include <algorithm>
#include <ogc/tpl.h>
#define DEFAULT_FIFO_SIZE (256*1024)

GXRModeObj *get_rmode();
namespace {
Mtx GXmodelView2D;
Mtx44 perspective;
GXRModeObj *rmode;
}

extern void *xfbs[2];
extern bool framebuffer;

GPU::GPU() {
    rmode = get_rmode();
    GXColor background = {0, 0, 0, 0xFF};
    void *gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
    memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
    GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
    GX_SetCopyClear(background, 0x00FFFFFF);
    GX_SetViewport(0,0,rmode->fbWidth, rmode->efbHeight,0,1);
    auto yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
    auto xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
    GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
    GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
    GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
    if(rmode->aa)
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xfbs[framebuffer],GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    // setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);


	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);


	GX_InvalidateTexAll();
    
    guOrtho(perspective,0,479,0,639,0,300);
    GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);
}
GPU::~GPU() {}

void GPU::startRender() {
    GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
    GX_InvVtxCache();
	GX_InvalidateTexAll();

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    guMtxIdentity(GXmodelView2D);
    guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
    GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);
}

void GPU::endRender() {
    GX_DrawDone();

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(xfbs[framebuffer],GX_TRUE);
}

VBO *GPU::createVBO(int tris) {
    VBO* b = new VBO;
    VBO &buf = *b;
    buf = {tris, new Triangle[tris], false, nullptr};
    VBOs.push_back(b);
    return b;
}
void GPU::destroyVBO(VBO *vbo) {
    auto it = std::find(VBOs.begin(), VBOs.end(), vbo);
    if(it == VBOs.end())
        return;
    VBOs.erase(it);
    delete[] vbo->tris;
    delete vbo;
}

void VBO::render(int from, int len) {
    GX_Begin(GX_TRIANGLES, GX_VTXFMT0, len*3);
        for(int i = from; i < from + len; i++) {
            GX_Position2f32(tris[i].a.pos.x, tris[i].a.pos.y);
            GX_TexCoord2f32(tris[i].a.texcoords.x, tris[i].a.texcoords.y);
            GX_Position2f32(tris[i].b.pos.x, tris[i].b.pos.y);
            GX_TexCoord2f32(tris[i].b.texcoords.x, tris[i].b.texcoords.y);
            GX_Position2f32(tris[i].c.pos.x, tris[i].c.pos.y);
            GX_TexCoord2f32(tris[i].c.texcoords.x, tris[i].c.texcoords.y);
        }
    GX_End();
}
Triangle VBO::get(int index) const {
    return tris[index];
}
void VBO::set(int index, Triangle item) {
    tris[index] = item;
}
