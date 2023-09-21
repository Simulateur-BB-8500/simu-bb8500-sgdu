/*
 * mpinv.c
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#include "mpinv.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** MPINV local macros ***/

#define MPINV_LOG

/*** MPINV local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_neutral;
	SOUND_context_t sound_forward_backward;
	MPINV_position_t position;
} MPINV_context_t;

/*** MPINV local global variables ***/

static MPINV_context_t mpinv_ctx;

/*** MPINV functions ***/

/*******************************************************************/
MPINV_status_t MPINV_init(void) {
	// Local variables.
	MPINV_status_t status = MPINV_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init context.
	mpinv_ctx.position = MPINV_POSITION_LAST;
	// Init sounds.
	sound_status = SOUND_init(&(mpinv_ctx.sound_neutral), "mpinv_neutral.wav", MPINV_AUDIO_GAIN);
	SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(mpinv_ctx.sound_forward_backward), "mpinv_forward_backward.wav", MPINV_AUDIO_GAIN);
	SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_MPINV
	LOG_STATUS(status, MPINV_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
MPINV_status_t MPINV_set_position(MPINV_position_t position) {
	// Local variables.
	MPINV_status_t status = MPINV_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check position.
	switch (position) {
	case MPINV_POSITION_FORWARD:
			// Check state change.
			if (mpinv_ctx.position != MPINV_POSITION_FORWARD) {
				// Log action.
				LOG("position=MPINV_POSITION_FORWARD");
				// Play sound.
				sound_status = SOUND_play(&(mpinv_ctx.sound_forward_backward), 0);
				SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(mpinv_ctx.sound_neutral), 0);
				SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
				// Send OpenRails shortcuts.
				keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_FORWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
				KEYBOARD_stack_exit_error(MPINV_ERROR_DRIVER_KEYBOARD);
				keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_FORWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
				KEYBOARD_stack_exit_error(MPINV_ERROR_DRIVER_KEYBOARD);
			}
			break;
	case MPINV_POSITION_NEUTRAL:
		// Check state change.
		if (mpinv_ctx.position != MPINV_POSITION_NEUTRAL) {
			// Log action.
			LOG("position=MPINV_POSITION_NEUTRAL");
			// Play sound.
			sound_status = SOUND_play(&(mpinv_ctx.sound_neutral), 0);
			SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(mpinv_ctx.sound_forward_backward), 0);
			SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
			// Send OpenRails shortcuts.
			switch (mpinv_ctx.position) {
			case MPINV_POSITION_FORWARD:
				keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_BACKWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
				KEYBOARD_stack_exit_error(MPINV_ERROR_DRIVER_KEYBOARD);
				break;
			case MPINV_POSITION_BACKWARD:
				keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_FORWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
				KEYBOARD_stack_exit_error(MPINV_ERROR_DRIVER_KEYBOARD);
				break;
			default:
				status = MPINV_ERROR_POSITION;
				goto errors;
			}
		}
		break;
	case MPINV_POSITION_BACKWARD:
		// Check state change.
		if (mpinv_ctx.position != MPINV_POSITION_BACKWARD) {
			// Log action.
			LOG("position=MPINV_POSITION_BACKWARD");
			// Play and stop sound.
			sound_status = SOUND_play(&(mpinv_ctx.sound_forward_backward), 0);
			SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(mpinv_ctx.sound_neutral), 0);
			SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
			// Send OpenRails shortcuts.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_BACKWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(MPINV_ERROR_DRIVER_KEYBOARD);
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_BACKWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(MPINV_ERROR_DRIVER_KEYBOARD);
		}
		break;
	default:
		status = MPINV_ERROR_POSITION;
		goto errors;
	}
	// Update local position.
	mpinv_ctx.position = position;
	// Process sounds.
	sound_status = SOUND_process(&(mpinv_ctx.sound_neutral));
	SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(mpinv_ctx.sound_forward_backward));
	SOUND_stack_exit_error(MPINV_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_MPINV
	LOG_STATUS(status, MPINV_SUCCESS, "OK");
#endif
	return status;
}
