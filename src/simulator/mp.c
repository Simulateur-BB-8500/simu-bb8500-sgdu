/*
 * mp.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "mp.h"

#include "error.h"
#include "keyboard.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** MP local macros ***/

#define MP_NUMBER_OF_VARIATOR_SOUNDS	10
#define MP_LOG

/*** MP local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_variator[MP_NUMBER_OF_VARIATOR_SOUNDS];
	SOUND_context_t sound_release;
	uint8_t variator_step_count;
} MP_context_t;

/*** MP local global variables ***/

static MP_context_t mp_ctx;

/*** MP functions ***/

/*******************************************************************/
MP_status_t MP_init(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	uint8_t idx = 0;
	// Init context.
	mp_ctx.variator_step_count = 0;
	// Init sounds.
	for (idx=0 ; idx<MP_NUMBER_OF_VARIATOR_SOUNDS ; idx++) {
		sound_status = SOUND_init(&(mp_ctx.sound_variator[idx]), "mp_variator.wav", MP_AUDIO_GAIN);
		SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
	}
errors:
	return status;
}

/*******************************************************************/
MP_status_t MP_set_event(MP_event_t event) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check event.
	switch (event) {
	case MP_EVENT_0:
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_0, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
		break;
	case MP_EVENT_T_MORE:
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_T_MORE, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
		KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
		// Increase step count.
		mp_ctx.variator_step_count++;
		break;
	case MP_EVENT_T_LESS:
		// Check step count.
		if (mp_ctx.variator_step_count > 0) {
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_T_LESS, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
			KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
			// Decrease step count.
			mp_ctx.variator_step_count--;
		}
		break;
	case MP_EVENT_PR:
		// TODO
		break;
	case MP_EVENT_P:
		// TODO
		break;
	case MP_EVENT_F_MORE:
		// TODO
		break;
	case MP_EVENT_F_LESS:
		// TODO
		break;
	case MP_EVENT_FR:
		// TODO
		break;
	default:
		status = MP_ERROR_EVENT;
		goto errors;
	}
errors:
	return status;
}
