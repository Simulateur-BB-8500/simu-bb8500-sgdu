/*
 * fpb.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "fpb.h"

#include "keyboard.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** FPB local macros ***/

#define FPB_LOG
#define FPB_FADE_DURATION_MS	500

/*** FPB local structures ***/

typedef enum {
	FPB_STATE_NEUTRAL,
	FPB_STATE_APPLY,
	FPB_STATE_RELEASE
} FPB_state_t;

typedef struct {
	SOUND_context_t sound_apply;
	SOUND_context_t sound_release;
	FPB_state_t state;
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
	SOUND_init(&(fpb_ctx.sound_apply), "fpb_apply.wav", FPB_AUDIO_GAIN);
	SOUND_set_volume(&(fpb_ctx.sound_apply), 1.0); // No fade effect required.
	SOUND_init(&(fpb_ctx.sound_release), "fpb_release.wav", FPB_AUDIO_GAIN);
	SOUND_set_volume(&(fpb_ctx.sound_release), 1.0); // No fade effect required.
}

/* APPLY FPB.
 * @param:	None.
 * @return:	None.
 */
void FPB_apply(void) {
	// Play sound.
	SOUND_play(&(fpb_ctx.sound_apply));
	// Press OpenRails shortcut.
	KEYBOARD_press(&ORTS_SHORTCUT_FPB_APPLY);
	// Update state.
	fpb_ctx.state = FPB_STATE_APPLY;
#ifdef FPB_LOG
	printf("FPB *** Apply.\n");
#endif
}

/* RELEASE FPB.
 * @param:	None.
 * @return:	None.
 */
void FPB_release(void) {
	// Play sound.
	SOUND_play(&(fpb_ctx.sound_release));
	// Press OpenRails shortcut.
	KEYBOARD_press(&ORTS_SHORTCUT_FPB_RELEASE);
	// Update state.
	fpb_ctx.state = FPB_STATE_RELEASE;
#ifdef FPB_LOG
	printf("FPB *** Release.\n");
#endif
}

/* SET FPB TO NEUTRAL.
 * @param:	None.
 * @return:	None.
 */
void FPB_neutral(void) {
	// Save release sound parameter for fade-out.
	SOUND_save_fade_parameters(&(fpb_ctx.sound_release));
	// Send accurate OpenRails shortcut.
	switch (fpb_ctx.state) {
	case FPB_STATE_APPLY:
		// Previous state was forward.
		KEYBOARD_release(&ORTS_SHORTCUT_FPB_APPLY);
		break;
	case FPB_STATE_RELEASE:
		// Previous state was backward.
		KEYBOARD_release(&ORTS_SHORTCUT_FPB_RELEASE);
		break;
	default:
		break;
	}
	// Update state.
	fpb_ctx.state = FPB_STATE_NEUTRAL;
#ifdef FPB_LOG
	printf("FPB *** Neutral.\n");
#endif
}

/* MAIN TASK OF FPB MODULE.
 * @param:	None.
 * @return:	None.
 */
void FPB_task(void) {
	// Release sound fade-out.
	if (fpb_ctx.state == FPB_STATE_NEUTRAL) {
		SOUND_fade_out(&(fpb_ctx.sound_release), FPB_FADE_DURATION_MS);
	}
}
