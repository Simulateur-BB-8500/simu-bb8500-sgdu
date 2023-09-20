/*
 * fpb.c
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#include "fpb.h"

#include "error.h"
#include "keyboard.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** FPB local macros ***/

#define FPB_FADE_DURATION_MS	500
#define FPB_LOG

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
	// Init sounds.
	sound_status = SOUND_init(&(fpb_ctx.sound_apply), "fpb_apply.wav", FPB_AUDIO_GAIN);
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(fpb_ctx.sound_release), "fpb_release.wav", FPB_AUDIO_GAIN);
	SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
errors:
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
			// Play sound.
			sound_status = SOUND_play(&(fpb_ctx.sound_apply), 0);
			SOUND_stack_exit_error(FPB_ERROR_DRIVER_SOUND);
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_press(&ORTS_SHORTCUT_FPB_APPLY);
			KEYBOARD_stack_exit_error(FPB_ERROR_DRIVER_KEYBOARD);
		}
		break;
	case FPB_STATE_NEUTRAL:
		// Check state change.
		if (fpb_ctx.state != FPB_STATE_APPLY) {
			// Stop sound.
			sound_status = SOUND_stop(&(fpb_ctx.sound_apply), FPB_FADE_DURATION_MS);
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
			// Play sound.
			sound_status = SOUND_play(&(fpb_ctx.sound_release), 0);
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
	return status;
}
