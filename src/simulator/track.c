/*
 * track.c
 *
 *  Created on: 22 sep. 2023
 *      Author: Ludo
 */

#include "track.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "sound.h"
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

/*** TRACK local macros ***/

#define TRACK_FADE_ON_OFF_DURATION_MS		1000
#define TRACK_FADE_DURATION_MS				10000
#define TRACK_FADE_MARGIN_MS				100

#define TRACK_SPEED_RANGE_NUMBER			5
#define TRACK_SPEED_RANGE_KMH				40

#define TRACK_STOP_SPEED_HISTORY_SIZE		5
#define TRACK_STOP_SPEED_THRESHOLD_KMH		8
#define TRACK_STOP_VARIATION_THRESHOLD_KMH	(-2)
#define TRACK_STOP_FADE_DURATION_MS			1000

/*** TRACK local structures ***/

/*******************************************************************/
typedef enum {
	TRACK_INTERNAL_STATE_OFF = 0,
	TRACK_INTERNAL_STATE_SOUND_0,
	TRACK_INTERNAL_STATE_SOUND_1,
	TRACK_INTERNAL_STATE_LAST
} TRACK_internal_state_t;

/*******************************************************************/
typedef struct {
	TRACK_internal_state_t state;
	uint8_t start_threshold_kmh;
	uint8_t stop_threshold_kmh;
	uint8_t median_speed_kmh;
	SOUND_context_t sound_0;
	SOUND_context_t sound_1;
} TRACK_speed_range_t;

/*******************************************************************/
typedef struct {
	uint8_t speed_kmh;
	uint8_t speed_history_kmh[TRACK_STOP_SPEED_HISTORY_SIZE];
	uint8_t speed_history_idx;
	TRACK_speed_range_t speed_range[TRACK_SPEED_RANGE_NUMBER];
	SOUND_context_t sound_stop;
	uint8_t sound_stop_enable;
} TRACK_Context;

/*** TRACK local global variables ***/

static TRACK_Context track_ctx;

/*** TRACK functions ***/

