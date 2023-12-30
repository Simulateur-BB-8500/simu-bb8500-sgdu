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
#include "lsagiu.h"
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
	int8_t variator_step_count;
} MP_context_t;

/*** MP local global variables ***/

static MP_context_t mp_ctx;

/*** MP local functions ***/

/*******************************************************************/
static MP_status_t _MP_more(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Send OpenRails shortcut.
	keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_MORE, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
	KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
errors:
	// Increase step count.
	mp_ctx.variator_step_count++;
	return status;
}

/*******************************************************************/
static MP_status_t _MP_less(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Send OpenRails shortcut.
	keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_LESS, ORTS_SHORTCUT_PRESS_DURATION_MS_MP);
	KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
errors:
	// Decrease step count.
	mp_ctx.variator_step_count--;
	return status;
}

/*******************************************************************/
static MP_status_t _MP_synchronize(int8_t step_count_target) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	// Synchronize step count.
	while (mp_ctx.variator_step_count != step_count_target) {
		// Check if we come from drive or brake.
		if (mp_ctx.variator_step_count < step_count_target) {
			status = _MP_more();
			if (status != MP_SUCCESS) goto errors;
		}
		else {
			status = _MP_less();
			if (status != MP_SUCCESS) goto errors;
		}
	}
errors:
	return status;
}

/*******************************************************************/
static MP_status_t _MP_play_random_variator_sound(void) {
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
	mp_ctx.variator_step_count = LSAGIU_MP_VARIATOR_STEP_0;
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
	// Check event.
	switch (event) {
	case MP_EVENT_0:
		// Log action.
		LOG("event=MP_EVENT_0");
		// Play sound.
		sound_status = SOUND_single_play(&(mp_ctx.sound_variator_end));
		SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		// Synchronize step count.
		status = _MP_synchronize(LSAGIU_MP_VARIATOR_STEP_0);
		if (status != MP_SUCCESS) goto errors;
		break;
	case MP_EVENT_T_MORE:
		// Log action.
		LOG("event=MP_EVENT_T_MORE");
		// Play random sound.
		if (mp_ctx.variator_step_count == LSAGIU_MP_VARIATOR_STEP_0) {
			sound_status = SOUND_single_play(&(mp_ctx.sound_variator_start));
			SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		}
		else {
			status = _MP_play_random_variator_sound();
			if (status != MP_SUCCESS) goto errors;
		}
		status = _MP_more();
		if (status != MP_SUCCESS) goto errors;
		break;
	case MP_EVENT_T_LESS:
		// Log action.
		LOG("event=MP_EVENT_T_LESS");
		// Check step count.
		if (mp_ctx.variator_step_count > LSAGIU_MP_VARIATOR_STEP_0) {
			// Play random sound.
			status = _MP_play_random_variator_sound();
			if (status != MP_SUCCESS) goto errors;
			// Decrease step count.
			status = _MP_less();
			if (status != MP_SUCCESS) goto errors;
		}
		break;
	case MP_EVENT_P:
		// Log action.
		LOG("event=MP_EVENT_P");
		// Play sound.
		status = _MP_play_random_variator_sound();
		if (status != MP_SUCCESS) goto errors;
		// Synchronize step count.
		status = _MP_synchronize(LSAGIU_MP_VARIATOR_STEP_P);
		if (status != MP_SUCCESS) goto errors;
		break;
	case MP_EVENT_F_MORE:
		// Log action.
		LOG("event=MP_EVENT_F_MORE");
		// Play random sound.
		status = _MP_play_random_variator_sound();
		if (status != MP_SUCCESS) goto errors;
		// Decrease step count.
		status = _MP_less();
		if (status != MP_SUCCESS) goto errors;
		break;
	case MP_EVENT_F_LESS:
		// Log action.
		LOG("event=MP_EVENT_F_LESS");
		// Check step count.
		if (mp_ctx.variator_step_count < LSAGIU_MP_VARIATOR_STEP_P) {
			// Play random sound.
			status = _MP_play_random_variator_sound();
			if (status != MP_SUCCESS) goto errors;
			// Increase step count.
			status = _MP_more();
			if (status != MP_SUCCESS) goto errors;
		}
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
