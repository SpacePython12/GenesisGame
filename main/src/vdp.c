#include "common.h"

extern u8 * VDPRegValues;
extern u8 * VDPAddrDests;

typedef struct VDPDMARequest {
    u32 src68k;
    u16 dstVDP;
    u16 size;
    u8 dest;
    u8 flags;
    u16 val;
} VDPDMARequest;

#define dmaQueueCapacity 32

static VDPDMARequest dmaQueue[dmaQueueCapacity];
static u32 dmaQueueFront = 0, dmaQueueBack = dmaQueueCapacity - 1, dmaQueueSize = 0;

static bool initialized = false;

#define PLNA_ADDR 0xC000
#define PLNB_ADDR 0xE000
#define WIND_ADDR 0xB000
#define SPTB_ADDR 0xB800
#define HSCR_ADDR 0xBC00

u16 vdpStatus() {
    return wordRead(0xC00004);
}

void vdpRegister(u8 reg, u8 val) {
    wordWrite(0xC00004, ((0x80+(reg & 0x1F)) << 8) + ((u16)val));
}

u16 vdpReadData() {
    return wordRead(0xC00000);
}

void vdpWriteData(u16 val) {
    wordWrite(0xC00000, val);
}

void vdpAddress(u16 addr, u8 dest, u8 flags) {
    u8 tmp = flags | ((u8 *)&VDPAddrDests)[dest];
    longWrite(0xC00004, ((((u32)addr & 0x3FFF) << 16) | ((u32)addr >> 14) | (((u32)tmp & 0x3C) << 2) | (((u32)tmp & 0x03) << 30)));
}

u16 vdpReadVRAM(u16 addr) {
    vdpAddress(addr, (VDP_VRAM_DEST | VDP_READ_DEST), 0);
    return vdpReadData();
}

void vdpWriteVRAM(u16 addr, u16 val) {
    vdpAddress(addr, (VDP_VRAM_DEST | VDP_WRITE_DEST), 0);
    vdpWriteData(val);
}

u16 vdpReadCRAM(u8 addr) {
    vdpAddress(addr, (VDP_CRAM_DEST | VDP_READ_DEST), 0);
    return vdpReadData();
}

void vdpWriteCRAM(u8 addr, u16 val) {
    vdpAddress(addr, (VDP_CRAM_DEST | VDP_WRITE_DEST), 0);
    vdpWriteData(val);
}

u16 vdpReadVSRAM(u8 addr) {
    vdpAddress(addr, (VDP_VSRAM_DEST | VDP_READ_DEST), 0);
    return vdpReadData();
}

void vdpWriteVSRAM(u8 addr, u16 val) {
    vdpAddress(addr, (VDP_VSRAM_DEST | VDP_WRITE_DEST), 0);
    vdpWriteData(val);
}

void vdpDoDMA(const void * src68k, u16 dstVDP, u16 len, u8 dest, u8 flags) {
    vdpPushDMA((VDPDMARequest){.dstVDP=dstVDP, .size=len, .src68k=(u32)src68k, .flags=(VDP_DMA_FLAG | flags), .dest=(dest | VDP_WRITE_DEST), .val=0});
}

void vdpDoCopy(u16 srcVDP, u16 dstVDP, u16 len, u8 flags) {
    vdpPushDMA((VDPDMARequest){.dstVDP=dstVDP, .size=len, .src68k=0x1800000 | srcVDP, .flags=(VDP_DMA_FLAG | VDP_VCOPY_FLAG | flags), .dest=0, .val=0});
}

void vdpDoFill(u16 dstVDP, u16 len, u8 val, u8 flags) {
    vdpPushDMA((VDPDMARequest){.dstVDP=dstVDP, .size=len, .src68k=0x1000000, .flags=(VDP_DMA_FLAG | VDP_VCOPY_FLAG | flags), .dest=0, .val=(u16)val << 8});
}

void vdpPushDMA(VDPDMARequest request) {
    if (dmaQueueSize == dmaQueueCapacity) return;
    dmaQueueBack = (dmaQueueBack + 1) % dmaQueueCapacity;
    dmaQueue[dmaQueueBack] = request;
    dmaQueueSize++;
}

void vdpPopDMA() {
    if (dmaQueueSize == 0) return;
    VDPDMARequest request = dmaQueue[dmaQueueFront];
    dmaQueueFront = (dmaQueueFront + 1) % dmaQueueCapacity;
    dmaQueueSize--;
    vdpRegister(0x13, ((request.size >> 1) & 0xFF));
    vdpRegister(0x14, (((request.size >> 1) & 0xFF00) >> 8));
    vdpRegister(0x15, ((request.src68k >> 1) & 0xFF));
    vdpRegister(0x16, (((request.src68k >> 1) & 0xFF00) >> 8));
    vdpRegister(0x17, (((request.src68k >> 1) & 0xFF0000) >> 16));
    vdpAddress(request.dstVDP, request.dest, request.flags);
    if (request.src68k & 0xC00000 == 0x800000) vdpWriteData(request.val);
}

bool vdpDMAActive() {
    return vdpStatus() & (1 << 1) ? true : false;
}

bool vdpIsPAL() {
    return vdpStatus() & (1 << 0) ? true : false;
}

bool vdpIsVBlank() {
    return vdpStatus() & (1 << 3) ? true : false;
}

bool vdpIsHBlank() {
    return vdpStatus() & (1 << 2) ? true : false;
}

void vdpInit() {
    if (initialized) return;
    initialized = true;
    register u8 i = 0, tmp = 0;
    if (byteRead(0xA10001) & 0x0F != 0) {
        longWrite(0xA14000, 0x53454741); // "SEGA"
    }
    for (i = 0x00; i < 0x13; i++) vdpRegister(i, ((u8 *)&VDPRegValues)[i]);
}

void vdpUpdate() {
    if (!vdpDMAActive()) {
        if (!vdpIsVBlank() && dmaQueue[dmaQueueFront].flags & VDP_VBLK_ONLY) return;
        if (!vdpIsHBlank() && dmaQueue[dmaQueueFront].flags & VDP_HBLK_ONLY) return;
        vdpPopDMA();
    }
}

/*
    Run length encoding of tile data:
        * 4 bits per pixel
        * One byte can store up to 16 occurrences of a pixel (count+1)
        * 0xCI -> C: count-1; I: palette index
        * Ex: 0x4D -> 5 occurences of palette index 0xD
*/

u32 vdpDecodeTileRLE(void * src, void * dest, u32 size, u32 max) {
    u32 complete = 0;
    u8 tmp[8];
    for (u32 i = 0; i < size; i++) {
        tmp[0] = *(u8 *)(src+i);
        for (u8 j = 0; j <= (tmp[0] & 0xF0) >> 4; i++) {
            if (complete >> 1 < max) {
                tmp[1] = *(u8 *)dest+(i+(complete >> 1));
                ((u8 *)dest)[i+(complete >> 1)] = (u8)(complete & 1 ? (tmp[0] << 4) : (tmp[0] & 0xF) | tmp[1]);
                complete++;
            } else {
                return complete;
            }
        }
    }
}