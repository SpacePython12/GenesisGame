#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <unistd.h>
#include <zip.h>

#include "../include/common.h"

static SDL_Window * Win;
static SDL_Renderer * Rend;
static SDL_Texture * FBuf;

static zip_t * archive;
static int ziperr;

static u8 * TileMem;
static u16 * LayoutMem;
static s16 * HScrollMem;
static s16 * VScrollMem;
static SpriteDef * SpriteMem;
static u16 * ColorMem;

static JoyState JoyPads[4];
static Mode7Table Mode7Tables[PLANE_COUNT];

static HBlankCB _HBlankCB = NULL;
static VBlankCB _VBlankCB = NULL;

static ScrollType HScrollMode = SCROLL_PLANE;
static ScrollType VScrollMode = SCROLL_PLANE;

#define PLANEA(index) (LayoutMem+((index)*2)+0x0000)
#define PLANEB(index) (LayoutMem+((index)*2)+0x4000)

#define LAYOUT(index, plane) (LayoutMem+((index)*2)+((plane)*0x4000))
#define LAYOUTXY(x, y, plane) LAYOUT((x)+((y)*0x40), plane)

#define HSCROLL_LN(line, plane) (HScrollMem+((line)*2)+((plane)*0x200))
#define HSCROLL_TI(tile, plane) (HScrollMem+((tile)*16)+((plane)*0x200))
#define VSCROLL_TI(tile, plane) (VScrollMem+((tile)*2)+((plane)*0x80))
#define HSCROLL_PL(plane) (HScrollMem+((plane)*0x200))
#define VSCROLL_PL(plane) (VScrollMem+((plane)*0x80))

#define TILE(index) (TileMem+((index)*0x40))
#define TILEXY(index, x, y) (TileMem+((index)*0x40)+(x)+((y)*8))

#define SPRITE(index) (SpriteMem+((index)*8))

#define PALETTE(index) (ColorMem[index])

#define S16TONORM(val) (((double)val/(double)0x100))
#define NORMTOS16(val) ((s16)(val*(double)0x100))

#define BOOLNEGATE(val, sign) (sign ? -val : val)

void Eng_Init(u16 width, u16 height) {
    archive = zip_open("data.pak", 0, &ziperr);

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("SDL failed to initialize.\n");
        exit(0);
    }
    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, 0, &mode)) {
        printf("SDL error: %s\n", SDL_GetError());
    }
    Win = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width*3, height*3, (SDL_WINDOW_OPENGL));
    Rend = SDL_CreateRenderer(Win, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    FBuf = SDL_CreateTexture(Rend, SDL_PIXELFORMAT_RGBA5551, SDL_TEXTUREACCESS_STREAMING, width, height);
    TileMem = calloc(0x400, 0x40);
    LayoutMem = calloc(0x8000, 2);
    HScrollMem = calloc(0x200, 2);
    VScrollMem = calloc(0x80, 2);
    SpriteMem = calloc(0x80, 8);
    ColorMem = calloc(0x200, 2);
    int i;
    for (i = 0; i < PLANE_COUNT; i++) {
        Mode7Tables[i].a = 0x100;
        Mode7Tables[i].d = 0x100;
    }
}

void Eng_SetWinTitle(const char * title) {
    SDL_SetWindowTitle(Win, title);
}

void Eng_Destroy() {
    SDL_DestroyWindow(Win);
    SDL_DestroyRenderer(Rend);
    SDL_DestroyTexture(FBuf);
    free(TileMem);
    free(HScrollMem);
    free(VScrollMem);
    free(SpriteMem);
    free(ColorMem);
    free(LayoutMem);
    zip_close(archive);
}

void Eng_SetHBlankCB(HBlankCB cb) {
    _HBlankCB = cb;
}

void Eng_SetVBlankCB(VBlankCB cb) {
    _VBlankCB = cb;
}

