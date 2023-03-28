#ifndef ENGINE_H
#define ENGINE_H 1

#include "common.h"

typedef struct SpriteDef {
    int x: 12;
    int w: 4;
    uint y: 12;
    uint h: 4;
    uint tile: 12;
    uint hflip: 1;
    uint vflip: 1;
    uint priority: 1;
    int rotation: 16;
} SpriteDef;

typedef struct TileDef {
    uint hflip: 1;
    uint vflip: 1;
    uint priority: 1;
    uint tile: 12;
} TileDef;

typedef struct Mode7Table {
    int h: 16;
    int v: 16;
    int x: 16;
    int y: 16;
    int a: 16;
    int b: 16;
    int c: 16;
    int d: 16;
} Mode7Table;

typedef struct JoyState {
    u32 buttons;
    s16 axes[6];
} JoyState;

#define X sizeof(JoyState)

typedef void (*HBlankCB)(u16 line); 
typedef void (*VBlankCB)();

typedef enum EngSetting {
    SET_COUNT,
} EngSetting;

typedef enum Plane {
    PLANE_A,
    PLANE_B,

    PLANE_COUNT,
} Plane;

typedef enum ScrollType {
    SCROLL_PLANE,
    SCROLL_TILE,
    SCROLL_INVALID,
    SCROLL_LINE,
} ScrollType;

typedef enum JoyButtons {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_START,
    BUTTON_MODE,
    BUTTON_LB,
    BUTTON_RB,
    BUTTON_LS,
    BUTTON_RS,

    BUTTON_COUNT,
} JoyButtons;

typedef enum JoyAxes {
    AXIS_LX,
    AXIS_LY,
    AXIS_RX,
    AXIS_RY,
    AXIS_LT,
    AXIS_RT,

    AXIS_COUNT,
} JoyAxes;

extern void Eng_Init(u16 width, u16 height);
extern void Eng_SetWinTitle(const char * title);
extern void Eng_Destroy();
extern void Eng_Loop();

extern void Eng_SetHBlankCB(HBlankCB cb);
extern void Eng_SetVBlankCB(VBlankCB cb);

extern u16 Eng_Read(u32 addr);
extern void Eng_Write(u32 addr, u16 val, u16 mask);

#define Eng_ReadByte(addr) (u8)(addr & 1 ? Eng_Read(addr & 0x3FFFE) >> 8 : Eng_Read(addr & 0x3FFFE) & 0xFF)
#define Eng_ReadWord(addr) (u16)(Eng_Read(addr))
#define Eng_ReadLong(addr) (u32)(((u32)Eng_Read(addr) << 16) | (u32)Eng_Read(addr + 2))

#define Eng_WriteByte(addr, val) {Eng_Write(addr & 0x3FFFE, addr & 1 ? (u16)val : (u16)val << 1, addr & 1 ? 0xFF00 : 0x00FF);}
#define Eng_WriteWord(addr, val) {Eng_Write(addr, val, 0xFFFF);}
#define Eng_WriteLong(addr, val) {Eng_Write(addr, (u16)(val >> 16), 0xFFFF); Eng_Write(addr+2, (u16)(val), 0xFFFF);}

#endif