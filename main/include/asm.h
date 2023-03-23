#ifndef ASM_H
#define ASM_H 1

#define asm_byteRead(addr, val) asm volatile ("move.b (%1) %0" : "=r" (val) : "m" (addr))
#define asm_wordRead(addr, val) asm volatile ("move.w (%1) %0" : "=r" (val) : "m" (addr))
#define asm_longRead(addr, val) asm volatile ("move.l (%1) %0" : "=r" (val) : "m" (addr))

#define asm_byteWrite(addr, val) asm volatile ("move.b %0 (%1)" :: "r" (val), "m" (addr))
#define asm_wordWrite(addr, val) asm volatile ("move.w %0 (%1)" : "=r" (val) : "m" (addr))
#define asm_longWrite(addr, val) asm volatile ("move.l %0 (%1)" : "=r" (val) : "m" (addr))

#define byteAccess(addr) *(u8*)(addr)
#define wordAccess(addr) *(u16*)(addr)
#define longAccess(addr) *(u32*)(addr)

static inline u8 byteRead(const u32 addr) {
    register u8 val;
    asm volatile ("moveb %1@,%0" : "=r" (val) : "ip" ((void *)addr) : "memory");
    return val;
}

static inline u16 wordRead(const u32 addr) {
    register u16 val;
    asm volatile ("movew %1@,%0" : "=r" (val) : "ip" ((void *)addr) : "memory");
    return val;
}

static inline u32 longRead(const u32 addr) {
    register u32 val;
    asm volatile ("movel %1@,%0" : "=r" (val) : "ip" ((void *)addr) : "memory");
    return val;
}

static inline void byteWrite(const u32 addr, u8 val) {
    asm volatile ("moveb %0,%1@" :: "ir" (val), "ip" ((void *)addr) : "memory");
}

static inline void wordWrite(const u32 addr, u16 val) {
    asm volatile ("movew %0,%1@" :: "ir" (val), "ip" ((void *)addr) : "memory");
}

static inline void longWrite(const u32 addr, u32 val) {
    asm volatile ("movel %0,%1@" :: "ir" (val), "ip" ((void *)addr) : "memory");
}

#endif