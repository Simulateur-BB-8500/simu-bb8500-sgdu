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
#include "stdio.h"

/*** FD local macros ***/

#define FD_LOG
#define FD_FADE_DURATION_MS		500

/*** FD local structures ***/

typedef enum {
	FD_STATE_NEUTRAL,
	FD_STATE_APPLY,
	FD_STATE_RELEASE
} FD_State;

typedef struct {
	SOUND_Context fd_apply_sound;
	SOUND_Context fd_release_sound;
	FD_State fd_state;
} FD_Context;

/*** FD local global variables ***/

static FD_Context fd_ctx;

/*** FD functions ***/

/* INIT FD MODULE.
 * @param:	None.
 * @return:	None.
 */
void FD_Init(void) {
	// Init sounds.
	SOUND_Init(&(fd_ctx.fd_apply_sound), "fd_apply.wav", FD_AUDIO_GAIN);
	SOUND_SetVolume(&(fd_ctx.fd_apply_sound), 1.0); // No fade effect required.
	SOUND_Init(&(fd_ctx.fd_release_sound), "fd_release.wav", FD_AUDIO_GAIN);
	SOUND_SetVolume(&(fd_ctx.fd_release_sound), 1.0); // No fade effect required.
}

/* APPLY FD.
 * @param:	None.
 * @return:	None.
 */
void FD_Apply(void) {
	// Play sound.
	SOUND_Play(&(fd_ctx.fd_apply_sound));
	// Send OpenRails shortcut (twice if previous state was released).
	KEYBOARD_Send(OPENRAILS_FD_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	KEYBOARD_Send(OPENRAILS_FD_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	// Update state.
	fd_ctx.fd_state = FD_STATE_APPLY;
#ifdef FD_LOG
	printf("FD *** Apply.\n");
#endif
}

/* SET FD TO NEUTRAL.
 * @param:	None.
 * @return:	None.
 */
void FD_Neutral(void) {
	// Save release sound parameter for fade-out.
	SOUND_SaveFadeParameters(&(fd_ctx.fd_release_sound));
	// Send accurate OpenRails shortcut.
	switch (fd_ctx.fd_state) {
	case FD_STATE_APPLY:
		// Previous state was forward.
		KEYBOARD_Send(OPENRAILS_FD_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
		break;
	case FD_STATE_RELEASE:
		// Previous state was backward.
		KEYBOARD_Send(OPENRAILS_FD_APPLY, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
		break;
	default:
		break;
	}
	// Update state.
	fd_ctx.fd_state = FD_STATE_NEUTRAL;
#ifdef FD_LOG
	printf("FD *** Neutral.\n");
#endif
}

/* RELEASE FD.
 * @param:	None.
 * @return:	None.
 */
void FD_Release(void) {
	// Play sound.
	SOUND_SetVolume(&(fd_ctx.fd_release_sound), 1.0); // No fade effect required.
	SOUND_Play(&(fd_ctx.fd_release_sound));
	// Send OpenRails shortcut (twice if previous state was applied).
	KEYBOARD_Send(OPENRAILS_FD_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	KEYBOARD_Send(OPENRAILS_FD_RELEASE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	// Update state.
	fd_ctx.fd_state = FD_STATE_RELEASE;
#ifdef FD_LOG
	printf("FD *** Release.\n");
#endif
}

/* MAIN TASK OF FD MODULE.
 * @param:	None.
 * @return:	None.
 */
void FD_Task(void) {
	// Release sound fade-out.
	if (fd_ctx.fd_state != FD_STATE_RELEASE) {
		SOUND_FadeOut(&(fd_ctx.fd_release_sound), FD_FADE_DURATION_MS);
	}
}
