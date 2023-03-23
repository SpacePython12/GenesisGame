#ifndef GAMEPAD_H
#define GAMEPAD_H 1

#include "types.h"

#define GPD_BUTTON_UP (1 << 0)
#define GPD_BUTTON_DOWN (1 << 1)
#define GPD_BUTTON_LEFT (1 << 2)
#define GPD_BUTTON_RIGHT (1 << 3)
#define GPD_BUTTON_B (1 << 4)
#define GPD_BUTTON_C (1 << 5)
#define GPD_BUTTON_A (1 << 6)
#define GPD_BUTTON_START (1 << 7)
#define GPD_BUTTON_Z (1 << 8)
#define GPD_BUTTON_Y (1 << 9)
#define GPD_BUTTON_X (1 << 10)
#define GPD_BUTTON_MODE (1 << 11)

extern void gpdUpdate();
extern u16 gpdGetP1State();
extern u16 gpdGetP1Change();
extern u16 gpdGetP2State();
extern u16 gpdGetP2Change();

#endif