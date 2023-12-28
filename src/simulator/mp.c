/*
 * mp.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "mp.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** MP local macros ***/

#define MP_NUMBER_OF_VARIATOR_SOUNDS	10
#define MP_RANDOM_SEQUENCE_SIZE			5

/*** MP local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_variator[MP_NUMBER_OF_VARIATOR_SOUNDS];
	uint8_t sound_variator_lock_count[MP_NUMBER_OF_VARIATOR_SOUNDS];
	SOUND_context_t sound_variator_start;
	SOUND_context_t sound_variator_end;
	uint8_t variator_step_count;
} MP_context_t;

/*** MP local global variables ***/

static MP_context_t mp_ctx;

/*** MP local functions ***/

/*******************************************************************/
MP_status_t _MP_play_random_variator_sound(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	uint8_t random_idx = 0;
	uint8_t idx = 0;
	// Compute random index.
	do {
		random_idx = (rand() % MP_NUMBER_OF_VARIATOR_SOUNDS);
	}
	while (mp_ctx.sound_variator_lock_count[random_idx] > 0);
	// Lock current index.
	mp_ctx.sound_variator_lock_count[random_idx] = MP_RANDOM_SEQUENCE_SIZE;
	// Decrement count of all sounds.
	for (idx=0 ; idx<MP_NUMBER_OF_VARIATOR_SOUNDS ; idx++) {
		if (mp_ctx.sound_variator_lock_count[idx] > 0) {
			mp_ctx.sound_variator_lock_count[idx]--;
		}
	}
	// Play sound.
	LOG("random_idx=%d", random_idx);
	sound_status = SOUND_single_play(&(mp_ctx.sound_variator[random_idx]));
	SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
errors:
	return status;
}

/*** MP functions ***/

/*******************************************************************/
MP_status_t MP_init(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	const char audio_file_name_base[] = "mp_variator_x.wav";
	char audio_file_name[] = "mp_variator_x.wav";
	char range_letter = 0;
	char* range_ptr = NULL;
	uint8_t idx = 0;
	// Init context.
	mp_ctx.variator_step_count = 0;
	// Init sounds.
	for (idx=0 ; idx<MP_NUMBER_OF_VARIATOR_SOUNDS ; idx++) {
		// Copy base name.
		strcpy(audio_file_name, audio_file_name_base);
		// Build audio file name.
		range_letter = ('a' + idx);
		range_ptr = strchr(audio_file_name, 'x');
		if (range_ptr != NULL) {
			(*range_ptr) = range_letter;
		}
		else {
			continue;
		}
		// Init sound.
		sound_status = SOUND_init(&(mp_ctx.sound_variator[idx]), audio_file_name, MP_AUDIO_GAIN);
		SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		// Reset lock count.
		mp_ctx.sound_variator_lock_count[idx] = 0;
	}
	sound_status = SOUND_init(&(mp_ctx.sound_variator_start), "mp_variator_start.wav", MP_AUDIO_GAIN);
	SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(mp_ctx.sound_variator_end), "mp_variator_end.wav", MP_AUDIO_GAIN);
	SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
errors:
#ifdef LOG_MP
	LOG_STATUS(status, MP_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
MP_status_t MP_set_event(MP_event_t event) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check event.
	switch (event) {
	case MP_EVENT_0:
		// Log action.
		LOG("event=MP_EVENT_0");
		// Play sound.
		sound_status = SOUND_single_play(&(mp_ctx.sound_variator_end));
		SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		// Decrease until step is 0.
		while (mp_ctx.variator_step_count > 0) {
			// Send OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_T_LESS, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
			KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
			// Decrease step count.
			mp_ctx.variator_step_count--;
		}
		break;
	case MP_EVENT_T_MORE:
		// Log action.
		LOG("event=MP_EVENT_T_MORE");
		// Play random sound.
		if (mp_ctx.variator_step_count == 0) {
			sound_status = SOUND_single_play(&(mp_ctx.sound_variator_start));
			SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		}
		else {
			status = _MP_play_random_variator_sound();
			if (status != MP_SUCCESS) goto errors;
		}
		// Increase step count.
		mp_ctx.variator_step_count++;
		// Send OpenRails shortcut.
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_T_MORE, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
		KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
		break;
	case MP_EVENT_T_LESS:
		// Log action.
		LOG("event=MP_EVENT_T_LESS");
		// Check step count.
		if (mp_ctx.variator_step_count > 0) {
			// Play random sound.
			status = _MP_play_random_variator_sound();
			if (status != MP_SUCCESS) goto errors;
			// Decrease step count.
			mp_ctx.variator_step_count--;
			// Send OpenRails shortcut.
			keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_T_LESS, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
			KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
		}
		break;
	case MP_EVENT_PR:
		// Log action.
		LOG("event=MP_EVENT_PR");
		// TODO
		break;
	case MP_EVENT_P:
		// Log action.
		LOG("event=MP_EVENT_P");
		// TODO
		break;
	case MP_EVENT_F_MORE:
		// Log action.
		LOG("event=MP_EVENT_F_MORE");
		// Play random sound.
		status = _MP_play_random_variator_sound();
		if (status != MP_SUCCESS) goto errors;
		// Send OpenRails shortcut.
		// TODO
		break;
	case MP_EVENT_F_LESS:
		// Log action.
		LOG("event=MP_EVENT_F_LESS");
		// Play random sound.
		status = _MP_play_random_variator_sound();
		if (status != MP_SUCCESS) goto errors;
		// Send OpenRails shortcut.
		// TODO
		break;
	case MP_EVENT_FR:
		// Log action.
		LOG("event=MP_EVENT_FR");
		// TODO
		break;
	default:
		status = MP_ERROR_EVENT;
		goto errors;
	}
errors:
#ifdef LOG_MP
	LOG_STATUS(status, MP_SUCCESS, "OK");
#endif
	return status;
}
