/*
 * fpb.c
 *
 *  Created on: 9 mai 2020
 *      Author: Ludovic
 */

#include "fpb.h"

#include "keyboard.h"
#include "mixer.h"
#include "openrails.h"
#include "sound.h"
#include "stdio.h"
#include "time.h"

/*** FPB local macros ***/

#define FPB_APPLY_RELEASE_PERIOD_MS		1000
#define FPB_LOG

/*** FPB structures ***/

typedef enum {
	FPB_REQUEST_ON,
	FPB_REQUEST_OFF,
	FPB_REQUEST_NEUTRAL,
	FPB_REQUEST_APPLY,
	FPB_REQUEST_RELEASE,
} FPB_Request;

typedef enum {
	FPB_STATE_OFF,
	FPB_STATE_NEUTRAL,
	FPB_STATE_APPLY,
	FPB_STATE_RELEASE
} FPB_State;

typedef struct {
	SOUND_Context fpb_on_sound;
	SOUND_Context fpb_apply_release_sound;
	SOUND_Context fpb_neutral_sound;
	FPB_State fpb_state;
	unsigned char fpb_request;
	unsigned long fpb_apply_release_start_time;
} FPB_Context;

/*** FPB local global variables ***/

static FPB_Context fpb_ctx;

/*** FPB functions ***/

/* INIT FPB MODULE.
 * @param:	None.
 * @return:	None.
 */
void FPB_Init(void) {
	// Init sounds.
	SOUND_Init(&(fpb_ctx.fpb_apply_release_sound), "fpb_apply_release.wav", FPB_AUDIO_GAIN);
	SOUND_SetVolume(&(fpb_ctx.fpb_apply_release_sound), 1.0); // No fade effect required.
	SOUND_Init(&(fpb_ctx.fpb_neutral_sound), "fpb_neutral.wav", FPB_AUDIO_GAIN);
	SOUND_SetVolume(&(fpb_ctx.fpb_neutral_sound), 1.0); // No fade effect required.
	// Init context.
	fpb_ctx.fpb_state = FPB_STATE_NEUTRAL; // Bypass for debug.
	fpb_ctx.fpb_request = FPB_REQUEST_NEUTRAL;
	fpb_ctx.fpb_apply_release_start_time = 0;
}

/* TURN FPB MODULE ON.
 * @param:	None.
 * @return:	None.
 */
void FPB_On(void) {
	// Update request.
	fpb_ctx.fpb_request = FPB_REQUEST_ON;
}

/* TURN FPB MODULE OFF.
 * @param:	None.
 * @return:	None.
 */
void FPB_Off(void) {
	// Update request.
	fpb_ctx.fpb_request = FPB_REQUEST_OFF;
}

/* APPLY FPB.
 * @param:	None.
 * @return:	None.
 */
void FPB_Apply(void) {
	// Update request.
	fpb_ctx.fpb_request = FPB_REQUEST_APPLY;
#ifdef FPB_LOG
	printf("FPB *** Apply.\n");
#endif
}

/* SET FPB TO NEUTRAL.
 * @param:	None.
 * @return:	None.
 */
void FPB_Neutral(void) {
	// Update request.
	fpb_ctx.fpb_request = FPB_REQUEST_NEUTRAL;
#ifdef FPB_LOG
	printf("FPB *** Neutral.\n");
#endif
}

/* RELEASE FPB.
 * @param:	None.
 * @return:	None.
 */
void FPB_Release(void) {
	// Update request.
	fpb_ctx.fpb_request = FPB_REQUEST_RELEASE;
#ifdef FPB_LOG
	printf("FPB *** Release.\n");
#endif
}

/* MAIN TASK OF FPB MODULE.
 * @param:	None.
 * @return:	None.
 */
void FPB_Task(void) {
	// Perform state machine.
	switch (fpb_ctx.fpb_state) {
	case FPB_STATE_OFF:
		// TBD.
		break;
	case FPB_STATE_NEUTRAL:
		switch (fpb_ctx.fpb_request) {
		case FPB_REQUEST_OFF:
			// TBD.
			break;
		case FPB_REQUEST_APPLY:
			// Play sound.
			SOUND_Play(&(fpb_ctx.fpb_apply_release_sound));
			// Send OpenRails shortcut.
			KEYBOARD_Send(OPENRAILS_FPB_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
			// Save time and switch state.
			fpb_ctx.fpb_apply_release_start_time = TIME_GetMs();
			fpb_ctx.fpb_state = FPB_STATE_APPLY;
			break;
		case FPB_REQUEST_RELEASE:
			// Play sound.
			SOUND_Play(&(fpb_ctx.fpb_apply_release_sound));
			// Send OpenRails shortcut.
			KEYBOARD_Send(OPENRAILS_FPB_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
			// Save time and switch state.
			fpb_ctx.fpb_apply_release_start_time = TIME_GetMs();
			fpb_ctx.fpb_state = FPB_STATE_RELEASE;
			break;
		default:
			// Nothing to do.
			break;
		}
		break;
	case FPB_STATE_APPLY:
		if (fpb_ctx.fpb_request == FPB_REQUEST_NEUTRAL) {
			// Play sound.
			SOUND_Play(&(fpb_ctx.fpb_neutral_sound));
			SOUND_Stop(&(fpb_ctx.fpb_apply_release_sound));
			// Come back to neutral state.
			fpb_ctx.fpb_state = FPB_STATE_NEUTRAL;
		}
		else {
			if (TIME_GetMs() > (fpb_ctx.fpb_apply_release_start_time + FPB_APPLY_RELEASE_PERIOD_MS)) {
				// Send OpenRails shortcut  and update time.
				KEYBOARD_Send(OPENRAILS_FPB_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
				fpb_ctx.fpb_apply_release_start_time = TIME_GetMs();
			}
		}
		break;
	case FPB_STATE_RELEASE:
		if (fpb_ctx.fpb_request == FPB_REQUEST_NEUTRAL) {
			// Play sound.
			SOUND_Play(&(fpb_ctx.fpb_neutral_sound));
			SOUND_Stop(&(fpb_ctx.fpb_apply_release_sound));
			// Come back to neutral state.
			fpb_ctx.fpb_state = FPB_STATE_NEUTRAL;
		}
		else {
			if (TIME_GetMs() > (fpb_ctx.fpb_apply_release_start_time + FPB_APPLY_RELEASE_PERIOD_MS)) {
				// Send OpenRails shortcut  and update time.
				KEYBOARD_Send(OPENRAILS_FPB_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
				fpb_ctx.fpb_apply_release_start_time = TIME_GetMs();
			}
		}
		break;
	default:
		// Unknown state.
		fpb_ctx.fpb_state = FPB_STATE_OFF;
		break;
	}
}
