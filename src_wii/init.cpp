#include <init.hpp>

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include <fat.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <ogc/tpl.h>

#define DEFAULT_FIFO_SIZE (256*1024)
namespace {
void *xfbs[2];
GXRModeObj *rmode = nullptr;
bool framebuffer;
Mtx GXmodelView2D;
}

void init(int argc, const char** argv) {
    VIDEO_Init();
    WPAD_Init();
    PAD_Init();
    rmode = VIDEO_GetPreferredMode(nullptr);
    xfbs[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfbs[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfbs[framebuffer]);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
    framebuffer++;
    GXColor background = {0, 0, 0, 0xff};
    void *gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
    memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);

    GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);

    GX_SetCopyClear(background, 0x00FFFFFF);
    GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
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

    if(!fatInitDefault()) {
        std::cerr << "fatInitDefault failure: terminating" << std::endl;
        abort();
    }
}
bool processEvents() {
    WPAD_ScanPads();
    PAD_ScanPads();
    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) return false;
    if(WPAD_ButtonsDown(1) & WPAD_BUTTON_HOME) return false;
    if(WPAD_ButtonsDown(2) & WPAD_BUTTON_HOME) return false;
    if(WPAD_ButtonsDown(3) & WPAD_BUTTON_HOME) return false;
    if(PAD_ButtonsDown(0) & PAD_BUTTON_START) return false;
    if(PAD_ButtonsDown(1) & PAD_BUTTON_START) return false;
    if(PAD_ButtonsDown(2) & PAD_BUTTON_START) return false;
    if(PAD_ButtonsDown(3) & PAD_BUTTON_START) return false;

    GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
    GX_InvVtxCache();
	GX_InvalidateTexAll();

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    guMtxIdentity(GXmodelView2D);
    guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
    GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);

    return true;
}
void swapFB() {
    GX_DrawDone();
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetAlphaUpdate(GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(xfbs[framebuffer],GX_TRUE);
    VIDEO_SetNextFramebuffer(xfbs[framebuffer]);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    framebuffer++;
}
void deinit() {
}
