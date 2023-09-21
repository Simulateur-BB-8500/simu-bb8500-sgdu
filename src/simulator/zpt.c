/*
 * zpt.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "zpt.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** ZPT local macros ***/

#define ZPT_LOG

/*** ZPT local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_rear_up;
	SOUND_context_t sound_rear_down;
	ZPT_state_t rear_state;
	SOUND_context_t sound_front_up;
	SOUND_context_t sound_front_down;
	ZPT_state_t front_state;
} ZPT_context_t;

/*** ZPT local global variables ***/

static ZPT_context_t zpt_ctx;

/*** ZPT functions ***/

/*******************************************************************/
ZPT_status_t ZPT_init(void) {
	// Local variables.
	ZPT_status_t status = ZPT_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Init context.
	zpt_ctx.rear_state = ZPT_STATE_LAST;
	zpt_ctx.front_state = ZPT_STATE_LAST;
	// Init sounds.
	sound_status = SOUND_init(&(zpt_ctx.sound_rear_up), "zpt_up.wav", ZPT_REAR_AUDIO_GAIN);
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zpt_ctx.sound_rear_down), "zpt_down.wav", ZPT_REAR_AUDIO_GAIN);
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zpt_ctx.sound_front_up), "zpt_up.wav", ZPT_FRONT_AUDIO_GAIN);
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(zpt_ctx.sound_front_down), "zpt_down.wav", ZPT_FRONT_AUDIO_GAIN);
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_ZPT
	LOG_STATUS(status, ZPT_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
ZPT_status_t ZPT_set_position(ZPT_pantograph_t pantograph, ZPT_state_t state) {
	// Local variables.
	ZPT_status_t status = ZPT_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check parameters.
	if (state >= ZPT_STATE_LAST) {
		status = ZPT_ERROR_STATE;
		goto errors;
	}
	// Check pantograph.
	switch (pantograph) {
	case ZPT_PANTOGRAPH_REAR:
		// Check state change.
		if ((zpt_ctx.rear_state != ZPT_STATE_UP) && (state == ZPT_STATE_UP)) {
			// Log action.
			LOG("pantograph=ZPT_PANTOGRAPH_REAR state=ZPT_STATE_UP");
			// Play and stop sounds.
			sound_status = SOUND_play(&(zpt_ctx.sound_rear_up), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(zpt_ctx.sound_rear_down), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			// Send OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_ZPT_BACK_TOGGLE, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(ZPT_ERROR_DRIVER_KEYBOARD);
		}
		if ((zpt_ctx.rear_state != ZPT_STATE_DOWN) && (state == ZPT_STATE_DOWN)) {
			// Log action.
			LOG("pantograph=ZPT_PANTOGRAPH_REAR state=ZPT_STATE_DOWN");
			// Play and stop sounds.
			sound_status = SOUND_play(&(zpt_ctx.sound_rear_down), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(zpt_ctx.sound_rear_up), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			// Send OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_ZPT_BACK_TOGGLE, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(ZPT_ERROR_DRIVER_KEYBOARD);
		}
		// Update local state.
		zpt_ctx.rear_state = state;
		break;
	case ZPT_PANTOGRAPH_FRONT:
		// Check state change.
		if ((zpt_ctx.front_state != ZPT_STATE_UP) && (state == ZPT_STATE_UP)) {
			// Log action.
			LOG("pantograph=ZPT_PANTOGRAPH_FRONT state=ZPT_STATE_UP");
			// Play and stop sounds.
			sound_status = SOUND_play(&(zpt_ctx.sound_front_up), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(zpt_ctx.sound_front_down), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			// Send OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_ZPT_FRONT_TOGGLE, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(ZPT_ERROR_DRIVER_KEYBOARD);
		}
		if ((zpt_ctx.front_state != ZPT_STATE_DOWN) && (state == ZPT_STATE_DOWN)) {
			// Log action.
			LOG("pantograph=ZPT_PANTOGRAPH_FRONT state=ZPT_STATE_DOWN");
			// Play and stop sounds.
			sound_status = SOUND_play(&(zpt_ctx.sound_front_down), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(zpt_ctx.sound_front_up), 0);
			SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
			// Send OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_ZPT_FRONT_TOGGLE, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_stack_exit_error(ZPT_ERROR_DRIVER_KEYBOARD);
		}
		break;
	default:
		status = ZPT_ERROR_PANTOGRAPH;
		goto errors;
	}
	// Process sounds.
	sound_status = SOUND_process(&(zpt_ctx.sound_rear_up));
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zpt_ctx.sound_rear_down));
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zpt_ctx.sound_front_up));
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(zpt_ctx.sound_front_down));
	SOUND_stack_exit_error(ZPT_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_ZPT
	LOG_STATUS(status, ZPT_SUCCESS, "OK");
#endif
	return status;
}
