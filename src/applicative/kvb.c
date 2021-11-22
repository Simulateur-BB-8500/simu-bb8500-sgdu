/*
 * kvb.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "kvb.h"

#include "lsmcu.h"
#include "lssgkcu.h"
#include "mixer.h"
#include "sound.h"
#include "time.h"

/*** KVB local macros ***/

#define KVB_PA400_DURATION_MS		2000
#define KVB_PA400_OFF_DURATION_MS	2000
#define KVB_UC512_DURATION_MS		2000
#define KVB_888888_DURATION_MS		3000

/*** KVB local structures ***/

typedef enum {
	KVB_STATE_OFF,
	KVB_STATE_PA400,
	KVB_STATE_PA400_OFF,
	KVB_STATE_UC512,
	KVB_STATE_888888,
	KVB_STATE_WAIT_VALIDATION,
	KVB_STATE_IDLE
} KVB_State;

typedef struct {
	SOUND_Context kvb_sound_turnon;
	SOUND_Context kvb_sound_turnoff;
	SOUND_Context kvb_sound_urgency;
	KVB_State kvb_state;
	unsigned int kvb_state_switch_time_ms;
	unsigned char kvb_bl_unlocked;
	unsigned char kvb_bpval_pressed;
} KVB_Context;

/*** KVB local global variables ***/

static KVB_Context kvb_ctx;

/*** KVB functions ***/

/* INIT KVB CALCULATOR.
 * @param:	None.
 * @return:	None.
 */
void KVB_Init(void) {
	// Init sounds.
	SOUND_Init(&(kvb_ctx.kvb_sound_turnon), "kvb_turnon.wav", KVB_AUDIO_GAIN);
	SOUND_SetVolume(&(kvb_ctx.kvb_sound_turnon), 1.0); // No fade effect required.
	SOUND_Init(&(kvb_ctx.kvb_sound_turnoff), "kvb_turnoff.wav", KVB_AUDIO_GAIN);
	SOUND_SetVolume(&(kvb_ctx.kvb_sound_turnoff), 1.0); // No fade effect required.
	SOUND_Init(&(kvb_ctx.kvb_sound_urgency), "kvb_urgency.wav", KVB_AUDIO_GAIN);
	SOUND_SetVolume(&(kvb_ctx.kvb_sound_urgency), 1.0); // No fade effect required.
	// Init state machine.
	kvb_ctx.kvb_state = KVB_STATE_OFF;
	kvb_ctx.kvb_state_switch_time_ms = 0;
	kvb_ctx.kvb_bl_unlocked = 0;
	kvb_ctx.kvb_bpval_pressed = 0;
}

/* TURN KVB ON.
 * @param:	None.
 * @return:	None.
 */
void KVB_TurnOn(void) {
	kvb_ctx.kvb_bl_unlocked = 1;
	// Play sound.
	SOUND_Play(&(kvb_ctx.kvb_sound_turnon));
	SOUND_Stop(&(kvb_ctx.kvb_sound_turnoff));
}

/* TURN KVB OFF.
 * @param:	None.
 * @return:	None.
 */
void KVB_TurnOff(void) {
	kvb_ctx.kvb_bl_unlocked = 0;
	// Play sound.
	SOUND_Play(&(kvb_ctx.kvb_sound_turnoff));
	SOUND_Stop(&(kvb_ctx.kvb_sound_turnon));
}

/* MAIN TASK OF KVB CALCULATOR.
 * @param:	None.
 * @return:	None.
 */
