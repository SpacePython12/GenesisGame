#ifndef VDP_H
#define VDP_H 1

#include "types.h"

#define VDP_VRAM_DEST 0x0
#define VDP_CRAM_DEST 0x2
#define VDP_VSRAM_DEST 0x4

#define VDP_READ_DEST 0x0
#define VDP_WRITE_DEST 0x1

#define VDP_TILE_SIZE 0x20

#define VDP_VCOPY_FLAG (1 << 4)
#define VDP_DMA_FLAG (1 << 5)
#define VDP_VBLK_ONLY (1 << 6) // Use for when this DMA should only happen during VBlank
#define VDP_HBLK_ONLY (1 << 7) // Use for when this DMA should only happen during HBlank

#define VDP_TRANS_DMA (0)

extern void vdpInit();
extern void vdpUpdate();
extern u16 vdpStatus();
extern void vdpRegister(u8 reg, u8 val);
extern u16 vdpReadData();
extern void vdpWriteData(u16 val);
extern void vdpAddress(u16 addr, u8 dest, u8 flags);
extern u16 vdpReadVRAM(u16 addr);
extern void vdpWriteVRAM(u16 addr, u16 val);
extern u16 vdpReadCRAM(u8 addr);
extern void vdpWriteCRAM(u8 addr, u16 val);
extern u16 vdpReadVSRAM(u8 addr);
extern void vdpWriteVSRAM(u8 addr, u16 val);

extern void vdpDoDMA(const void * src68k, u16 dstVDP, u16 len, u8 dest, u8 flags);
extern void vdpDoCopy(u16 srcVDP, u16 dstVDP, u16 len, u8 flags);
extern void vdpDoFill(u16 dstVDP, u16 len, u8 val, u8 flags);
extern bool vdpDMAActive();
extern bool vdpIsPAL();

extern u32 vdpDecodeTileRLE(void * src, void * dest, u32 size, u32 max);

#endif