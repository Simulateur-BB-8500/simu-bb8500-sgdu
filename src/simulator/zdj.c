/*
 * zdj.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "zdj.h"

#include "mixer.h"
#include "sound.h"
#include "stdio.h"

/*** ZDJ local global variables ***/

static SOUND_context_t zdj_sound_open;
static SOUND_context_t zdj_sound_lock;

/*** ZDJ functions ***/

/* INIT ZDJ SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void ZDJ_init(void) {
	// Init sound.
	SOUND_init(&zdj_sound_open, "zdj_off.wav", ZDJ_AUDIO_GAIN);
	SOUND_set_volume(&zdj_sound_open, 1.0); // No fade effect required.
	SOUND_init(&zdj_sound_lock, "zen_on.wav", ZDJ_AUDIO_GAIN);
	SOUND_set_volume(&zdj_sound_lock, 1.0); // No fade effect required.
}

/* OPEN CIRCUIT BREAKER.
 * @param:	None.
 * @return:	None.
 */
void ZDJ_open(void) {
	// Play sound.
	SOUND_play(&zdj_sound_open);
	SOUND_stop(&zdj_sound_lock);
	printf("ZDJ *** Open.\n");
	fflush(stdout);
}

/* LOCK CIRCUIT BREAKER.
 * @param:	None.
 * @return:	None.
 */
void ZDJ_lock(void) {
	// Play sound.
	SOUND_play(&zdj_sound_lock);
	printf("ZDJ *** Lock.\n");
	fflush(stdout);
}
