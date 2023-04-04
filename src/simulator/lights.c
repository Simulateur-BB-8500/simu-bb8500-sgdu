/*
 * lights.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "lights.h"

#include "keyboard.h"
#include "orts_shortcut.h"
#include "stdint.h"
#include "stdio.h"

/*** LIGHTS local macros ***/

#define LIGHTS_LOG

/*** LIGHTS local structures ***/

typedef enum {
	LIGHTS_ZFG_STATUS_BIT_INDEX,
	LIGHTS_ZFD_STATUS_BIT_INDEX,
	LIGHTS_ZPR_STATUS_BIT_INDEX
} LIGHTS_status_bit_index_t;

typedef enum {
	LIGHTS_STATE_OFF,
	LIGHTS_STATE_ON
} LIGHTS_state_t;

typedef struct {
	uint8_t status;
	LIGHTS_state_t state;
} LIGHTS_context_t;

/*** LIGHTS local global variables ***/

static LIGHTS_context_t lights_ctx;

/*** LIGHTS functions ***/

/* INIT LIGHT MODULE.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_init(void) {
	lights_ctx.status = 0;
	lights_ctx.state = LIGHTS_STATE_OFF;
}

/* TURN ZFG ON.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_zfg_on(void) {
	lights_ctx.status |= (0b1 << LIGHTS_ZFG_STATUS_BIT_INDEX);
#ifdef LIGHTS_LOG
	printf("LIGHTS *** ZFG on\n");
	fflush(stdout);
#endif
}

/* TURN ZFG OFF.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_zfg_off(void) {
	lights_ctx.status &= ~(0b1 << LIGHTS_ZFG_STATUS_BIT_INDEX);
#ifdef LIGHTS_LOG
	printf("LIGHTS *** ZFG off\n");
	fflush(stdout);
#endif
}

/* TURN ZFD ON.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_zfd_on(void) {
	lights_ctx.status |= (0b1 << LIGHTS_ZFD_STATUS_BIT_INDEX);
#ifdef LIGHTS_LOG
	printf("LIGHTS *** ZFD on\n");
	fflush(stdout);
#endif
}

/* TURN ZFD OFF.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_zfd_off(void) {
	lights_ctx.status &= ~(0b1 << LIGHTS_ZFD_STATUS_BIT_INDEX);
#ifdef LIGHTS_LOG
	printf("LIGHTS *** ZFD off\n");
	fflush(stdout);
#endif
}

/* TURN ZPR ON.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_zpr_on(void) {
	lights_ctx.status |= (0b1 << LIGHTS_ZPR_STATUS_BIT_INDEX);
#ifdef LIGHTS_LOG
	printf("LIGHTS *** ZPR on\n");
	fflush(stdout);
#endif
}

/* TURN ZPR OFF.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_zpr_off(void) {
	lights_ctx.status &= ~(0b1 << LIGHTS_ZPR_STATUS_BIT_INDEX);
#ifdef LIGHTS_LOG
	printf("LIGHTS *** ZPR off\n");
	fflush(stdout);
#endif
}

/* MAIN TASK OF LIGHTS MODULE.
 * @param:	None.
 * @return:	None.
 */
void LIGHTS_task(void) {
	// Perform state machine.
	switch (lights_ctx.state) {
	case LIGHTS_STATE_OFF:
		if (lights_ctx.status != 0) {
			// Send keyboard control.
			KEYBOARD_send(&ORTS_SHORTCUT_LIGHTS_ON, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_send(&ORTS_SHORTCUT_LIGHTS_ON, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			lights_ctx.state = LIGHTS_STATE_ON;
		}
		break;
	case LIGHTS_STATE_ON:
		if (lights_ctx.status == 0) {
			// Send keyboard control.
			KEYBOARD_send(&ORTS_SHORTCUT_LIGHTS_OFF, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			KEYBOARD_send(&ORTS_SHORTCUT_LIGHTS_OFF, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
			lights_ctx.state = LIGHTS_STATE_OFF;
		}
		break;
	default:
		lights_ctx.state = LIGHTS_STATE_OFF;
		break;
	}
}
