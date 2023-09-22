/*
 * zvm.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "zvm.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** ZVM local macros ***/

#define ZVM_FADE_DURATION_MS	1000
#define ZVM_FADE_MARGIN_MS		100

/*** ZVM local structures ***/

/*******************************************************************/
typedef enum {
	ZVM_INTERNAL_STATE_TURN_OFF,
	ZVM_INTERNAL_STATE_TURN_ON,
	ZVM_INTERNAL_STATE_ON_0,
	ZVM_INTERNAL_STATE_ON_1,
	ZVM_INTERNAL_STATE_LAST
} ZVM_internal_state;

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_turn_on;
	SOUND_context_t sound_on_0;
	SOUND_context_t sound_on_1;
	SOUND_context_t sound_turn_off;
	ZVM_state_t state;
	ZVM_internal_state internal_state;
} ZVM_Context;

/*** ZVM local global variables ***/

static ZVM_Context zvm_ctx;

/*** ZVM local functions ***/

/*******************************************************************/
ZVM_status_t _ZVM_sound_on(void) {
	// Local variables.
	ZVM_status_t status = ZVM_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Start playing turn-on sound.
	sound_status = SOUND_play(&(zvm_ctx.sound_turn_on), 0);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	// Stop all other sounds.
	sound_status = SOUND_stop(&(zvm_ctx.sound_turn_off), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_stop(&(zvm_ctx.sound_on_0), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_stop(&(zvm_ctx.sound_on_1), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
errors:
	return status;
}

/*******************************************************************/
ZVM_status_t _ZVM_sound_off(void) {
	// Local variables.
	ZVM_status_t status = ZVM_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Start playing turn-off sound.
	sound_status = SOUND_play(&(zvm_ctx.sound_turn_off), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	// Stop all other sounds.
	sound_status = SOUND_stop(&(zvm_ctx.sound_turn_on), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_stop(&(zvm_ctx.sound_on_0), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_stop(&(zvm_ctx.sound_on_1), ZVM_FADE_DURATION_MS);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
errors:
	return status;
}

/*** ZVM functions ***/

/*******************************************************************/
ZVM_status_t ZVM_init(void) {
	// Local variables.
	ZVM_status_t status = ZVM_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init internal_state machine.
	zvm_ctx.state = ZVM_STATE_LAST;
	zvm_ctx.internal_state = ZVM_INTERNAL_STATE_TURN_OFF;
	// Init sounds.
	sound_status = SOUND_init(&(zvm_ctx.sound_turn_on), "zvm_turn_on.wav", ZVM_AUDIO_GAIN);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zvm_ctx.sound_on_0), "zvm_on.wav", ZVM_AUDIO_GAIN);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zvm_ctx.sound_on_1), "zvm_on.wav", ZVM_AUDIO_GAIN);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zvm_ctx.sound_turn_off), "zvm_turn_off.wav", ZVM_AUDIO_GAIN);
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_ZVM
	LOG_STATUS(status, ZVM_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
ZVM_status_t ZVM_set_state(ZVM_state_t state) {
	// Local variables.
	ZVM_status_t status = ZVM_SUCCESS;
	// Check parameter.
	if (state >= ZVM_STATE_LAST) {
		status = ZVM_ERROR_STATE;
		goto errors;
	}
	// Print state.
	if (state == ZVM_STATE_ON) {
		LOG("state=ZVM_STATE_ON");
	}
	else {
		LOG("state=ZVM_STATE_OFF");
	}
	// Update context.
	zvm_ctx.state = state;
errors:
#ifdef LOG_ZVM
	LOG_STATUS(status, ZVM_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
ZVM_status_t ZVM_process(void) {
	// Local variables.
	ZVM_status_t status = ZVM_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Update sounds positions.
	sound_status = SOUND_update(&(zvm_ctx.sound_turn_on));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(zvm_ctx.sound_on_0));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(zvm_ctx.sound_on_1));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(zvm_ctx.sound_turn_off));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	// Perform internal internal_state machine.
	switch (zvm_ctx.internal_state) {
	case ZVM_INTERNAL_STATE_TURN_OFF:
		if (zvm_ctx.state == ZVM_STATE_ON) {
			// Play ZVM turn on sound.
			status = _ZVM_sound_on();
			if (status != ZVM_SUCCESS) goto errors;
			// Update state.
			zvm_ctx.internal_state = ZVM_INTERNAL_STATE_TURN_ON;
		}
		break;
	case ZVM_INTERNAL_STATE_TURN_ON:
		if (zvm_ctx.state == ZVM_STATE_OFF) {
			// Play ZVM turn off sound.
			status = _ZVM_sound_off();
			if (status != ZVM_SUCCESS) goto errors;
			// Update state.
			zvm_ctx.internal_state = ZVM_INTERNAL_STATE_TURN_OFF;
		}
		else {
			if ((zvm_ctx.sound_turn_on.position_ms) > (zvm_ctx.sound_turn_on.length_ms - ZVM_FADE_DURATION_MS - ZVM_FADE_MARGIN_MS)) {
				// Perform overlap.
				sound_status = SOUND_play(&(zvm_ctx.sound_on_0), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_turn_on), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_on_1), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_turn_off), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				// Update state.
				zvm_ctx.internal_state = ZVM_INTERNAL_STATE_ON_0;
			}
		}
		break;
	case ZVM_INTERNAL_STATE_ON_0:
		if (zvm_ctx.state == ZVM_STATE_OFF) {
			// Play ZVM turn off sound.
			status = _ZVM_sound_off();
			if (status != ZVM_SUCCESS) goto errors;
			// Update state.
			zvm_ctx.internal_state = ZVM_INTERNAL_STATE_TURN_OFF;
		}
		else {
			if ((zvm_ctx.sound_on_0.position_ms) > (zvm_ctx.sound_on_0.length_ms - ZVM_FADE_DURATION_MS - ZVM_FADE_MARGIN_MS)) {
				// Perform overlap.
				sound_status = SOUND_play(&(zvm_ctx.sound_on_1), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_on_0), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_turn_on), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_turn_off), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				// Update state.
				zvm_ctx.internal_state = ZVM_INTERNAL_STATE_ON_1;
			}
		}
		break;
	case ZVM_INTERNAL_STATE_ON_1:
		if (zvm_ctx.state == ZVM_STATE_OFF) {
			// Play ZVM turn off sound.
			status = _ZVM_sound_off();
			if (status != ZVM_SUCCESS) goto errors;
			// Update state.
			zvm_ctx.internal_state = ZVM_INTERNAL_STATE_TURN_OFF;
		}
		else {
			if ((zvm_ctx.sound_on_1.position_ms) > (zvm_ctx.sound_on_1.length_ms - ZVM_FADE_DURATION_MS - ZVM_FADE_MARGIN_MS)) {
				// Perform overlap.
				sound_status = SOUND_play(&(zvm_ctx.sound_on_0), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_on_1), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_turn_on), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(zvm_ctx.sound_turn_off), ZVM_FADE_DURATION_MS);
				SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
				// Update state.
				zvm_ctx.internal_state = ZVM_INTERNAL_STATE_ON_0;
			}
		}
		break;
	default:
		// Unknown state.
		status = ZVM_ERROR_INTERNAL_STATE;
		goto errors;
	}
	// Process sounds.
	sound_status = SOUND_process(&(zvm_ctx.sound_turn_on));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zvm_ctx.sound_on_0));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zvm_ctx.sound_on_1));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zvm_ctx.sound_turn_off));
	SOUND_stack_exit_error(ZVM_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_ZVM
	LOG_STATUS(status, ZVM_SUCCESS, "OK");
#endif
	return status;
}
