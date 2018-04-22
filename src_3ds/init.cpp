#include <init.hpp>
#include <iostream>

#include <3ds.h>

void init(int argc, const char** argv) {
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);
    if(romfsInit() < 0) {
        std::cerr << "Could not open the romfs!" << std::endl;
        throw nullptr;
    }
    if(sdmcInit() < 0) {
        std::cerr << "Could not open the sdmc!" << std::endl;
        throw nullptr;
    }
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
