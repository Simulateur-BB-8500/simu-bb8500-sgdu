/*
 * lights.c
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#include "light.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "orts_shortcut.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** LIGHTS local macros ***/

#define LIGHTS_LOG

/*** LIGHTS local structures ***/

/*******************************************************************/
typedef struct {
	uint8_t status;
	uint8_t overall_state;
} LIGHT_context_t;

/*** LIGHTS local global variables ***/

static LIGHT_context_t light_ctx;

/*** LIGHTS functions ***/

/*******************************************************************/
LIGHT_status_t LIGHT_init(void) {
	// Local variables.
	LIGHT_status_t status = LIGHT_SUCCESS;
	// Init context.
	light_ctx.status = 0;
	light_ctx.overall_state = 0;
#ifdef LOG_LIGHT
	LOG_STATUS(status, LIGHT_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
LIGHT_status_t LIGHT_set_state(LIGHT_type_t type, LIGHT_state_t state) {
	// Local variables.
	LIGHT_status_t status = LIGHT_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check parameters.
	if (type >= LIGHT_TYPE_LAST) {
		status = LIGHT_ERROR_TYPE;
		goto errors;
	}
	if (state >= LIGHT_STATE_LAST) {
		status = LIGHT_ERROR_STATE;
		goto errors;
	}
	// Update bitfield.
	light_ctx.status |= (0b1 << type);
	// Check status.
	if ((light_ctx.status != 0) && (light_ctx.overall_state == 0)) {
		// Send OpenRails shortcuts.
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_LIGHTS_ON, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		KEYBOARD_stack_exit_error(LIGHT_ERROR_DRIVER_KEYBOARD);
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_LIGHTS_ON, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		KEYBOARD_stack_exit_error(LIGHT_ERROR_DRIVER_KEYBOARD);
		// Update overall state.
		light_ctx.overall_state = 1;
	}
	if ((light_ctx.status == 0) && (light_ctx.overall_state != 0)) {
		// Send OpenRails shortcuts.
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_LIGHTS_OFF, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		KEYBOARD_stack_exit_error(LIGHT_ERROR_DRIVER_KEYBOARD);
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_LIGHTS_OFF, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		KEYBOARD_stack_exit_error(LIGHT_ERROR_DRIVER_KEYBOARD);
		// Update overall state.
		light_ctx.overall_state = 0;
	}
errors:
#ifdef LOG_LIGHT
	LOG_STATUS(status, LIGHT_SUCCESS, "type=%d state=%d", type, state);
#endif
	return status;
}
