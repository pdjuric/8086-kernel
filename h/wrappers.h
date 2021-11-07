#ifndef LIBWRAP_H_
#define LIBWRAP_H_

#include <iostream.h>
#include "stdio.h"
#include <stdlib.h>
#include <dos.h>
#include <stdarg.h>

extern volatile int lockFlag;
extern volatile int contextSwitchDemand;

#define lock() {lockFlag++;}
#define unlock() {lockFlag--; if ( contextSwitchDemand == 1) { dispatch(); 	}}

#define lockSem() {semLockFlag = 1;}
#define unlockSem() {semLockFlag = 0; while (releaseFlag > 0) {release(); releaseFlag--; }}

#define alloc(a, b) {lock(); a = new b; if (a == 0) print("ERROR: COULDN'T ALLOCATE MEMORY!\n");  unlock();}
#define dealloc(a) {lock(); delete a; a = 0; unlock();}
#define deallocArray(a) {lock(); delete [] a; a = 0; unlock();}

int print(const char *format, ...);

#define noInterrupts() asm pushf; asm cli
#define okInterrupts() asm popf

#endif /* LIBWRAP_H_ */

