#include "common.h"

extern u32 * TestTileData;
extern u32 TestTileDataSize;
extern u16 * TestColorData;
extern u32 TestColorDataSize;

void main(bool hard) {
    vdpDoDMA(&TestColorData, 0, TestColorDataSize, VDP_CRAM_DEST, 0);
    vdpUpdate();
    vdpDoDMA(&TestTileData, 0, TestTileDataSize, VDP_VRAM_DEST, 0);
    vdpUpdate();
}

void update() {
    gpdGetP1State();
}