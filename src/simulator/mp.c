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
#include "scu.h"
#include "sound.h"
#include "stdlib.h"
#include "stdint.h"
#include "time.h"

/*** MP local macros ***/

#define MP_NUMBER_OF_VARIATOR_TRACKS	10

#define MP_0_DRIVE_LEVEL				0
#define MP_0_DYNAMIC_BRAKE_LEVEL		(-1)

#define MP_P_DRIVE_LEVEL				0
#define MP_P_DYNAMIC_BRAKE_LEVEL		0

#define MP_SYNCHRONIZE_PERIOD_MS		1000

/*** MP local structures ***/

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_variator[MP_NUMBER_OF_VARIATOR_TRACKS];
	SOUND_context_t sound_variator_start;
	SOUND_context_t sound_variator_end;
	uint8_t sound_variator_index;
	int32_t drive_level;
	int32_t drive_level_target;
	int32_t dynamic_brake_level;
	int32_t dynamic_brake_level_target;
	uint8_t synchronize_flag;
	uint32_t synchronize_next_time;
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
	keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_MORE, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
	KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
errors:
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}

/*******************************************************************/
static MP_status_t _MP_less(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Send OpenRails shortcut.
	keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_MP_LESS, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
	KEYBOARD_stack_exit_error(MP_ERROR_DRIVER_KEYBOARD);
errors:
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}

/*******************************************************************/
static MP_status_t _MP_synchronize(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	// Synchronize step count.
	if ((mp_ctx.drive_level != mp_ctx.drive_level_target) || (mp_ctx.dynamic_brake_level != mp_ctx.dynamic_brake_level_target)) {
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "drive_level=%d dynamic_brake_level=%d", mp_ctx.drive_level, mp_ctx.dynamic_brake_level);
#endif
		// Check if we come from drive or brake.
		if ((mp_ctx.drive_level > mp_ctx.drive_level_target) || (mp_ctx.dynamic_brake_level < mp_ctx.dynamic_brake_level_target)) {
			status = _MP_less();
			if (status != MP_SUCCESS) goto errors;
		}
		if ((mp_ctx.drive_level < mp_ctx.drive_level_target) || (mp_ctx.dynamic_brake_level > mp_ctx.dynamic_brake_level_target)) {
			status = _MP_more();
			if (status != MP_SUCCESS) goto errors;
		}
	}
errors:
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}

/*******************************************************************/
static MP_status_t _MP_play_variator_sound(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Play sound.
	sound_status = SOUND_single_play(&(mp_ctx.sound_variator[mp_ctx.sound_variator_index]));
	SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
errors:
	// Increment index.
	mp_ctx.sound_variator_index = (mp_ctx.sound_variator_index + 1) % MP_NUMBER_OF_VARIATOR_TRACKS;
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}

/*** MP functions ***/

/*******************************************************************/
MP_status_t MP_init(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	uint8_t idx = 0;
	// Init context.
	mp_ctx.sound_variator_index = 0;
	mp_ctx.drive_level = MP_0_DRIVE_LEVEL;
	mp_ctx.drive_level_target = MP_0_DRIVE_LEVEL;
	mp_ctx.dynamic_brake_level = MP_0_DYNAMIC_BRAKE_LEVEL;
	mp_ctx.dynamic_brake_level_target = MP_0_DYNAMIC_BRAKE_LEVEL;
	mp_ctx.synchronize_flag = 0;
	mp_ctx.synchronize_next_time = 0;
	// Init sounds.
	for (idx=0 ; idx<MP_NUMBER_OF_VARIATOR_TRACKS ; idx++) {
		// Init sound.
		sound_status = SOUND_init(&(mp_ctx.sound_variator[idx]), "mp_variator.wav", MP_AUDIO_GAIN);
		SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
	}
	sound_status = SOUND_init(&(mp_ctx.sound_variator_start), "mp_variator_start.wav", MP_AUDIO_GAIN);
	SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(mp_ctx.sound_variator_end), "mp_variator_end.wav", MP_AUDIO_GAIN);
	SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
