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
#include <debug.h>

#define DEFAULT_FIFO_SIZE (256*1024)
namespace {
GXRModeObj *rmode = nullptr;
bool first = true;
}
void *xfbs[2];
bool framebuffer;
GXRModeObj *get_rmode() {
    return rmode;
}
void init(int argc, const char** argv) {
    DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
    _break();
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
    framebuffer = !framebuffer;

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
    VIDEO_SetNextFramebuffer(xfbs[framebuffer]);
    if(__builtin_expect(first, 0)) {
        VIDEO_SetBlack(false);
        first = false;
    }
    VIDEO_Flush();
    VIDEO_WaitVSync();
    framebuffer = !framebuffer;
}
void deinit() {
}
