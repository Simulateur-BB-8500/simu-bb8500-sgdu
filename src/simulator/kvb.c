/*
 * kvb.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "kvb.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** KVB local macros ***/

#define KVB_LOG

/*** KVB local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_on;
	SOUND_context_t sound_off;
	SOUND_context_t sound_urgency;
} KVB_context_t;

/*** KVB local global variables ***/

static KVB_context_t kvb_ctx;

/*** KVB functions ***/

/*******************************************************************/
KVB_status_t KVB_init(void) {
	// Local variables.
	KVB_status_t status = KVB_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init sounds.
	sound_status = SOUND_init(&(kvb_ctx.sound_on), "kvb_turn_on.wav", KVB_AUDIO_GAIN);
	SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
//	sound_status = SOUND_init(&(kvb_ctx.sound_off), "kvb_turn_off.wav", KVB_AUDIO_GAIN);
//	SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
//	sound_status = SOUND_init(&(kvb_ctx.sound_urgency), "kvb_urgency.wav", KVB_AUDIO_GAIN);
//	SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_KVB
	LOG_STATUS(status, KVB_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
KVB_status_t KVB_set_state(KVB_state_t state) {
	// Local variables.
	KVB_status_t status = KVB_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Check state.
	switch (state) {
	case KVB_STATE_ON:
		// Log action.
		LOG("state=KVB_STATE_ON");
		// Play and stop sounds.
		sound_status = SOUND_play(&(kvb_ctx.sound_on), 0);
		SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
//		sound_status = SOUND_stop(&(kvb_ctx.sound_off), 0);
//		SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
		break;
	case KVB_STATE_OFF:
		// Log action.
		LOG("state=KVB_STATE_OFF");
		// Play and stop sounds.
//		sound_status = SOUND_play(&(kvb_ctx.sound_off), 0);
//		SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
		sound_status = SOUND_stop(&(kvb_ctx.sound_on), 0);
		SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
		break;
	default:
		status = KVB_ERROR_STATE;
		goto errors;
	}
	// Process sounds.
	sound_status = SOUND_process(&(kvb_ctx.sound_on));
	SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
//	sound_status = SOUND_process(&(kvb_ctx.sound_off));
//	SOUND_stack_exit_error(KVB_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_KVB
	LOG_STATUS(status, KVB_SUCCESS, "OK");
#endif
	return status;
}
