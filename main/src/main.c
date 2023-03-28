#include "common.h"

extern u32 * TestTileData;
extern u32 TestTileDataSize;
extern u16 * TestColorData;
extern u32 TestColorDataSize;

void VBlankFN() {
    
}

int main() {
    Eng_Init(320, 224);
    Eng_SetVBlankCB(&VBlankFN);
    Eng_Loop();
    Eng_Destroy();
}