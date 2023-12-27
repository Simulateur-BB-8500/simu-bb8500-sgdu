/*
 * emergency.c
 *
 *  Created on: 26 dec. 2023
 *      Author: Ludo
 */

#include "emergency.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** EMERGENCY local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_turn_on;
	EMERGENCY_state_t state;
} EMERGENCY_context_t;

/*** EMERGENCY local global variables ***/

static EMERGENCY_context_t emergency_ctx;

/*** EMERGENCY functions ***/

/*******************************************************************/
EMERGENCY_status_t EMERGENCY_init(void) {
	// Local variables.
	EMERGENCY_status_t status = EMERGENCY_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init context.
	emergency_ctx.state = EMERGENCY_STATE_LAST;
	// Init sounds.
	sound_status = SOUND_init(&(emergency_ctx.sound_turn_on), "pbl2_turn_off.wav", EMERGENCY_AUDIO_GAIN);
	SOUND_stack_exit_error(EMERGENCY_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_EMERGENCY
	LOG_STATUS(status, EMERGENCY_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
EMERGENCY_status_t EMERGENCY_set_state(EMERGENCY_state_t state) {
	// Local variables.
	EMERGENCY_status_t status = EMERGENCY_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check state.
	switch (state) {
	case EMERGENCY_STATE_ON:
		// Check state change.
		if (emergency_ctx.state != EMERGENCY_STATE_ON) {
			// Log action.
			LOG("state=EMERGENCY_STATE_ON");
			// Play sound.
			sound_status = SOUND_play(&(emergency_ctx.sound_turn_on), 0);
			SOUND_stack_exit_error(EMERGENCY_ERROR_DRIVER_SOUND);
			sound_status = SOUND_process(&(emergency_ctx.sound_turn_on));
			SOUND_stack_exit_error(EMERGENCY_ERROR_DRIVER_SOUND);
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_BPURG, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(EMERGENCY_ERROR_DRIVER_KEYBOARD);
		}
		break;
	case EMERGENCY_STATE_OFF:
		// Check state change.
		if (emergency_ctx.state != EMERGENCY_STATE_OFF) {
			// Log action.
			LOG("state=EMERGENCY_STATE_OFF");
			// Press OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_BPURG, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(EMERGENCY_ERROR_DRIVER_KEYBOARD);
		}
		break;
	default:
		status = EMERGENCY_ERROR_STATE;
		goto errors;
	}
	// Update local state.
	emergency_ctx.state = state;
errors:
#ifdef LOG_EMERGENCY
	LOG_STATUS(status, EMERGENCY_SUCCESS, "OK");
#endif
	return status;
}
