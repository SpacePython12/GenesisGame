#ifndef SYS_H
#define SYS_H 1

#include "types.h"
#include "asm.h"

extern void main(bool hard);
extern void update();

extern void mdInit();
extern bool hardReset();

#endif