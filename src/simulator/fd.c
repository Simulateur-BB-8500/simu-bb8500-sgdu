/*
 * fd.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "fd.h"

#include "keyboard.h"
#include "mixer.h"
#include "openrails.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** FD local macros ***/

#define FD_LOG
#define FD_FADE_DURATION_MS		500

/*** FD local structures ***/

typedef enum {
	FD_STATE_NEUTRAL,
	FD_STATE_APPLY,
	FD_STATE_RELEASE
} FD_state_t;

typedef struct {
	SOUND_context_t sound_apply;
	SOUND_context_t sound_release;
	FD_state_t state;
} FD_context_t;

/*** FD local global variables ***/

static FD_context_t fd_ctx;

/*** FD functions ***/

/* INIT FD MODULE.
 * @param:	None.
 * @return:	None.
 */
void FD_init(void) {
	// Init sounds.
	SOUND_init(&(fd_ctx.sound_apply), "fd_apply.wav", FD_AUDIO_GAIN);
	SOUND_set_volume(&(fd_ctx.sound_apply), 1.0); // No fade effect required.
	SOUND_init(&(fd_ctx.sound_release), "fd_release.wav", FD_AUDIO_GAIN);
	SOUND_set_volume(&(fd_ctx.sound_release), 1.0); // No fade effect required.
}

/* APPLY FD.
 * @param:	None.
 * @return:	None.
 */
void FD_apply(void) {
	// Play sound.
	SOUND_play(&(fd_ctx.sound_apply));
	// Send OpenRails shortcut (twice if previous state was released).
	KEYBOARD_send(&OPENRAILS_FD_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	KEYBOARD_send(&OPENRAILS_FD_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	// Update state.
	fd_ctx.state = FD_STATE_APPLY;
#ifdef FD_LOG
	printf("FD *** Apply.\n");
#endif
}

/* SET FD TO NEUTRAL.
 * @param:	None.
 * @return:	None.
 */
void FD_neutral(void) {
	// Save release sound parameter for fade-out.
	SOUND_save_fade_parameters(&(fd_ctx.sound_release));
	// Send accurate OpenRails shortcut.
	switch (fd_ctx.state) {
	case FD_STATE_APPLY:
		// Previous state was forward.
		KEYBOARD_send(&OPENRAILS_FD_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
		break;
	case FD_STATE_RELEASE:
		// Previous state was backward.
		KEYBOARD_send(&OPENRAILS_FD_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
		break;
	default:
		break;
	}
	// Update state.
	fd_ctx.state = FD_STATE_NEUTRAL;
#ifdef FD_LOG
	printf("FD *** Neutral.\n");
#endif
}

/* RELEASE FD.
 * @param:	None.
 * @return:	None.
 */
void FD_release(void) {
	// Play sound.
	SOUND_set_volume(&(fd_ctx.sound_release), 1.0); // No fade effect required.
	SOUND_play(&(fd_ctx.sound_release));
	// Send OpenRails shortcut (twice if previous state was applied).
	KEYBOARD_send(&OPENRAILS_FD_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	KEYBOARD_send(&OPENRAILS_FD_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	// Update state.
	fd_ctx.state = FD_STATE_RELEASE;
#ifdef FD_LOG
	printf("FD *** Release.\n");
#endif
}

/* MAIN TASK OF FD MODULE.
 * @param:	None.
 * @return:	None.
 */
void FD_task(void) {
	// Release sound fade-out.
	if (fd_ctx.state != FD_STATE_RELEASE) {
		SOUND_fade_out(&(fd_ctx.sound_release), FD_FADE_DURATION_MS);
	}
}
