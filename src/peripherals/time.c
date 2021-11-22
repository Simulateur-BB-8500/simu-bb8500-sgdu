/*
 * time.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "time.h"

#ifdef WINDOWS
#include "windows.h"
#endif

/*** TIME local structures ***/

typedef struct {
	unsigned long time_start_ms;
#ifdef WINDOWS
	SYSTEMTIME time_structure;
#endif
} TIME_Context;

/*** TIME local global variables ***/

static TIME_Context time_ctx;

/*** TIME functions ***/

/* INIT SYSTEM TIME.
 * @param:	None.
 * @return: None.
 */
void TIME_Init(void) {
#ifdef WINDOWS
	GetSystemTime(&(time_ctx.time_structure));
	time_ctx.time_start_ms = (time_ctx.time_structure.wHour * 3600000) +
							 (time_ctx.time_structure.wMinute * 60000) +
							 (time_ctx.time_structure.wSecond * 1000) +
							 (time_ctx.time_structure.wMilliseconds);
#endif
}

/* RETURN THE CURRENT PROGRAM TIME.
 * @param:	None.
 * @return:	Number of milliseconds ellapsed since the program started.
 */
unsigned long TIME_GetMs(void) {
	// Local variables.
	unsigned long now = 0;
#ifdef WINDOWS
	GetSystemTime(&(time_ctx.time_structure));
	now = (time_ctx.time_structure.wHour * 3600000) +
		  (time_ctx.time_structure.wMinute * 60000) +
		  (time_ctx.time_structure.wSecond * 1000) +
		  (time_ctx.time_structure.wMilliseconds);
#endif
	return (now - time_ctx.time_start_ms);
}
