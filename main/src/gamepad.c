#include "common.h"

#define GPD1_CTL 0xA10003
#define GPD2_CTL 0xA10005
#define GPD1_DTA 0xA10009
#define GPD2_DTA 0xA1000B

static u16 gp1, gp2, ogp1, ogp2;

u16 gpdRead3B(u32 ctl, u32 dta) {
    u16 res, tmp;
    byteWrite(ctl, 0x40);
    byteWrite(dta, 0x40);
    res = byteRead(dta) & 0x3F;
    byteWrite(dta, 0x00);
    tmp = (byteRead(dta) & 0x30) << 2;
    res = res | tmp;
    return res;
}

u16 gpdRead6B(u32 ctl, u32 dta) {
    u16 res = gpdRead3B(ctl, dta), tmp;
    byteWrite(dta, 0x40);
    byteWrite(dta, 0x00);
    byteWrite(dta, 0x40);
    tmp = (byteRead(dta) & 0x0F) << 8;
    byteWrite(dta, 0x00);
    res = res | tmp;
    return res;
}

void gpdUpdate() {
    ogp1 = gp1;
    ogp2 = gp2;
    gp1 = gpdRead6B(GPD1_CTL, GPD1_DTA);
    gp2 = gpdRead6B(GPD2_CTL, GPD2_DTA);
}

u16 gpdGetP1State() {
    return gp1;
}

u16 gpdGetP1Change() {
    return gp1 ^ ogp1;
}

u16 gpdGetP2State() {
    return gp2;
}

u16 gpdGetP2Change() {
    return gp2 ^ ogp2;
}