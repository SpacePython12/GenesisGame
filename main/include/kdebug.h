#ifndef _INC_DEBUG_
#define _INC_DEBUG_

#include "types.h"

extern void Debug_Halt( );
extern void Debug_Alert(char *str);
extern void Debug_AlertNumber(u32 nVal);
extern void Debug_StartTimer( );
extern void Debug_StopTimer( );


#endif