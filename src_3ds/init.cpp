#include <init.hpp>

#include <3ds.h>

void init() {
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);
}
bool processEvents() {
    if(!aptMainLoop())
        return false;
    gspWaitForVBlank();
    hidScanInput();
    if(hidKeysDown() & KEY_START)
        return false;
    return true;
}
void swapFB() {
    gfxFlushBuffers();
    gfxSwapBuffers();
}
void deinit() {
    gfxExit();
}
