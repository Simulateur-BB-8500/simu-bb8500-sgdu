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

static SOUND_context_t zdj_open_sound;
static SOUND_context_t zdj_lock_sound;

/*** ZDJ functions ***/

/* INIT ZDJ SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void ZDJ_init(void) {
	// Init sound.
	SOUND_init(&zdj_open_sound, "zdj.wav", ZDJ_AUDIO_GAIN);
	SOUND_set_volume(&zdj_open_sound, 1.0); // No fade effect required.
	SOUND_init(&zdj_lock_sound, "zen.wav", ZDJ_AUDIO_GAIN);
	SOUND_set_volume(&zdj_lock_sound, 1.0); // No fade effect required.
}

/* OPEN CIRCUIT BREAKER.
 * @param:	None.
 * @return:	None.
 */
void ZDJ_open(void) {
	// Play sound.
	SOUND_play(&zdj_open_sound);
	SOUND_stop(&zdj_lock_sound);
	printf("ZDJ *** Open.\n");
	fflush(stdout);
}

/* LOCK CIRCUIT BREAKER.
 * @param:	None.
 * @return:	None.
 */
void ZDJ_lock(void) {
	// Play sound.
	SOUND_play(&zdj_lock_sound);
	printf("ZDJ *** Lock.\n");
	fflush(stdout);
}
