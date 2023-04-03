/*
 * zba.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "zba.h"

#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** ZBA local global variables ***/

static SOUND_context_t zba_sound_on;
static SOUND_context_t zba_sound_off;

/*** ZBA functions ***/

/* INIT ZBA SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void ZBA_init(void) {
	// Init sound.
	SOUND_init(&zba_sound_on, "zba_on.wav", ZBA_AUDIO_GAIN);
	SOUND_set_volume(&zba_sound_on, 1.0); // No fade effect required.
	SOUND_init(&zba_sound_off, "zba_off.wav", ZBA_AUDIO_GAIN);
	SOUND_set_volume(&zba_sound_off, 1.0); // No fade effect required.
}

/* TURN ZBA ON.
 * @param:	None.
 * @return:	None.
 */
void ZBA_turn_on(void) {
	// Play sound.
	SOUND_play(&zba_sound_on);
	printf("ZBA *** Turn on.\n");
	fflush(stdout);
}

/* TURN ZBA OFF.
 * @param:	None.
 * @return:	None.
 */
void ZBA_turn_off(void) {
	// Play sound.
	SOUND_play(&zba_sound_off);
	printf("ZBA *** Turn off.\n");
	fflush(stdout);
}
