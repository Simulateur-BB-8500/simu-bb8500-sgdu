/*
 * fpb.c
 *
 *  Created on: 9 mai 2020
 *      Author: Ludo
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
} FPB_request_t;

typedef enum {
	FPB_STATE_OFF,
	FPB_STATE_NEUTRAL,
	FPB_STATE_APPLY,
	FPB_STATE_RELEASE
} FPB_state_t;

typedef struct {
	SOUND_context_t on_sound;
	SOUND_context_t apply_release_sound;
	SOUND_context_t neutral_sound;
	FPB_state_t state;
	unsigned char request;
	unsigned long apply_release_start_time;
} FPB_context_t;

/*** FPB local global variables ***/

static FPB_context_t fpb_ctx;

/*** FPB functions ***/

/* INIT FPB MODULE.
 * @param:	None.
 * @return:	None.
 */
void FPB_init(void) {
	// Init sounds.
	SOUND_init(&(fpb_ctx.apply_release_sound), "apply_release.wav", FPB_AUDIO_GAIN);
	SOUND_set_volume(&(fpb_ctx.apply_release_sound), 1.0); // No fade effect required.
	SOUND_init(&(fpb_ctx.neutral_sound), "neutral.wav", FPB_AUDIO_GAIN);
	SOUND_set_volume(&(fpb_ctx.neutral_sound), 1.0); // No fade effect required.
	// Init context.
	fpb_ctx.state = FPB_STATE_NEUTRAL; // Bypass for debug.
	fpb_ctx.request = FPB_REQUEST_NEUTRAL;
	fpb_ctx.apply_release_start_time = 0;
}

/* TURN FPB MODULE ON.
 * @param:	None.
 * @return:	None.
 */
void FPB_on(void) {
	// Update request.
	fpb_ctx.request = FPB_REQUEST_ON;
}

/* TURN FPB MODULE OFF.
 * @param:	None.
 * @return:	None.
 */
void FPB_off(void) {
	// Update request.
	fpb_ctx.request = FPB_REQUEST_OFF;
}

/* APPLY FPB.
 * @param:	None.
 * @return:	None.
 */
void FPB_apply(void) {
	// Update request.
	fpb_ctx.request = FPB_REQUEST_APPLY;
#ifdef FPB_LOG
	printf("FPB *** Apply.\n");
#endif
}

/* SET FPB TO NEUTRAL.
 * @param:	None.
 * @return:	None.
 */
void FPB_neutral(void) {
	// Update request.
	fpb_ctx.request = FPB_REQUEST_NEUTRAL;
#ifdef FPB_LOG
	printf("FPB *** Neutral.\n");
#endif
}

/* RELEASE FPB.
 * @param:	None.
 * @return:	None.
 */
void FPB_release(void) {
	// Update request.
	fpb_ctx.request = FPB_REQUEST_RELEASE;
#ifdef FPB_LOG
	printf("FPB *** Release.\n");
#endif
}

/* MAIN TASK OF FPB MODULE.
 * @param:	None.
 * @return:	None.
 */
void FPB_task(void) {
	// Perform state machine.
	switch (fpb_ctx.state) {
	case FPB_STATE_OFF:
		// TBD.
		break;
	case FPB_STATE_NEUTRAL:
		switch (fpb_ctx.request) {
		case FPB_REQUEST_OFF:
			// TBD.
			break;
		case FPB_REQUEST_APPLY:
			// Play sound.
			SOUND_play(&(fpb_ctx.apply_release_sound));
			// Send OpenRails shortcut.
			KEYBOARD_send(OPENRAILS_FPB_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
			// Save time and switch state.
			fpb_ctx.apply_release_start_time = TIME_get_ms();
			fpb_ctx.state = FPB_STATE_APPLY;
			break;
		case FPB_REQUEST_RELEASE:
			// Play sound.
			SOUND_play(&(fpb_ctx.apply_release_sound));
			// Send OpenRails shortcut.
			KEYBOARD_send(OPENRAILS_FPB_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
			// Save time and switch state.
			fpb_ctx.apply_release_start_time = TIME_get_ms();
			fpb_ctx.state = FPB_STATE_RELEASE;
			break;
		default:
			// Nothing to do.
			break;
		}
		break;
	case FPB_STATE_APPLY:
		if (fpb_ctx.request == FPB_REQUEST_NEUTRAL) {
			// Play sound.
			SOUND_play(&(fpb_ctx.neutral_sound));
			SOUND_stop(&(fpb_ctx.apply_release_sound));
			// Come back to neutral state.
			fpb_ctx.state = FPB_STATE_NEUTRAL;
		}
		else {
			if (TIME_get_ms() > (fpb_ctx.apply_release_start_time + FPB_APPLY_RELEASE_PERIOD_MS)) {
				// Send OpenRails shortcut  and update time.
				KEYBOARD_send(OPENRAILS_FPB_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
				fpb_ctx.apply_release_start_time = TIME_get_ms();
			}
		}
		break;
	case FPB_STATE_RELEASE:
		if (fpb_ctx.request == FPB_REQUEST_NEUTRAL) {
			// Play sound.
			SOUND_play(&(fpb_ctx.neutral_sound));
			SOUND_stop(&(fpb_ctx.apply_release_sound));
			// Come back to neutral state.
			fpb_ctx.state = FPB_STATE_NEUTRAL;
		}
		else {
			if (TIME_get_ms() > (fpb_ctx.apply_release_start_time + FPB_APPLY_RELEASE_PERIOD_MS)) {
				// Send OpenRails shortcut  and update time.
				KEYBOARD_send(OPENRAILS_FPB_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
				fpb_ctx.apply_release_start_time = TIME_get_ms();
			}
		}
		break;
	default:
		// Unknown state.
		fpb_ctx.state = FPB_STATE_OFF;
		break;
	}
}
