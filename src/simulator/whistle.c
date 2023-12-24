/*
 * whistle.c
 *
 *  Created on: 10 may 2020
 *      Author: Ludo
 */

#include "whistle.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** WHISTLE local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_low_tone;
	SOUND_context_t sound_low_tone_end;
	SOUND_context_t sound_high_tone;
	SOUND_context_t sound_high_tone_end;
	WHISTLE_state_t state;
} WHISTLE_Context;

/*** WHISTLE local global variables ***/

static WHISTLE_Context whistle_ctx;

/*** WHISTLE functions ***/

/*******************************************************************/
WHISTLE_status_t WHISTLE_init(void) {
	// Local variables.
	WHISTLE_status_t status = WHISTLE_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init context.
	whistle_ctx.state = WHISTLE_STATE_LAST;
	// Init sounds.
	sound_status = SOUND_init(&(whistle_ctx.sound_low_tone), "whistle_low_tone.wav", WHISTLE_AUDIO_GAIN);
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(whistle_ctx.sound_low_tone_end), "whistle_low_tone_end.wav", WHISTLE_AUDIO_GAIN);
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(whistle_ctx.sound_high_tone), "whistle_high_tone.wav", WHISTLE_AUDIO_GAIN);
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(whistle_ctx.sound_high_tone_end), "whistle_high_tone_end.wav", WHISTLE_AUDIO_GAIN);
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	// Init context.
	whistle_ctx.state = WHISTLE_STATE_NEUTRAL;
errors:
#ifdef LOG_WHISTLE
	LOG_STATUS(status, WHISTLE_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
WHISTLE_status_t WHISTLE_set_state(WHISTLE_state_t state) {
	// Local variables.
	WHISTLE_status_t status = WHISTLE_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Check state.
	switch (state) {
	case WHISTLE_STATE_HIGH_TONE:
		// Check state change.
		if (whistle_ctx.state != WHISTLE_STATE_HIGH_TONE) {
			// Log action.
			LOG("state=WHISTLE_STATE_HIGH_TONE");
			// Play sound.
			sound_status = SOUND_play(&(whistle_ctx.sound_high_tone), 0);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			// Stop all other sounds.
			sound_status = SOUND_stop(&(whistle_ctx.sound_low_tone), 50);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(whistle_ctx.sound_low_tone_end), 50);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(whistle_ctx.sound_high_tone_end), 50);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
		}
		break;
	case WHISTLE_STATE_NEUTRAL:
		// Check state change.
		if (whistle_ctx.state != WHISTLE_STATE_NEUTRAL) {
			// Log action.
			LOG("state=WHISTLE_STATE_NEUTRAL");
			// Check previous state.
			if (whistle_ctx.state == WHISTLE_STATE_LOW_TONE) {
				// End low tone.
				sound_status = SOUND_play(&(whistle_ctx.sound_low_tone_end), 0);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
				// Stop all other sounds.
				sound_status = SOUND_stop(&(whistle_ctx.sound_low_tone), 50);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(whistle_ctx.sound_high_tone), 50);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(whistle_ctx.sound_high_tone_end), 50);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			}
			if (whistle_ctx.state == WHISTLE_STATE_HIGH_TONE) {
				// End high tone.
				sound_status = SOUND_play(&(whistle_ctx.sound_high_tone_end), 0);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
				// Stop all other sounds.
				sound_status = SOUND_stop(&(whistle_ctx.sound_low_tone), 50);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(whistle_ctx.sound_low_tone_end), 50);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(whistle_ctx.sound_high_tone), 50);
				SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			}
		}
		break;
	case WHISTLE_STATE_LOW_TONE:
		// Check state change.
		if (whistle_ctx.state != WHISTLE_STATE_LOW_TONE) {
			// Log action.
			LOG("state=WHISTLE_STATE_LOW_TONE");
			// Play sound.
			sound_status = SOUND_play(&(whistle_ctx.sound_low_tone), 0);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			// Stop all other sounds.
			sound_status = SOUND_stop(&(whistle_ctx.sound_low_tone_end), 50);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(whistle_ctx.sound_high_tone), 50);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(whistle_ctx.sound_high_tone_end), 50);
			SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
		}
		break;
	default:
		status = WHISTLE_ERROR_STATE;
		goto errors;
	}
	// Update state.
	whistle_ctx.state = state;
errors:
#ifdef LOG_WHISTLE
	LOG_STATUS(status, WHISTLE_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
WHISTLE_status_t WHISTLE_process(void) {
	// Local variables.
	WHISTLE_status_t status = WHISTLE_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Process sounds.
	sound_status = SOUND_process(&(whistle_ctx.sound_low_tone));
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(whistle_ctx.sound_low_tone_end));
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(whistle_ctx.sound_high_tone));
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(whistle_ctx.sound_high_tone_end));
	SOUND_stack_exit_error(WHISTLE_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_WHISTLE
	LOG_STATUS(status, WHISTLE_SUCCESS, "OK");
#endif
	return status;
}