errors:
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}

/*******************************************************************/
MP_status_t MP_set_event(MP_event_t event) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Reset flag by default.
	mp_ctx.synchronize_flag = 0;
	// Check event.
	switch (event) {
	case MP_EVENT_0:
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "event=MP_EVENT_0");
#endif
		// Play sound.
		sound_status = SOUND_single_play(&(mp_ctx.sound_variator_end));
		SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		// Request synchronization.
		mp_ctx.drive_level_target = MP_0_DRIVE_LEVEL;
		mp_ctx.dynamic_brake_level_target = MP_0_DYNAMIC_BRAKE_LEVEL;
		mp_ctx.synchronize_flag = 1;
		break;
	case MP_EVENT_T_MORE:
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "event=MP_EVENT_T_MORE");
#endif
		// Play random sound.
		if ((mp_ctx.drive_level == MP_0_DRIVE_LEVEL) && (mp_ctx.dynamic_brake_level == MP_0_DYNAMIC_BRAKE_LEVEL)) {
			sound_status = SOUND_single_play(&(mp_ctx.sound_variator_start));
			SOUND_stack_exit_error(MP_ERROR_DRIVER_SOUND);
		}
		else {
			status = _MP_play_variator_sound();
			if (status != MP_SUCCESS) goto errors;
		}
		status = _MP_more();
		if (status != MP_SUCCESS) goto errors;
		break;
	case MP_EVENT_T_LESS:
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "event=MP_EVENT_T_LESS");
#endif
		// Check drive level.
		if (mp_ctx.drive_level > 0) {
			// Play random sound.
			status = _MP_play_variator_sound();
			if (status != MP_SUCCESS) goto errors;
			// Decrease step count.
			status = _MP_less();
			if (status != MP_SUCCESS) goto errors;
		}
		break;
	case MP_EVENT_P:
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "event=MP_EVENT_P");
#endif
		// Play sound.
		status = _MP_play_variator_sound();
		if (status != MP_SUCCESS) goto errors;
		// Request synchronization.
		mp_ctx.drive_level_target = MP_P_DRIVE_LEVEL;
		mp_ctx.dynamic_brake_level_target = MP_P_DYNAMIC_BRAKE_LEVEL;
		mp_ctx.synchronize_flag = 1;
		break;
	case MP_EVENT_F_MORE:
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "event=MP_EVENT_F_MORE");
#endif
		// Play random sound.
		status = _MP_play_variator_sound();
		if (status != MP_SUCCESS) goto errors;
		// Decrease step count.
		status = _MP_less();
		if (status != MP_SUCCESS) goto errors;
		break;
	case MP_EVENT_F_LESS:
#ifdef LOG_MP
		LOG_trace(LOG_COLOR_WHITE, "event=MP_EVENT_F_LESS");
#endif
		// Check dynamic brake level.
		if (mp_ctx.dynamic_brake_level > 0) {
			// Play random sound.
			status = _MP_play_variator_sound();
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
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}

/*******************************************************************/
void MP_set_current_position(int32_t drive_level, int32_t dynamic_brake_level) {
	// Update local data.
	mp_ctx.drive_level = drive_level;
	mp_ctx.dynamic_brake_level = dynamic_brake_level;
}

/*******************************************************************/
MP_status_t MP_process(void) {
	// Local variables.
	MP_status_t status = MP_SUCCESS;
	// Check flag.
	if ((mp_ctx.synchronize_flag != 0) && (TIME_get_milliseconds() >= mp_ctx.synchronize_next_time)) {
		// Update next time.
		mp_ctx.synchronize_next_time = TIME_get_milliseconds() + MP_SYNCHRONIZE_PERIOD_MS;
		// Synchronize throttle position.
		status = _MP_synchronize();
		if (status != MP_SUCCESS) goto errors;
	}
errors:
	LOG_ERROR(status, MP_SUCCESS);
	return status;
}