void Eng_Access(u32 addr, u16 * val, bool write) {
    if (addr >= 0x00000 && addr < 0x10000) { // Tile Memory
        if (!write) *val = *(u16 *)(TileMem+(addr & 0x0FFFF));
        else *(u16 *)(TileMem+(addr & 0x0FFFF)) = *val;
    } else if (addr >= 0x10000 && addr < 0x20000) { // Layout Memory
        if (!write) *val = *(u16 *)(LayoutMem+(addr & 0x0FFFF));
        else *(u16 *)(LayoutMem+(addr & 0x0FFFF)) = *val;
    } else if (addr >= 0x20000 && addr < 0x20400) { // HScroll Memory
        if (!write) *val = *(u16 *)(HScrollMem+(addr & 0x3FF));
        else *(u16 *)(HScrollMem+(addr & 0x3FF)) = *val;
    } else if (addr >= 0x20400 && addr < 0x20500) { // VScroll Memory
        if (!write) *val = *(u16 *)(VScrollMem+(addr & 0x0FF));
        else *(u16 *)(VScrollMem+(addr & 0x0FF)) = *val;
    } else if (addr >= 0x20500 && addr < 0x20510) { // Mode 7 Tables
        if (!write) *val = *(u16 *)(Mode7Tables+(addr & 0xF));
        else *(u16 *)(Mode7Tables+(addr & 0xF)) = *val;
    } else if (addr >= 0x20800 && addr < 0x20C00) { // Sprite Memory
        if (!write) *val = *(u16 *)(SpriteMem+(addr & 0x3FF));
        else *(u16 *)(SpriteMem+(addr & 0x3FF)) = *val;
    } else if (addr >= 0x20C00 && addr < 0x20E00) { // Color Memory
        if (!write) *val = *(u16 *)(ColorMem+(addr & 0x1FF));
        else *(u16 *)(ColorMem+(addr & 0x1FF)) = *val & 0xFFFE;

    } else if (addr >= 0x20E00 && addr < 0x20E40) { // Joypads
        if (!write) *val = *(u16 *)(JoyPads+(addr & 0x3F));
    } else {
        if (!write) *val = 0;
    }
}

u16 Eng_Read(u32 addr) {
    u16 tmp = 0;
    Eng_Access(addr, &tmp, false);
    return tmp;
}

void Eng_Write(u32 addr, u16 val, u16 mask) {
    u16 tmp = (val & mask) | (Eng_Read(addr) & ~mask); 
    Eng_Access(addr, &tmp, true);
}

void Eng_OpenResourceRaw(const char * parent, const char * name, zip_file_t ** file) {
    char * path = calloc(strlen(parent)+strlen(name)+2, 1);
    strcat(path, parent);
    strcat(path, ":");
    strcat(path,name);
    for (uint i = 0; i < strlen(path); i++) {
        if (path[i] == ':') path[i] = '/';
    }
    struct zip_stat stat;
    zip_stat_init(&stat);
    *file = zip_fopen(archive, path, 0);
}

size_t Eng_OpenResource(const char * namespace, const char * name, void * ptr, size_t offset, size_t size) {
    size_t ret = 0;
    zip_file_t * file;
    Eng_OpenResourceRaw(namespace, name, &file);
    if (ptr) {
        zip_fseek(file, offset, SEEK_SET);
        ret = zip_fread(file, ptr, size);
    } else {
        zip_fseek(file, 0, SEEK_END);
        ret = zip_ftell(file);
    }
    zip_fclose(file);
    return ret;
}

void Eng_OpenPalette(const char * name, u8 offset, u8 count) {
    zip_file_t * file;
    Eng_OpenResourceRaw("palette", name, &file);
    u16 size = offset + count >= 0x100 ? 0x100-offset : count;
    zip_fread(file, ColorMem+(offset*2), size*2);
    zip_fclose(file);
}

