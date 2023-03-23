#include "common.h"

extern u32 * TestTileData;
extern u16 * TestColorData;

void main(bool hard) {
    vdpDoDMA(&TestColorData, 0, 0x20, VDP_CRAM_DEST, VDP_VBLK_ONLY);
    vdpDoDMA(&TestTileData, 0, 0x20, VDP_VRAM_DEST, VDP_VBLK_ONLY);
}

void update() {
    gpdGetP1State();
}