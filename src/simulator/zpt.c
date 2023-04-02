/*
 * zpt.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "zpt.h"

#include "keyboard.h"
#include "mixer.h"
#include "openrails.h"
#include "sound.h"
#include "stdio.h"

/*** ZPT local macros ***/

#define ZPT_LOG

/*** ZPT local structures ***/

typedef struct {
	SOUND_context_t sound_back_up;
	SOUND_context_t sound_back_down;
	unsigned char back_raised;
	SOUND_context_t sound_front_up;
	SOUND_context_t sound_front_down;
	unsigned char front_raised;
} ZPT_context_t;

/*** ZPT local global variables ***/

static ZPT_context_t zpt_ctx;

/*** ZPT functions ***/

/* INIT ZPT SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void ZPT_init(void) {
	// Init sounds.
	SOUND_init(&(zpt_ctx.sound_back_up), "zpt_up.wav", ZPT_BACK_AUDIO_GAIN);
	SOUND_set_volume(&(zpt_ctx.sound_back_up), 1.0); // No fade effect required.
	SOUND_init(&(zpt_ctx.sound_back_down), "zpt_down.wav", ZPT_BACK_AUDIO_GAIN);
	SOUND_set_volume(&(zpt_ctx.sound_back_down), 1.0); // No fade effect required.
	SOUND_init(&(zpt_ctx.sound_front_up), "zpt_up.wav", ZPT_FRONT_AUDIO_GAIN);
	SOUND_set_volume(&(zpt_ctx.sound_front_up), 1.0); // No fade effect required.
	SOUND_init(&(zpt_ctx.sound_front_down), "zpt_down.wav", ZPT_FRONT_AUDIO_GAIN);
	SOUND_set_volume(&(zpt_ctx.sound_front_down), 1.0); // No fade effect required.
	// Init context.
	zpt_ctx.back_raised = 0;
	zpt_ctx.front_raised = 0;
}

/* RAISE BACK PANTOGRAPH.
 * @param:	None.
 * @eturn:	None.
 */
void ZPT_back_up(void) {
	// Play sound.
	SOUND_play(&(zpt_ctx.sound_back_up));
	// Send OpenRails shortcut if state changed.
	if (zpt_ctx.back_raised == 0) {
		KEYBOARD_send(OPENRAILS_ZPT_BACK_TOGGLE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	}
	zpt_ctx.back_raised = 1;
#ifdef ZPT_LOG
	printf("ZPT *** Back up.\n");
	fflush(stdout);
#endif
}

/* LOWER BACK PANTOGRAPH.
 * @param:	None.
 * @eturn:	None.
 */
void ZPT_back_down(void) {
	// Play sound.
	SOUND_play(&(zpt_ctx.sound_back_down));
	SOUND_stop(&(zpt_ctx.sound_back_up));
	// Send OpenRails shortcut if state changed.
	if (zpt_ctx.back_raised != 0) {
		KEYBOARD_send(OPENRAILS_ZPT_BACK_TOGGLE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	}
	zpt_ctx.back_raised = 0;
#ifdef ZPT_LOG
	printf("ZPT *** Back down.\n");
	fflush(stdout);
#endif
}

/* RAISE FRONT PANTOGRAPH.
 * @param:	None.
 * @eturn:	None.
 */
void ZPT_front_up(void) {
	// Play sound.
	SOUND_play(&(zpt_ctx.sound_front_up));
	// Send OpenRails shortcut if state changed.
	if (zpt_ctx.front_raised == 0) {
		KEYBOARD_send(OPENRAILS_ZPT_FRONT_TOGGLE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	}
	zpt_ctx.front_raised = 1;
#ifdef ZPT_LOG
	printf("ZPT *** Front up.\n");
	fflush(stdout);
#endif
}

/* LOWER FRONT PANTOGRAPH.
 * @param:	None.
 * @eturn:	None.
 */
void ZPT_front_down(void) {
	// Play sound.
	SOUND_play(&(zpt_ctx.sound_front_down));
	SOUND_stop(&(zpt_ctx.sound_front_up));
	// Send OpenRails shortcut if state changed.
	if (zpt_ctx.front_raised != 0) {
		KEYBOARD_send(OPENRAILS_ZPT_FRONT_TOGGLE, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
	}
	zpt_ctx.front_raised = 0;
#ifdef ZPT_LOG
	printf("ZPT *** Front down.\n");
	fflush(stdout);
#endif
}
