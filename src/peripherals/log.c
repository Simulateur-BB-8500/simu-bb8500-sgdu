/*
 * log.c
 *
 *  Created on: 03 may 2024
 *      Author: Ludo
 */

#include "log.h"

#include "stdint.h"
#include "stdio.h"
#include "time.h"
#include "windows.h"

/*** LOG local global variables ***/

static const WORD LOG_COLOR_CODE[LOG_COLOR_LAST] = {4, 10, 3, 6, 7};

/*** LOG local functions ***/

/*******************************************************************/
static void _LOG_set_color(LOG_color_t color) {
	// Clamp parameter.
	if (color >= LOG_COLOR_LAST) {
		color = LOG_COLOR_WHITE;
	}
	// Set console color.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LOG_COLOR_CODE[color]);
}

/*** LOG functions ***/

/*******************************************************************/
void LOG_print(LOG_color_t color, const char* format, ...) {
	// Local variables.
	va_list args;
	va_start(args, format);
	// Print message.
	_LOG_set_color(color);
	vprintf(format, args);
	fflush(stdout);
}

/*******************************************************************/
void LOG_print_system_time(void) {
	// Local variables.
	TIME_system_t system_time;
	// Print timestamp.
	TIME_get_system_time(&system_time);
	LOG_print(LOG_COLOR_WHITE, "%02d:%02d:%02d:%03d ", system_time.hours, system_time.minutes, system_time.seconds, system_time.milliseconds);
}
