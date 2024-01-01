/*
 * time.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "time.h"

#include "stdint.h"
#include "stdio.h"
#include "windows.h"

/*** TIME local structures ***/

/*******************************************************************/
typedef struct {
	uint64_t start_ms;
	SYSTEMTIME system_time;
} TIME_context_t;

/*** TIME local global variables ***/

static TIME_context_t time_ctx;

/*** TIME functions ***/

/*******************************************************************/
void TIME_init(void) {
	// Set start value.
	GetLocalTime(&(time_ctx.system_time));
	time_ctx.start_ms = (time_ctx.system_time.wHour * 3600000) + (time_ctx.system_time.wMinute * 60000) + (time_ctx.system_time.wSecond * 1000) + (time_ctx.system_time.wMilliseconds);
}


/*******************************************************************/
void TIME_print(void) {
	// Read current time.
	GetLocalTime(&(time_ctx.system_time));
	// Print time.
	printf("%02d:%02d:%02d:%03d ", time_ctx.system_time.wHour, time_ctx.system_time.wMinute, time_ctx.system_time.wSecond, time_ctx.system_time.wMilliseconds);
}

/*******************************************************************/
uint32_t TIME_get_milliseconds(void) {
	// Local variables.
	uint64_t now = 0;
	// Read current time.
	GetLocalTime(&(time_ctx.system_time));
	now = (time_ctx.system_time.wHour * 3600000) + (time_ctx.system_time.wMinute * 60000) + (time_ctx.system_time.wSecond * 1000) + (time_ctx.system_time.wMilliseconds);
	// Return the delta.
	return ((uint32_t) (now - time_ctx.start_ms));
}
