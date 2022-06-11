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
#include "stdio.h"
#include "time.h"

/*** KVB local macros ***/

#define KVB_LOG

/*** KVB local structures ***/

typedef struct {
	SOUND_context_t sound_turnon;
	SOUND_context_t sound_turnoff;
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
	SOUND_init(&(kvb_ctx.sound_turnon), "kvb_turnon.wav", KVB_AUDIO_GAIN);
	SOUND_set_volume(&(kvb_ctx.sound_turnon), 1.0); // No fade effect required.
	SOUND_init(&(kvb_ctx.sound_turnoff), "kvb_turnoff.wav", KVB_AUDIO_GAIN);
	SOUND_set_volume(&(kvb_ctx.sound_turnoff), 1.0); // No fade effect required.
	SOUND_init(&(kvb_ctx.sound_urgency), "kvb_urgency.wav", KVB_AUDIO_GAIN);
	SOUND_set_volume(&(kvb_ctx.sound_urgency), 1.0); // No fade effect required.
}

/* TURN KVB ON.
 * @param:	None.
 * @return:	None.
 */
void KVB_turn_on(void) {
	// Play sound.
	SOUND_play(&(kvb_ctx.sound_turnon));
	SOUND_stop(&(kvb_ctx.sound_turnoff));
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
	SOUND_play(&(kvb_ctx.sound_turnoff));
	SOUND_stop(&(kvb_ctx.sound_turnon));
#ifdef KVB_LOG
	printf("KVB *** Turn off.\n");
#endif
}
