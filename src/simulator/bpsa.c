/*
 * bpsa.c
 *
 *  Created on: 2 jan. 2024
 *      Author: Ludo
 */

#include "bpsa.h"

#include "error.h"
#include "keyboard.h"
#include "log.h"
#include "orts_shortcut.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** BPSA local structures ***/

/*******************************************************************/
typedef struct {
	BPSA_state_t state;
} BPSA_context_t;

/*** BPSA local global variables ***/

static BPSA_context_t bpsa_ctx;

/*** BPSA functions ***/

/*******************************************************************/
BPSA_status_t BPSA_init(void) {
	// Local variables.
	BPSA_status_t status = BPSA_SUCCESS;
	// Init state.
	bpsa_ctx.state = BPSA_STATE_OFF;
	LOG_ERROR(status, BPSA_SUCCESS);
	return status;
}

/*******************************************************************/
BPSA_status_t BPSA_set_state(BPSA_state_t state) {
	// Local variables.
	BPSA_status_t status = BPSA_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	// Check state.
	if (state >= BPSA_STATE_LAST) {
		status = BPSA_ERROR_STATE;
		goto errors;
	}
	if (bpsa_ctx.state != state) {
		keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_BPSA, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		KEYBOARD_stack_exit_error(BPSA_ERROR_DRIVER_KEYBOARD);
	}
#ifdef LOG_BPSA
	LOG("state=%d", state);
#endif
	bpsa_ctx.state = state;
errors:
	LOG_ERROR(status, BPSA_SUCCESS);
	return status;
}
