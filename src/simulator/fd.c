/*
 * fd.c
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#include "fd.h"

#include "error.h"
#include "keyboard.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** FD local macros ***/

#define FD_FADE_DURATION_MS		500
#define FD_LOG

/*** FD local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_apply;
	SOUND_context_t sound_release;
	FD_state_t state;
} FD_context_t;

/*** FD local global variables ***/

static FD_context_t fd_ctx;

/*** FD functions ***/

/*******************************************************************/
FD_status_t FD_init(void) {
	// Local variables.
	FD_status_t status = FD_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init sounds.
	sound_status = SOUND_init(&(fd_ctx.sound_apply), "fd_apply.wav", FD_AUDIO_GAIN);
	SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(fd_ctx.sound_release), "fd_release.wav", FD_AUDIO_GAIN);
	SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
errors:
	return status;
}

/*******************************************************************/
FD_status_t FD_set_state(FD_state_t state) {
	// Local variables.
	FD_status_t status = FD_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check state.
	switch (state) {
	case FD_STATE_APPLY:
		// Check state change.
		if (fd_ctx.state != FD_STATE_APPLY) {
			// Play sound.
			sound_status = SOUND_play(&(fd_ctx.sound_apply), 0);
			SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_press(&ORTS_SHORTCUT_FD_APPLY);
			KEYBOARD_stack_exit_error(FD_ERROR_DRIVER_KEYBOARD);
		}
		break;
	case FD_STATE_NEUTRAL:
		// Check state change.
		if (fd_ctx.state != FD_STATE_APPLY) {
			// Stop sound.
			sound_status = SOUND_stop(&(fd_ctx.sound_apply), FD_FADE_DURATION_MS);
			SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
			// Check previous state.
			if (fd_ctx.state == FD_STATE_APPLY) {
				// Release shortcut.
				keyboard_status = KEYBOARD_release(&ORTS_SHORTCUT_FD_APPLY);
				KEYBOARD_stack_exit_error(FD_ERROR_DRIVER_KEYBOARD);
			}
			if (fd_ctx.state == FD_STATE_RELEASE) {
				// Release shortcut.
				keyboard_status = KEYBOARD_release(&ORTS_SHORTCUT_FD_RELEASE);
				KEYBOARD_stack_exit_error(FD_ERROR_DRIVER_KEYBOARD);
			}
		}
		break;
	case FD_STATE_RELEASE:
		// Check state change.
		if (fd_ctx.state != FD_STATE_RELEASE) {
			// Play sound.
			sound_status = SOUND_play(&(fd_ctx.sound_release), 0);
			SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_press(&ORTS_SHORTCUT_FD_RELEASE);
			KEYBOARD_stack_exit_error(FD_ERROR_DRIVER_KEYBOARD);
		}
		break;
	default:
		status = FD_ERROR_STATE;
		goto errors;
	}
	// Update local state.
	fd_ctx.state = state;
errors:
	return status;
}

/*******************************************************************/
FD_status_t FD_process(void) {
	// Local variables.
	FD_status_t status = FD_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Process sounds.
	sound_status = SOUND_process(&(fd_ctx.sound_apply));
	SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(fd_ctx.sound_release));
	SOUND_stack_exit_error(FD_ERROR_DRIVER_SOUND);
errors:
	return status;
}
