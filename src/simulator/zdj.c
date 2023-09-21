/*
 * zdj.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "zdj.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** ZDJ local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_open;
	SOUND_context_t sound_lock;
} ZDJ_context_t;

/*** ZDJ local global variables ***/

static ZDJ_context_t zdj_ctx;

/*** ZDJ functions ***/

/*******************************************************************/
ZDJ_status_t ZDJ_init(void) {
	// Local variables.
	ZDJ_status_t status = ZDJ_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init sound.
	sound_status = SOUND_init(&(zdj_ctx.sound_open), "zdj_open.wav", ZDJ_AUDIO_GAIN);
	SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zdj_ctx.sound_lock), "zdj_lock.wav", ZDJ_AUDIO_GAIN);
	SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_ZDJ
	LOG_STATUS(status, ZDJ_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
ZDJ_status_t ZDJ_set_state(ZDJ_state_t state) {
	// Local variables.
	ZDJ_status_t status = ZDJ_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Check state.
	switch (state) {
	case ZDJ_STATE_OPEN:
		// Log action.
		LOG("state=ZDJ_STATE_OPEN");
		// Play and stop sounds.
		sound_status = SOUND_play(&(zdj_ctx.sound_open), 0);
		SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
		sound_status = SOUND_stop(&(zdj_ctx.sound_lock), 0);
		SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
		break;
	case ZDJ_STATE_LOCK:
		// Log action.
		LOG("state=ZDJ_STATE_LOCK");
		// Play and stop sounds.
		sound_status = SOUND_play(&(zdj_ctx.sound_lock), 0);
		SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
		sound_status = SOUND_stop(&(zdj_ctx.sound_open), 0);
		SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
		break;
	default:
		status = ZDJ_ERROR_STATE;
		goto errors;
	}
	// Process sounds.
	sound_status = SOUND_process(&(zdj_ctx.sound_open));
	SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zdj_ctx.sound_lock));
	SOUND_stack_exit_error(ZDJ_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_ZDJ
	LOG_STATUS(status, ZDJ_SUCCESS, "OK");
#endif
	return status;
}