/*******************************************************************/
TRACK_status_t TRACK_init(void) {
	// Local variables.
	TRACK_status_t status = TRACK_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	const char audio_file_name_base[] = "track_speed_range_x.wav";
	char audio_file_name[] = "track_speed_range_x.wav";
	char range_letter = 0;
	char* range_ptr = NULL;
	uint8_t idx = 0;
	// Init context.
	track_ctx.speed_kmh = 0;
	for (idx=0 ; idx<TRACK_STOP_SPEED_HISTORY_SIZE ; idx++) track_ctx.speed_history_kmh[idx] = 0;
	track_ctx.speed_history_idx = 0;
	track_ctx.sound_stop_enable = 0;
	// Init speed ranges.
	for (idx=0 ; idx<TRACK_SPEED_RANGE_NUMBER ; idx++) {
		// Init state.
		track_ctx.speed_range[idx].state = TRACK_INTERNAL_STATE_OFF;
		// Init thresholds and median speed.
		track_ctx.speed_range[idx].start_threshold_kmh = ((TRACK_SPEED_RANGE_KMH / 2) * idx);
		track_ctx.speed_range[idx].stop_threshold_kmh = (track_ctx.speed_range[idx].start_threshold_kmh + TRACK_SPEED_RANGE_KMH);
		track_ctx.speed_range[idx].median_speed_kmh = (uint8_t) (((uint32_t) track_ctx.speed_range[idx].start_threshold_kmh + (uint32_t) track_ctx.speed_range[idx].stop_threshold_kmh) / (2));
		// Copy base name.
		strcpy(audio_file_name, audio_file_name_base);
		// Build audio file name.
		range_letter = ('a' + idx);
		range_ptr = strchr(audio_file_name, 'x');
		if (range_ptr != NULL) {
			(*range_ptr) = range_letter;
		}
		else {
			continue;
		}
		// Init sound.
		sound_status = SOUND_init(&(track_ctx.speed_range[idx].sound_0), audio_file_name, TRACK_AUDIO_GAIN);
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		sound_status = SOUND_init(&(track_ctx.speed_range[idx].sound_1), audio_file_name, TRACK_AUDIO_GAIN);
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
	}
	sound_status = SOUND_init(&(track_ctx.sound_stop), "track_stop.wav", (TRACK_AUDIO_GAIN / 3.0));
	SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_TRACK
	LOG_STATUS(status, TRACK_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
TRACK_status_t TRACK_set_speed(uint8_t speed_kmh) {
	// Local variables.
	TRACK_status_t status = TRACK_SUCCESS;
	// Update local context.
	track_ctx.speed_kmh = speed_kmh;
	// Add new sample in history.
	track_ctx.speed_history_kmh[track_ctx.speed_history_idx] = speed_kmh;
	track_ctx.speed_history_idx = (track_ctx.speed_history_idx + 1) % TRACK_STOP_SPEED_HISTORY_SIZE;
#ifdef LOG_TRACK
	LOG_STATUS(status, TRACK_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
TRACK_status_t TRACK_process(void) {
	// Local variables.
	TRACK_status_t status = TRACK_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	uint8_t start = 0;
	uint8_t median = 0;
	uint8_t stop = 0;
	float gain = 0.0;
	uint8_t idx = 0;
	uint8_t speed_in_range_flag = 0;
	int32_t speed_variation_kmh = 0;
	// Ranges loop.
	for (idx=0 ; idx<TRACK_SPEED_RANGE_NUMBER ; idx++) {
		// Update sounds.
		sound_status = SOUND_update(&(track_ctx.speed_range[idx].sound_0));
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		sound_status = SOUND_update(&(track_ctx.speed_range[idx].sound_1));
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		// Copy local variables for readability.
		start = track_ctx.speed_range[idx].start_threshold_kmh;
		median = track_ctx.speed_range[idx].median_speed_kmh;
		stop = track_ctx.speed_range[idx].stop_threshold_kmh;
		// Compute gain according to speed.
		if ((track_ctx.speed_kmh > start) && (track_ctx.speed_kmh < stop)) {
			// Compute triangular equation.
			if (track_ctx.speed_kmh < median) {
				gain = SOUND_AUDIO_VOLUME_MIN + (((SOUND_AUDIO_VOLUME_MAX - SOUND_AUDIO_VOLUME_MIN) * ((float) track_ctx.speed_kmh - (float) start)) / ((float) median - (float) start));
			}
			else {
				if (idx == (TRACK_SPEED_RANGE_NUMBER - 1)) {
					gain = SOUND_AUDIO_VOLUME_MAX;
				}
				else {
					gain = SOUND_AUDIO_VOLUME_MIN + (((SOUND_AUDIO_VOLUME_MAX - SOUND_AUDIO_VOLUME_MIN) * ((float) stop - (float) track_ctx.speed_kmh)) / ((float) stop - (float) median));
				}

			}
			speed_in_range_flag = 1;
		}
		else {
			// Speed out of range.
			gain = SOUND_AUDIO_VOLUME_MIN;
			speed_in_range_flag = 0;
		}
		// Perform state machine.
		switch (track_ctx.speed_range[idx].state) {
		case TRACK_INTERNAL_STATE_OFF:
			// Check speed.
			if (speed_in_range_flag != 0) {
				// Start sounds.
				sound_status = SOUND_play(&(track_ctx.speed_range[idx].sound_0), TRACK_FADE_ON_OFF_DURATION_MS);
				SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_1), TRACK_FADE_ON_OFF_DURATION_MS);
				SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
				// Update state.
				track_ctx.speed_range[idx].state = TRACK_INTERNAL_STATE_SOUND_0;
			}
			break;
		case TRACK_INTERNAL_STATE_SOUND_0:
			// Check speed.
			if (speed_in_range_flag == 0) {
				// Stop sounds.
				sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_0), TRACK_FADE_ON_OFF_DURATION_MS);
				SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_1), TRACK_FADE_ON_OFF_DURATION_MS);
				SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
				// Update state.
				track_ctx.speed_range[idx].state = TRACK_INTERNAL_STATE_OFF;
			}
			else {
				// Check sound position.
				if (track_ctx.speed_range[idx].sound_0.position_ms > (track_ctx.speed_range[idx].sound_0.length_ms - TRACK_FADE_DURATION_MS - TRACK_FADE_MARGIN_MS)) {
					// Perform overlap.
					sound_status = SOUND_play(&(track_ctx.speed_range[idx].sound_1), TRACK_FADE_DURATION_MS);
					SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
					sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_0), TRACK_FADE_DURATION_MS);
					SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
					// Update state.
					track_ctx.speed_range[idx].state = TRACK_INTERNAL_STATE_SOUND_1;
				}
			}
			break;
		case TRACK_INTERNAL_STATE_SOUND_1:
			// Check speed.
			if (speed_in_range_flag == 0) {
				// Stop sounds.
				sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_0), TRACK_FADE_ON_OFF_DURATION_MS);
				SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_1), TRACK_FADE_ON_OFF_DURATION_MS);
				SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
				// Update state.
				track_ctx.speed_range[idx].state = TRACK_INTERNAL_STATE_OFF;
			}
			else {
				// Check sound position.
				if (track_ctx.speed_range[idx].sound_1.position_ms > (track_ctx.speed_range[idx].sound_1.length_ms - TRACK_FADE_DURATION_MS - TRACK_FADE_MARGIN_MS)) {
					// Perform overlap.
					sound_status = SOUND_play(&(track_ctx.speed_range[idx].sound_0), TRACK_FADE_DURATION_MS);
					SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
					sound_status = SOUND_stop(&(track_ctx.speed_range[idx].sound_1), TRACK_FADE_DURATION_MS);
					SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
					// Update state.
					track_ctx.speed_range[idx].state = TRACK_INTERNAL_STATE_SOUND_0;
				}
			}
			break;
		default:
			break;
		}
		// Set volumes.
		sound_status = SOUND_set_gain(&(track_ctx.speed_range[idx].sound_0), gain);
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		sound_status = SOUND_set_gain(&(track_ctx.speed_range[idx].sound_1), gain);
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		// Process sounds.
		sound_status = SOUND_process(&(track_ctx.speed_range[idx].sound_0));
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		sound_status = SOUND_process(&(track_ctx.speed_range[idx].sound_1));
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
	}
	// Compute speed variation.
	idx = (track_ctx.speed_history_idx == 0) ? (TRACK_STOP_SPEED_HISTORY_SIZE - 1) : (track_ctx.speed_history_idx - 1);
	speed_variation_kmh = ((int32_t) track_ctx.speed_history_kmh[idx] - (int32_t) track_ctx.speed_history_kmh[track_ctx.speed_history_idx]);
	// Manage stop sound.
	if (track_ctx.speed_kmh == 0) {
		// Stop sound when train stops.
		sound_status = SOUND_stop(&(track_ctx.sound_stop), TRACK_STOP_FADE_DURATION_MS);
		SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
		// Reset flag.
		track_ctx.sound_stop_enable = 0;
	}
	else {
		// Check speed variation.
		if ((track_ctx.speed_kmh < TRACK_STOP_SPEED_THRESHOLD_KMH) && (speed_variation_kmh <= TRACK_STOP_VARIATION_THRESHOLD_KMH) && (track_ctx.sound_stop_enable != 0)) {
			// Play sound.
			sound_status = SOUND_play(&(track_ctx.sound_stop), TRACK_STOP_FADE_DURATION_MS);
			SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
			// Reset flag.
			track_ctx.sound_stop_enable = 0;
		}
		if (speed_variation_kmh >= 0) {
			// Stop sound when speed in stable.
			sound_status = SOUND_stop(&(track_ctx.sound_stop), TRACK_STOP_FADE_DURATION_MS);
			SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
			// Enable replay.
			track_ctx.sound_stop_enable = 1;
		}
	}
	sound_status = SOUND_process(&(track_ctx.sound_stop));
	SOUND_stack_exit_error(TRACK_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_TRACK
	LOG_STATUS(status, TRACK_SUCCESS, "OK");
#endif
	return status;
}
