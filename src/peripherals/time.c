/*
 * time.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "time.h"

#include "stdint.h"
#include "windows.h"

/*** TIME local structures ***/

/*******************************************************************/
typedef struct {
	unsigned long start_ms;
	SYSTEMTIME system_time;
} TIME_context_t;

/*** TIME local global variables ***/

static TIME_context_t time_ctx;

/*** TIME functions ***/

/*******************************************************************/
void TIME_init(void) {
	// Set start value.
	GetSystemTime(&(time_ctx.system_time));
	time_ctx.start_ms = (time_ctx.system_time.wHour * 3600000) +
						(time_ctx.system_time.wMinute * 60000) +
						(time_ctx.system_time.wSecond * 1000) +
						(time_ctx.system_time.wMilliseconds);
}

/*******************************************************************/
uint64_t TIME_get_milliseconds(void) {
	// Local variables.
	unsigned long now = 0;
	// Read current time.
	GetSystemTime(&(time_ctx.system_time));
	now = (time_ctx.system_time.wHour * 3600000) +
		  (time_ctx.system_time.wMinute * 60000) +
		  (time_ctx.system_time.wSecond * 1000) +
		  (time_ctx.system_time.wMilliseconds);
	// Return the delta.
	return (now - time_ctx.start_ms);
}
