#include <init.hpp>
#include <iostream>

#include <3ds.h>
#include <citro3d.h>
#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))
namespace {
C3D_RenderTarget *target;
}

void init(int argc, const char** argv) {
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, nullptr);
    if(romfsInit() < 0) {
        std::cerr << "Could not open the romfs!" << std::endl;
        throw nullptr;
    }
    if(sdmcInit() < 0) {
        std::cerr << "Could not open the sdmc!" << std::endl;
        throw nullptr;
    }
    
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
    C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
}
bool processEvents() {
    if(!aptMainLoop())
        return false;
    hidScanInput();
    if(hidKeysDown() & KEY_START)
        return false;
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(target);
    return true;
}
void swapFB() {
    C3D_FrameEnd(0);
}
void deinit() {
    gfxExit();
}
