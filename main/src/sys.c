#include "common.h"

extern char * Z80Driver;
extern u32 * Z80Driver_Size;

bool hardReset() {
    return (!longRead(0xA10008) || !longRead(0xA1000C));
}

void mdInit() {
    asm ("movew %0,%%sr" :: "i" (0x2500));
    u32 i;
    // Has the console been hard-reset?
    if (hardReset()) {

        // Clear RAM
        // for (i = 0xFF0000; i <= 0xFF7FFC; i += 4) {
        //     longWrite(i, 0);
        // }

        // If hardware revision is not 0, satisfy TMSS by writing "SEGA" to TMSS register

        vdpInit();

        // Program Z80
        // wordWrite(0xA11100, 0x0100); // Z80 BUSREQ
        // wordWrite(0xA11200, 0x0100); // Z80 RESET

        // // Waits until the 68k has access to the Z80's bus
        // while (wordRead(0xA11100) & 1) {}

        // for (i = 0; i < *Z80Driver_Size; i+=2) {
        //     wordWrite(0xA00000 + i, wordRead((u32)(&Z80Driver + i)));
        // }

        // wordWrite(0xA11200, 0x0000); // Z80 RESET
    }
}


void _vblank() {
    vdpUpdate();
    gpdUpdate();
    update();
}

void _hblank() {
    vdpUpdate();
}

void _int() {
    while (true) {}
}

void _irq() {
    asm ("rte");
}

void _start() {
    // mdInit();
    vdpInit();
    main(hardReset());
    while(true) {}
}