void KVB_Task(void) {
	// Perform internal state machine.
	switch (kvb_ctx.kvb_state) {
	case KVB_STATE_OFF:
		if (kvb_ctx.kvb_bl_unlocked != 0) {
			// Start KVB init.
			LSMCU_Send(LSMCU_IN_KVB_YG_PA400);
			LSMCU_Send(LSMCU_IN_KVB_LSSF_BLINK);
			kvb_ctx.kvb_state = KVB_STATE_PA400;
			kvb_ctx.kvb_state_switch_time_ms = TIME_GetMs();
		}
		break;
	case KVB_STATE_PA400:
		// Check BL state.
		if (kvb_ctx.kvb_bl_unlocked == 0) {
			LSMCU_Send(LSMCU_IN_KVB_ALL_OFF);
			kvb_ctx.kvb_state = KVB_STATE_OFF;
		}
		else {
			// Wait PA400 display duration.
			if (TIME_GetMs() > (kvb_ctx.kvb_state_switch_time_ms + KVB_PA400_DURATION_MS)) {
				LSMCU_Send(LSMCU_IN_KVB_YG_OFF);
				kvb_ctx.kvb_state = KVB_STATE_PA400_OFF;
				kvb_ctx.kvb_state_switch_time_ms = TIME_GetMs();
			}
		}
		break;
	case KVB_STATE_PA400_OFF:
		// Check BL state.
		if (kvb_ctx.kvb_bl_unlocked == 0) {
			LSMCU_Send(LSMCU_IN_KVB_ALL_OFF);
			kvb_ctx.kvb_state = KVB_STATE_OFF;
		}
		else {
			// Wait transition duration.
			if (TIME_GetMs() > (kvb_ctx.kvb_state_switch_time_ms + KVB_PA400_OFF_DURATION_MS)) {
				LSMCU_Send(LSMCU_IN_KVB_YG_UC512);
				kvb_ctx.kvb_state = KVB_STATE_UC512;
				kvb_ctx.kvb_state_switch_time_ms = TIME_GetMs();
			}
		}
		break;
	case KVB_STATE_UC512:
		// Check BL state.
		if (kvb_ctx.kvb_bl_unlocked == 0) {
			LSMCU_Send(LSMCU_IN_KVB_ALL_OFF);
			kvb_ctx.kvb_state = KVB_STATE_OFF;
		}
		else {
			// Wait for UC512 display duration.
			if (TIME_GetMs() > (kvb_ctx.kvb_state_switch_time_ms + KVB_UC512_DURATION_MS)) {
				LSMCU_Send(LSMCU_IN_KVB_YG_888);
				LSMCU_Send(LSMCU_IN_KVB_LVAL_BLINK);
				kvb_ctx.kvb_state = KVB_STATE_888888;
				kvb_ctx.kvb_state_switch_time_ms = TIME_GetMs();
			}
		}
		break;
	case KVB_STATE_888888:
		// Check BL state.
		if (kvb_ctx.kvb_bl_unlocked == 0) {
			LSMCU_Send(LSMCU_IN_KVB_ALL_OFF);
			kvb_ctx.kvb_state = KVB_STATE_OFF;
		}
		else {
			// Wait for 888888 display duration.
			if (TIME_GetMs() > (kvb_ctx.kvb_state_switch_time_ms + KVB_888888_DURATION_MS)) {
				LSMCU_Send(LSMCU_IN_KVB_YG_OFF);
				kvb_ctx.kvb_state = KVB_STATE_WAIT_VALIDATION;
				kvb_ctx.kvb_state_switch_time_ms = TIME_GetMs();
			}
		}
		break;
	case KVB_STATE_WAIT_VALIDATION:
		// Check BL state.
		if (kvb_ctx.kvb_bl_unlocked == 0) {
			LSMCU_Send(LSMCU_IN_KVB_ALL_OFF);
			kvb_ctx.kvb_state = KVB_STATE_OFF;
		}
		else {
			// Check BPVAL.
			if (kvb_ctx.kvb_bpval_pressed != 0) {
				// Parameters validated, go to idle state.
				kvb_ctx.kvb_state = KVB_STATE_IDLE;
			}
		}
		break;
	case KVB_STATE_IDLE:
		// Check BL state.
		if (kvb_ctx.kvb_bl_unlocked == 0) {
			LSMCU_Send(LSMCU_IN_KVB_ALL_OFF);
			kvb_ctx.kvb_state = KVB_STATE_OFF;
		}
		break;
	default:
		break;
	}
}