void Eng_LoadPalette(const u16 * palette, u8 offset, u8 count) {
    u16 size = offset + count >= 0x100 ? 0x100-offset : count;
    memmove(ColorMem+(offset*2), palette, size*2);
}

void Eng_StorePalette(u16 * palette, u8 offset, u8 count) {
    u16 size = offset + count >= 0x100 ? 0x100-offset : count;
    memmove(palette, ColorMem+(offset*2), size*2);
}

void Eng_OpenTiles(const char * name, u16 offset, u16 count) {
    zip_file_t * file;
    Eng_OpenResourceRaw("tileset", name, &file);
    
    u32 size = offset + count >= 0x400 ? 0x400-offset : count;
    zip_fread(file, TileMem+(offset*0x40), size*0x40);
    zip_fclose(file);
}

void Eng_LoadTiles(const u8 * tiles, u16 offset, u16 count) {
    u32 size = offset + count >= 0x400 ? 0x400-offset : count;
    memmove(TileMem+(offset*0x40), tiles, size*0x40);
}

void Eng_StoreTiles(u8 * tiles, u16 offset, u16 count) {
    u32 size = offset + count >= 0x400 ? 0x400-offset : count;
    memmove(tiles, TileMem+(offset*0x40), size*0x40);
}

void Eng_OpenLayout(const char * name, u16 offset, u16 count) {
    zip_file_t * file;
    Eng_OpenResourceRaw("tilemap", name, &file);
    u32 size = offset + count >= 0x8000 ? 0x8000-offset : count;
    zip_fread(file, LayoutMem+(offset*2), size*2);
    zip_fclose(file);
}

void Eng_LoadLayout(const u16 * layout, u16 offset, u16 count) {
    u32 size = offset + count >= 0x8000 ? 0x8000-offset : count;
    memmove(LayoutMem+(offset*2), layout, size*2);
}

void Eng_StoreLayout(u16 * layout, u16 offset, u16 count) {
    u32 size = offset + count >= 0x8000 ? 0x8000-offset : count;
    memmove(layout, LayoutMem+(offset*2), size*2);
}

static inline s16 Eng_GetRowScroll(Plane plane, u16 row) {
    switch (HScrollMode) {
        case SCROLL_INVALID:
        case SCROLL_PLANE:
            return *(HSCROLL_PL(plane));
        case SCROLL_TILE:
            return *(HSCROLL_TI(row >> 3, plane));
        case SCROLL_LINE:
            return *(HSCROLL_LN(row, plane));
    }
    return 0;
}

static inline s16 Eng_GetColScroll(Plane plane, u16 col) {
    switch (VScrollMode) {
        case SCROLL_INVALID:
        case SCROLL_PLANE:
            return *(VSCROLL_PL(plane));
        case SCROLL_LINE:
        case SCROLL_TILE:
            return *(VSCROLL_TI(col >> 3, plane));
    }
    return 0;
}

static inline u8 Eng_GetTileColor(Plane plane, u16 x, u16 y) {
    // u16 tx = (u16)((double)x * SDL_cos(S16TORAD(RotationTable[plane]))) - (u16)((double)y * SDL_sin(S16TORAD(RotationTable[plane])));
    // u16 ty = (u16)((double)x * SDL_sin(S16TORAD(RotationTable[plane]))) + (u16)((double)y * SDL_cos(S16TORAD(RotationTable[plane])));
    u16 tx = S16TONORM(Mode7Tables[plane].a) * ((double)x + (double)(Mode7Tables[plane].h) - (double)(Mode7Tables[plane].x)) + \
        S16TONORM(Mode7Tables[plane].b) * ((double)y + (double)(Mode7Tables[plane].v) - (double)(Mode7Tables[plane].y)) + (double)(Mode7Tables[plane].x);
    u16 ty = S16TONORM(Mode7Tables[plane].c) * ((double)x + (double)(Mode7Tables[plane].h) - (double)(Mode7Tables[plane].x)) + \
        S16TONORM(Mode7Tables[plane].d) * ((double)y + (double)(Mode7Tables[plane].v) - (double)(Mode7Tables[plane].y)) + (double)(Mode7Tables[plane].y);
    //printf("X: %x, Y: %x, TX: %x, TY: %x\n", x, y, tx, ty);
    if (false) {
        if (tx >= 0x800 || ty >= 0x800) return 0;
    } else {
        tx %= 0x400;
        ty %= 0x400;
    }
    u16 tile = *(LAYOUTXY(tx >> 3, ty >> 3, plane)) & (0xFFF);
    u8 ind = *(TILEXY(tile % 0x800, tx % 8, ty % 8));
    // printf("Tile: %x, Index: %x\n", tile, ind);
    return ind;
}

