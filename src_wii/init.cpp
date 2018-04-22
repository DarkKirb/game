#include <init.hpp>

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include <fat.h>
#include <iostream>

static void *xfb = nullptr;
static GXRModeObj *rmode = nullptr;

void init(int argc, const char** argv) {
    VIDEO_Init();
    WPAD_Init();
    PAD_Init();
    rmode = VIDEO_GetPreferredMode(nullptr);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth*VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

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
   return true;
}
void swapFB() {
    VIDEO_WaitVSync();
}
void deinit() {
}
