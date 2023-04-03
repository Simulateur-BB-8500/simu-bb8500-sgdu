/*
 * kvb.c
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#include "kvb.h"

#include "lsmcu.h"
#include "lssgiu.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** KVB local macros ***/

#define KVB_LOG

/*** KVB local structures ***/

typedef struct {
	SOUND_context_t sound_on;
	SOUND_context_t sound_off;
	SOUND_context_t sound_urgency;
} KVB_context_t;

/*** KVB local global variables ***/

static KVB_context_t kvb_ctx;

/*** KVB functions ***/

/* INIT KVB CALCULATOR.
 * @param:	None.
 * @return:	None.
 */
void KVB_init(void) {
	// Init sounds.
	SOUND_init(&(kvb_ctx.sound_on), "kvb_on.wav", KVB_AUDIO_GAIN);
	SOUND_set_volume(&(kvb_ctx.sound_on), 1.0); // No fade effect required.
	SOUND_init(&(kvb_ctx.sound_off), "kvb_off.wav", KVB_AUDIO_GAIN);
	SOUND_set_volume(&(kvb_ctx.sound_off), 1.0); // No fade effect required.
	SOUND_init(&(kvb_ctx.sound_urgency), "kvb_urgency.wav", KVB_AUDIO_GAIN);
	SOUND_set_volume(&(kvb_ctx.sound_urgency), 1.0); // No fade effect required.
}

/* TURN KVB ON.
 * @param:	None.
 * @return:	None.
 */
void KVB_turn_on(void) {
	// Play sound.
	SOUND_play(&(kvb_ctx.sound_on));
	SOUND_stop(&(kvb_ctx.sound_off));
#ifdef KVB_LOG
	printf("KVB *** Turn on.\n");
#endif
}

/* TURN KVB OFF.
 * @param:	None.
 * @return:	None.
 */
void KVB_turn_off(void) {
	// Play sound.
	SOUND_play(&(kvb_ctx.sound_off));
	SOUND_stop(&(kvb_ctx.sound_on));
#ifdef KVB_LOG
	printf("KVB *** Turn off.\n");
#endif
}
