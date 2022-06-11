/*
 * s.c
 *
 *  Created on: 10 may 2020
 *      Author: Ludo
 */

#include "whistle.h"

#include "mixer.h"
#include "openrails.h"
#include "sound.h"
#include "stdio.h"

/*** WHISTLE local macros ***/

#define WHISTLE_LOG

/*** WHISTLE local structures ***/

typedef enum {
	WHISTLE_STATE_HIGH_TONE,
	WHISTLE_STATE_NEUTRAL,
	WHISTLE_STATE_LOW_TONE
} WHISTLE_state_t;

typedef struct {
	// Sounds.
	SOUND_context_t low_tone_sound;
	SOUND_context_t low_tone_end_sound;
	SOUND_context_t high_tone_sound;
	SOUND_context_t high_tone_end_sound;
	// State.
	WHISTLE_state_t s_state;
} WHISTLE_Context;

/*** WHISTLE local global variables ***/

static WHISTLE_Context s_ctx;

/*** WHISTLE functions ***/

/* INIT WHISTLE MODULE.
 * @param:	None.
 * @return:	None.
 */
void WHISTLE_init(void) {
	// Init sounds.
	SOUND_init(&(s_ctx.low_tone_sound), "low_tone.wav", WHISTLE_AUDIO_GAIN);
	SOUND_set_volume(&(s_ctx.low_tone_sound), 1.0); // No fade effect required.
	SOUND_init(&(s_ctx.low_tone_end_sound), "low_tone_end.wav", WHISTLE_AUDIO_GAIN);
	SOUND_set_volume(&(s_ctx.low_tone_end_sound), 1.0); // No fade effect required.
	SOUND_init(&(s_ctx.high_tone_sound), "high_tone.wav", WHISTLE_AUDIO_GAIN);
	SOUND_set_volume(&(s_ctx.high_tone_sound), 1.0); // No fade effect required.
	SOUND_init(&(s_ctx.high_tone_end_sound), "high_tone_end.wav", WHISTLE_AUDIO_GAIN);
	SOUND_set_volume(&(s_ctx.high_tone_end_sound), 1.0); // No fade effect required.
	// Init context.
	s_ctx.s_state = WHISTLE_STATE_NEUTRAL;
}

/* PLAY HIGH TONE HORN.
 * @param:	None.
 * @return:	None.
 */
void WHISTLE_high_tone(void) {
	// Play sound.
	SOUND_play(&(s_ctx.high_tone_sound));
	// Update state.
	s_ctx.s_state = WHISTLE_STATE_HIGH_TONE;
#ifdef WHISTLE_LOG
	printf("WHISTLE *** High tone.\n");
	fflush(stdout);
#endif
}

/* PLAY LOW TONE HORN.
 * @param:	None.
 * @return:	None.
 */
void WHISTLE_low_tone(void) {
	// Play sound.
	SOUND_play(&(s_ctx.low_tone_sound));
	// Update state.
	s_ctx.s_state = WHISTLE_STATE_LOW_TONE;
#ifdef WHISTLE_LOG
	printf("WHISTLE *** Low tone.\n");
	fflush(stdout);
#endif
}

/* TURN HORN OFF.
 * @param:	None.
 * @eturn:	None.
 */
void WHISTLE_neutral(void) {
	// Check state.
	switch (s_ctx.s_state) {
	case WHISTLE_STATE_LOW_TONE:
		// End low tone.
		SOUND_play(&(s_ctx.low_tone_end_sound));
		SOUND_stop(&(s_ctx.low_tone_sound));
		break;
	case WHISTLE_STATE_HIGH_TONE:
		// End high tone.
		SOUND_play(&(s_ctx.high_tone_end_sound));
		SOUND_stop(&(s_ctx.high_tone_sound));
		break;
	default:
		break;
	}
	// Update state.
	s_ctx.s_state = WHISTLE_STATE_NEUTRAL;
#ifdef WHISTLE_LOG
	printf("WHISTLE *** Neutral.\n");
	fflush(stdout);
#endif
}