static inline u8 Eng_GetSpriteColor(u8 index, u8 x, u8 y) {
    SpriteDef sprite = SpriteMem[index];
    x += (sprite.w << 2);
    y += (sprite.h << 2);
    u16 tx = (u16)((double)x * SDL_cos(S16TONORM(-sprite.rotation)*M_PI)) - (u16)((double)y * SDL_sin(S16TONORM(-sprite.rotation)*M_PI));
    u16 ty = (u16)((double)x * SDL_sin(S16TONORM(-sprite.rotation)*M_PI)) + (u16)((double)y * SDL_cos(S16TONORM(-sprite.rotation)*M_PI));
    tx -= (sprite.w << 2);
    ty -= (sprite.h << 2);
    if (tx >= (sprite.w << 3) || ty >= (sprite.h << 3)) return 0;
    u8 ind = *(TILEXY((tx >> 3) + ((ty >> 3)+sprite.w), tx % 8, ty % 8));
    return ind;
}

void Eng_Loop() {
    bool running = true;
    int x, y, w, h, i;
    u16 * pixels;
    u16 tmp;
    int pitch;
    double scroll = 0.0;
    Mode7Tables[PLANE_A].a = 256;
    Mode7Tables[PLANE_A].d = 256;
    bool shiftKeyPressed = false;
    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            Mode7Tables[PLANE_A].v -= 1;
                            break;
                        case SDLK_DOWN:
                            Mode7Tables[PLANE_A].v += 1;
                            break;
                        case SDLK_LEFT:
                            Mode7Tables[PLANE_A].h -= 1;
                            break;
                        case SDLK_RIGHT:
                            Mode7Tables[PLANE_A].h += 1;
                            break;
                    }
                    break;
            }
        }
        // SDL_RenderClear(Rend);
        SDL_QueryTexture(FBuf, NULL, NULL, &w, &h);
        SDL_LockTexture(FBuf, NULL, (void **)&pixels, &pitch);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                pixels[x + (y*w)] = (PALETTE(0));
                tmp = Eng_GetTileColor(PLANE_B, x + Eng_GetRowScroll(PLANE_B, y), y + Eng_GetColScroll(PLANE_B, x));
                if (tmp) pixels[x + (y*w)] = (PALETTE(tmp & 0xFF));
                tmp = Eng_GetTileColor(PLANE_A, x + Eng_GetRowScroll(PLANE_A, y), y + Eng_GetColScroll(PLANE_A, x));
                if (tmp) pixels[x + (y*w)] = (PALETTE(tmp & 0xFF));
                // for (i = 0; i < 0x80; i++) {
                //     tmp = Eng_GetSpriteColor(i, x - SpriteMem[i].x, y - SpriteMem[i].y);
                //     if (tmp) pixels[x + (y*w)] = *(PALETTE(tmp & 0xFF));
                // }
            }
            if (_HBlankCB) _HBlankCB(y);
        }
        if (_VBlankCB) _VBlankCB();
        SDL_UnlockTexture(FBuf);
        SDL_RenderCopy(Rend, FBuf, NULL, NULL);
        SDL_RenderPresent(Rend);
    }
}