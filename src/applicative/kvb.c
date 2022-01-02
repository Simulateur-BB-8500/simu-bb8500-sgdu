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
	SOUND_Context kvb_sound_turnon;
	SOUND_Context kvb_sound_turnoff;
	SOUND_Context kvb_sound_urgency;
} KVB_Context;

/*** KVB local global variables ***/

static KVB_Context kvb_ctx;

/*** KVB functions ***/

/* INIT KVB CALCULATOR.
 * @param:	None.
 * @return:	None.
 */
void KVB_Init(void) {
	// Init sounds.
	SOUND_Init(&(kvb_ctx.kvb_sound_turnon), "kvb_turnon.wav", KVB_AUDIO_GAIN);
	SOUND_SetVolume(&(kvb_ctx.kvb_sound_turnon), 1.0); // No fade effect required.
	SOUND_Init(&(kvb_ctx.kvb_sound_turnoff), "kvb_turnoff.wav", KVB_AUDIO_GAIN);
	SOUND_SetVolume(&(kvb_ctx.kvb_sound_turnoff), 1.0); // No fade effect required.
	SOUND_Init(&(kvb_ctx.kvb_sound_urgency), "kvb_urgency.wav", KVB_AUDIO_GAIN);
	SOUND_SetVolume(&(kvb_ctx.kvb_sound_urgency), 1.0); // No fade effect required.
}

/* TURN KVB ON.
 * @param:	None.
 * @return:	None.
 */
void KVB_TurnOn(void) {
	// Play sound.
	SOUND_Play(&(kvb_ctx.kvb_sound_turnon));
	SOUND_Stop(&(kvb_ctx.kvb_sound_turnoff));
#ifdef KVB_LOG
	printf("KVB *** Turn on.\n");
#endif
}

/* TURN KVB OFF.
 * @param:	None.
 * @return:	None.
 */
void KVB_TurnOff(void) {
	// Play sound.
	SOUND_Play(&(kvb_ctx.kvb_sound_turnoff));
	SOUND_Stop(&(kvb_ctx.kvb_sound_turnon));
#ifdef KVB_LOG
	printf("KVB *** Turn off.\n");
#endif
}
