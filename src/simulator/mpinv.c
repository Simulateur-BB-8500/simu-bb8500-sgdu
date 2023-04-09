/*
 * mpinv.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "mpinv.h"

#include "keyboard.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** MPINV local macros ***/

#define MPINV_LOG

/*** MPINV local structures ***/

typedef enum {
	MPINV_STATE_NEUTRAL,
	MPINV_STATE_FORWARD,
	MPINV_STATE_BACKWARD
} MPINV_state_t;

typedef struct {
	SOUND_context_t sound_forward_backward;
	SOUND_context_t sound_neutral;
	MPINV_state_t state;
} MPINV_context_t;

/*** MPINV local global variables ***/

static MPINV_context_t mpinv_ctx;

/*** MPINV functions ***/

/* INIT MPINV MODULE.
 * @param:	None.
 * @return:	None.
 */
void MPINV_init(void) {
	// Init sounds.
	SOUND_init(&(mpinv_ctx.sound_forward_backward), "mpinv_forward_backward.wav", MPINV_AUDIO_GAIN);
	SOUND_set_volume(&(mpinv_ctx.sound_forward_backward), 1.0); // No fade effect required.
	SOUND_init(&(mpinv_ctx.sound_neutral), "mpinv_neutral.wav", MPINV_AUDIO_GAIN);
	SOUND_set_volume(&(mpinv_ctx.sound_neutral), 1.0); // No fade effect required.
}

/* MOVE INVERSOR TO FORWARD.
 * @param:	None.
 * @return:	None.
 */
void MPINV_forward(void) {
	// Play sound.
	SOUND_play(&(mpinv_ctx.sound_forward_backward));
	// Send OpenRails shortcut.
	KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_FORWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
	// Update state.
	mpinv_ctx.state = MPINV_STATE_FORWARD;
#ifdef MPINV_LOG
	printf("MPINV *** Forward.\n");
	fflush(stdout);
#endif
}

/* MOVE INVERSOR TO NEUTRAL.
 * @param:	None.
 * @return:	None.
 */
void MPINV_neutral(void) {
	// Play sound.
	SOUND_play(&(mpinv_ctx.sound_neutral));
	// Send accurate OpenRails shortcut.
	switch (mpinv_ctx.state) {
	case MPINV_STATE_FORWARD:
		// Previous state was forward.
		KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_BACKWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		break;
	case MPINV_STATE_BACKWARD:
		// Previous state was backward.
		KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_FORWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
		break;
	default:
		break;
	}
	// UPdate state.
	mpinv_ctx.state = MPINV_STATE_NEUTRAL;
#ifdef MPINV_LOG
	printf("MPINV *** Neutral.\n");
	fflush(stdout);
#endif
}

/* MOVE INVERSOR TO BACKWARD.
 * @param:	None.
 * @return:	None.
 */
void MPINV_backward(void) {
	// Play sound.
	SOUND_play(&(mpinv_ctx.sound_forward_backward));
	// Send OpenRails shortcut.
	KEYBOARD_single_press(&ORTS_SHORTCUT_MPINV_BACKWARD, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
	// Update state.
	mpinv_ctx.state = MPINV_STATE_BACKWARD;
#ifdef MPINV_LOG
	printf("MPINV *** Backward.\n");
	fflush(stdout);
#endif
}

