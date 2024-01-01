/*
 * pbl2.c
 *
 *  Created on: 21 sep. 2023
 *      Author: Ludo
 */

#include "pbl2.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** PBL2 local macros ***/

#define PBL2_FADE_DURATION_MS	1000

/*** PBL2 local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_turn_on;
	SOUND_context_t sound_turn_off;
	PBL2_state_t state;
} PBL2_context_t;

/*** PBL2 local global variables ***/

static PBL2_context_t pbl2_ctx;

/*** PBL2 functions ***/

/*******************************************************************/
PBL2_status_t PBL2_init(void) {
	// Local variables.
	PBL2_status_t status = PBL2_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init context.
	pbl2_ctx.state = PBL2_STATE_LAST;
	// Init sounds.
	sound_status = SOUND_init(&(pbl2_ctx.sound_turn_on), "pbl2_turn_on.wav", PBL2_AUDIO_GAIN);
	SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(pbl2_ctx.sound_turn_off), "pbl2_turn_off.wav", (PBL2_AUDIO_GAIN / 3.0));
	SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
errors:
	LOG_ERROR(status, PBL2_SUCCESS);
	return status;
}

/*******************************************************************/
PBL2_status_t PBL2_set_state(PBL2_state_t state) {
	// Local variables.
	PBL2_status_t status = PBL2_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Check state.
	switch (state) {
	case PBL2_STATE_ON:
		// Check state change.
		if (pbl2_ctx.state != PBL2_STATE_ON) {
#ifdef LOG_PBL2
			LOG("state=PBL2_STATE_ON");
#endif
			// Play and stop sounds.
			sound_status = SOUND_play(&(pbl2_ctx.sound_turn_on), 0);
			SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(pbl2_ctx.sound_turn_off), PBL2_FADE_DURATION_MS);
			SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
		}
		break;
	case PBL2_STATE_OFF:
		// Check state change.
		if (pbl2_ctx.state != PBL2_STATE_OFF) {
#ifdef LOG_PBL2
			LOG("state=PBL2_STATE_OFF");
#endif
			// Play and stop sounds.
			sound_status = SOUND_play(&(pbl2_ctx.sound_turn_off), 0);
			SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(pbl2_ctx.sound_turn_on), PBL2_FADE_DURATION_MS);
			SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
		}
		break;
	default:
		status = PBL2_ERROR_STATE;
		goto errors;
	}
	// Update local state.
	pbl2_ctx.state = state;
errors:
	LOG_ERROR(status, PBL2_SUCCESS);
	return status;
}

/*******************************************************************/
PBL2_status_t PBL2_process(void) {
	// Local variables.
	PBL2_status_t status = PBL2_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Process sounds.
	sound_status = SOUND_process(&(pbl2_ctx.sound_turn_on));
	SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(pbl2_ctx.sound_turn_off));
	SOUND_stack_exit_error(PBL2_ERROR_DRIVER_SOUND);
errors:
	LOG_ERROR(status, PBL2_SUCCESS);
	return status;
}
