/*
 * fpb.c
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#include "fpb.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** FPB local macros ***/

#define FPB_FADE_DURATION_MS		2000

#define FPB_AUDIO_GAIN_MIN			0.2
#define FPB_AUDIO_GAIN_MAX			FPB_AUDIO_GAIN

#define FPB_SPEED_THRESHOLD_LOW		30
#define FPB_SPEED_THRESHOLD_HIGH	100

/*** FPB local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_apply;
	SOUND_context_t sound_release;
	FPB_state_t state;
} FPB_context_t;

/*** FPB local global variables ***/

static FPB_context_t fpb_ctx;

/*** FPB functions ***/

/*******************************************************************/
FPB_status_t FPB_init(void) {
	// Local variables.
	FPB_status_t status = FPB_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init state.
	fpb_ctx.state = FPB_STATE_LAST;
	// Init sounds.
	sound_status = SOUND_init(&(fpb_ctx.sound_apply), "fpb_apply.wav", FPB_AUDIO_GAIN_MIN);
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(fpb_ctx.sound_release), "fpb_release.wav", FPB_AUDIO_GAIN_MIN);
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_FPB
	LOG_STATUS(status, FPB_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
FPB_status_t FPB_set_state(FPB_state_t state) {
	// Local variables.
	FPB_status_t status = FPB_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check state.
	switch (state) {
	case FPB_STATE_APPLY:
		// Check state change.
		if (fpb_ctx.state != FPB_STATE_APPLY) {
			// Log action.
			LOG("state=FPB_STATE_APPLY");
			// Play and stop sounds.
			sound_status = SOUND_play(&(fpb_ctx.sound_apply), 0);
			SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(fpb_ctx.sound_release), FPB_FADE_DURATION_MS);
			SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_press(&ORTS_SHORTCUT_FPB_APPLY);
			KEYBOARD_stack_exit_error(FPB_ERROR_DRIVER_KEYBOARD);
		}
		break;
	case FPB_STATE_NEUTRAL:
		// Check state change.
		if (fpb_ctx.state != FPB_STATE_NEUTRAL) {
			// Log action.
			LOG("state=FPB_STATE_NEUTRAL");
			// Stop sound.
			sound_status = SOUND_stop(&(fpb_ctx.sound_release), FPB_FADE_DURATION_MS);
			SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
			// Check previous state.
			if (fpb_ctx.state == FPB_STATE_APPLY) {
				// Release shortcut.
				keyboard_status = KEYBOARD_release(&ORTS_SHORTCUT_FPB_APPLY);
				KEYBOARD_stack_exit_error(FPB_ERROR_DRIVER_KEYBOARD);
			}
			if (fpb_ctx.state == FPB_STATE_RELEASE) {
				// Release shortcut.
				keyboard_status = KEYBOARD_release(&ORTS_SHORTCUT_FPB_RELEASE);
				KEYBOARD_stack_exit_error(FPB_ERROR_DRIVER_KEYBOARD);
			}
		}
		break;
	case FPB_STATE_RELEASE:
		// Check state change.
		if (fpb_ctx.state != FPB_STATE_RELEASE) {
			// Log action.
			LOG("state=FPB_STATE_RELEASE");
			// Play and stop sounds.
			sound_status = SOUND_play(&(fpb_ctx.sound_release), 0);
			SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(fpb_ctx.sound_apply), FPB_FADE_DURATION_MS);
			SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_press(&ORTS_SHORTCUT_FPB_RELEASE);
			KEYBOARD_stack_exit_error(FPB_ERROR_DRIVER_KEYBOARD);
		}
		break;
	default:
		status = FPB_ERROR_STATE;
		goto errors;
	}
	// Update local state.
	fpb_ctx.state = state;
errors:
#ifdef LOG_FPB
	LOG_STATUS(status, FPB_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
FPB_status_t FPB_set_speed(uint8_t speed_kmh) {
	// Local variables.
	FPB_status_t status = FPB_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	float audio_gain = 0.0;
	// Compute audio gain according to train speed.
	if (speed_kmh < FPB_SPEED_THRESHOLD_LOW) {
		audio_gain = FPB_AUDIO_GAIN_MIN;
	}
	else if (speed_kmh < FPB_SPEED_THRESHOLD_HIGH) {
		audio_gain = (((FPB_AUDIO_GAIN_MAX - FPB_AUDIO_GAIN_MIN) * (speed_kmh - FPB_SPEED_THRESHOLD_LOW)) / (FPB_SPEED_THRESHOLD_HIGH - FPB_SPEED_THRESHOLD_LOW)) + FPB_AUDIO_GAIN_MIN;
	}
	else {
		audio_gain = FPB_AUDIO_GAIN_MAX;
	}
	// Update gain.
	sound_status = SOUND_set_gain(&(fpb_ctx.sound_apply), audio_gain);
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
	sound_status = SOUND_set_gain(&(fpb_ctx.sound_release), audio_gain);
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_FPB
	LOG_STATUS(status, FPB_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
FPB_status_t FPB_process(void) {
	// Local variables.
	FPB_status_t status = FPB_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Process sounds.
	sound_status = SOUND_process(&(fpb_ctx.sound_apply));
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(fpb_ctx.sound_release));
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_FPB
	LOG_STATUS(status, FPB_SUCCESS, "OK");
#endif
	return status;
}
