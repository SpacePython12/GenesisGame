#include <stdio.h>
#include "../include/common.h"

bool shouldLoad = true;

void VBlankFN() {
    if (shouldLoad) {
        Eng_OpenTiles("player.bin", 1, 128);
        Eng_OpenPalette("player.bin", 0, 16);
        Eng_WriteWord(0x20C00, 0xFFFE);
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 16; x++) {
                Eng_WriteWord(0x10000+((x+(y*64))*2), TILEDEF(x+(y*16)+1, false, false, false));
            }
        }
        for (int i = 0; i < 0x100; i+=2) {
            Eng_WriteWord(0x40 + i, (i << 8) | (i+1));
        }
        printf("Color 1: %.4x\n", Eng_ReadWord(0x20C02));
        shouldLoad = false;
    }
}

void HBlankFN(u16 scanline) {

}

int main() {
    Eng_Init(320, 224);
    Eng_SetVBlankCB(&VBlankFN);
    Eng_SetHBlankCB(&HBlankFN);
    Eng_Loop();
    Eng_Destroy();
}