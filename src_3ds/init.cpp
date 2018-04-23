#include <init.hpp>
#include <iostream>

#include <3ds.h>
#include <citro3d.h>

namespace {
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
    
}
bool processEvents() {
    if(!aptMainLoop())
        return false;
    hidScanInput();
    if(hidKeysDown() & KEY_START)
        return false;
    return true;
}
void swapFB() {
}
void deinit() {
    gfxExit();
}
