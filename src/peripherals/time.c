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
	unsigned long start_ms;
#ifdef WINDOWS
	SYSTEMTIME system_time;
#endif
} TIME_context_t;

/*** TIME local global variables ***/

static TIME_context_t time_ctx;

/*** TIME functions ***/

/* INIT SYSTEM TIME.
 * @param:	None.
 * @return: None.
 */
void TIME_init(void) {
#ifdef WINDOWS
	GetSystemTime(&(time_ctx.system_time));
	time_ctx.start_ms = (time_ctx.system_time.wHour * 3600000) +
							 (time_ctx.system_time.wMinute * 60000) +
							 (time_ctx.system_time.wSecond * 1000) +
							 (time_ctx.system_time.wMilliseconds);
#endif
}

/* RETURN THE CURRENT PROGRAM TIME.
 * @param:	None.
 * @return:	Number of milliseconds ellapsed since the program started.
 */
unsigned long TIME_get_ms(void) {
	// Local variables.
	unsigned long now = 0;
#ifdef WINDOWS
	GetSystemTime(&(time_ctx.system_time));
	now = (time_ctx.system_time.wHour * 3600000) +
		  (time_ctx.system_time.wMinute * 60000) +
		  (time_ctx.system_time.wSecond * 1000) +
		  (time_ctx.system_time.wMilliseconds);
#endif
	return (now - time_ctx.start_ms);
}
