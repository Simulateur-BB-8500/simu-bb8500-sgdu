/*
 * log.c
 *
 *  Created on: 29 mar. 2020
 *      Author: Ludo
 */

#include "log.h"

#include "file.h"
#include "keyboard.h"
#include "lsmcu.h"
#include "openrails.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

/*** LOG local macros ***/

#define LOG_PERIOD_MS			500
#define LOG_SPEED_MAX_LENGTH	10
#define LOG_SPEED_ERROR			0xFF
//#define LOG_DEBUG

/*** LOG local structures ***/

typedef struct {
	FILE* file;
	uint8_t enable;
	uint64_t next_time;
} LOG_context_t;

/*** LOG local global variables ***/

static LOG_context_t log_ctx;

/*** LOG local functions ***/

uint32_t LOG_GetSpeed() {
	// Local variables.
	char log_line[FILE_LINE_MAX_LENGTH];
	char speed_string[LOG_SPEED_MAX_LENGTH];
	char* log_line_kmh;
	uint32_t index_kmh = 0;
	uint32_t char_idx = index_kmh = 0;
	uint32_t space_idx = 0;
	uint32_t speed_idx = 0;
	// Get last line.
	FILE_get_last_line(log_ctx.file, log_line);
#ifdef LOG_DEBUG
	printf("LOG *** Line = %s\n", log_line);
#endif

	uint32_t speed = LOG_SPEED_ERROR;
	// Search "km/h" in the last line.
	log_line_kmh = strstr(log_line, "km/h");
	if (log_line_kmh != ((void*) 0)) {
		index_kmh = (log_line_kmh - log_line);
#ifdef LOG_DEBUG
		printf("LOG *** Index km/h = %d\n", index_kmh);
#endif
		// Search previous space.
		char_idx = index_kmh;
		space_idx = 0;
		for (; char_idx>0; char_idx--) {
			if (log_line[char_idx] == ' ') {
				space_idx = char_idx;
				break;
			}
		}
#ifdef LOG_DEBUG
		printf("LOG *** Previous space index = %d\n", space_idx);
#endif
		// Extract speed.
		speed_idx = 0;
		for (char_idx=(space_idx+1); char_idx<index_kmh ; char_idx++) {
			// Stop reading if a coma or "km/h" is found.
			if ((log_line[char_idx] == ',') || (log_line[char_idx] == 'k')) {
				break;
			}
			else {
				speed_string[speed_idx] = log_line[char_idx];
				speed_idx++;
			}
		}
		// Fill remaining characters with '\0'.
		for (; speed_idx < LOG_SPEED_MAX_LENGTH; speed_idx++) {
			speed_string[speed_idx] = '\0';
		}
#ifdef LOG_DEBUG
		printf("LOG *** Speed string = <%s>\n", speed_string);
#endif
		// Parse speed.
		speed = atoi(speed_string);
#ifdef LOG_DEBUG
		printf("LOG *** Speed = %d\n", speed);
#endif
	}
	else {
#ifdef LOG_DEBUG
		printf("LOG *** String 'km/h' was not found.\n");
#endif
	}
	return speed;
}

/*** LOG functions ***/

/* INIT GAME LOG UNIT.
 * @param:	None.
 * @return:	None.
 */
void LOG_init(void) {
	// Open OpenRails log file.
	FILE_open(&log_ctx.file, "C:/Users/User/Desktop/OpenRailsLog.txt");
	log_ctx.next_time = 0;
	log_ctx.enable = 0;
}

/* START GAME LOG.
 * @param:	None.
 * @eturn:	None.
 */
void LOG_enable(void) {
	log_ctx.enable = 1;
}

/* STOP GAME LOG.
 * @param:	None.
 * @eturn:	None.
 */
void LOG_disable(void) {
	log_ctx.enable = 0;
}

/* MAIN TASK OF GAME LOG UNIT.
 * @param:	None.
 * @return:	None.
 */
void LOG_task(void) {
	// Local variables.
	uint32_t speed_kmh = 0;
	// Check enable bit and period.
	if ((log_ctx.enable != 0) && (TIME_get_ms() > log_ctx.next_time)) {
		// Update next time.
		log_ctx.next_time = TIME_get_ms() + LOG_PERIOD_MS;
		// Activate log.
		KEYBOARD_send(&OPENRAILS_LOG, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
		// Get speed.
		speed_kmh = LOG_GetSpeed();
		if (speed_kmh != LOG_SPEED_ERROR) {
#ifdef LOG_DEBUG
			printf("LOG *** Speed = %dkm/h\n", speed_kmh);
#endif
			// Transmit to dashboard.
			LSMCU_send(speed_kmh);
		}
#ifdef LOG_DEBUG
		else {
			printf("LOG *** Speed error\n");
		}
		fflush(stdout);
#endif
	}
}
