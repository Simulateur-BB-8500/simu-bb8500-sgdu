/*
 * zvm.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "zvm.h"

#include "mixer.h"
#include "sound.h"
#include "stdio.h"

/*** ZVM local macros ***/

#define ZVM_FADE_DURATION_MS	1000
#define ZVM_FADE_MARGIN_MS		1000 // Added to fade duration.

/*** ZVM local stuctures ***/

typedef enum {
	ZVM_STATE_OFF,
	ZVM_STATE_TURNON,
	ZVM_STATE_TURNON_TO_ON1,
	ZVM_STATE_ON1,
	ZVM_STATE_ON1_TO_ON2,
	ZVM_STATE_ON2,
	ZVM_STATE_ON2_TO_ON1,
	ZVM_STATE_TURNOFF
} ZVM_state_t;

typedef struct {
	SOUND_context_t sound_turn_on;
	SOUND_context_t sound_on1;
	SOUND_context_t sound_on2;
	SOUND_context_t sound_turn_off;
	unsigned char on;
	ZVM_state_t state;
} ZVM_Context;

/*** ZVM local global variables ***/

static ZVM_Context zvm_ctx;

/*** ZVM functions ***/

/* INIT ZVM SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void ZVM_init(void) {
	// Init sounds.
	SOUND_init(&(zvm_ctx.sound_turn_on), "turn_on.wav", ZVM_AUDIO_GAIN);
	SOUND_init(&(zvm_ctx.sound_on1), "on.wav", ZVM_AUDIO_GAIN);
	SOUND_init(&(zvm_ctx.sound_on2), "on.wav", ZVM_AUDIO_GAIN);
	SOUND_init(&(zvm_ctx.sound_turn_off), "turn_off.wav", ZVM_AUDIO_GAIN);
	// Init state machine.
	zvm_ctx.on = 0;
	zvm_ctx.state = ZVM_STATE_OFF;
}

/* TURN MOTORS FANS ON.
 * @param:	None.
 * @return:	None.
 */
void ZVM_turn_on(void) {
	zvm_ctx.on = 1;
	printf("ZVM *** Turn on.\n");
	fflush(stdout);
}

/* TURN MOTORS FANS OFF.
 * @param:	None.
 * @return:	None.
 */
void ZVM_turn_off(void) {
	zvm_ctx.on = 0;
	printf("ZVM *** Turn off.\n");
	fflush(stdout);
}

/* MAIN TASK OF ZVM SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void ZVM_task(void) {
	// Local variables.
	unsigned char turn_on_fade_end = 0;
	unsigned char on1_fade_end = 0;
	unsigned char on2_fade_end = 0;
	unsigned char turn_off_fade_end = 0;
	// Perform internal state machine.
	switch (zvm_ctx.state) {
	case ZVM_STATE_OFF:
		if (zvm_ctx.on != 0) {
			// Start playing turn-on sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_on), 1.0); // No fade-in effect required.
			SOUND_play(&(zvm_ctx.sound_turn_on));
			zvm_ctx.state = ZVM_STATE_TURNON;
		}
		break;
	case ZVM_STATE_TURNON:
		if (zvm_ctx.on == 0) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
			// Start playing turn-off sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_off), 0.0);
			SOUND_play(&(zvm_ctx.sound_turn_off));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_off));
			zvm_ctx.state = ZVM_STATE_TURNOFF;
		}
		else {
			if ((SOUND_get_position_ms(&(zvm_ctx.sound_turn_on))) > ((SOUND_get_length_ms(&(zvm_ctx.sound_turn_on))) - ZVM_FADE_DURATION_MS - ZVM_FADE_MARGIN_MS)) {
				// Save turn-on volume.
				SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
				// Start playing On1 sound.
				SOUND_set_volume(&(zvm_ctx.sound_on1), 0.0);
				SOUND_play(&(zvm_ctx.sound_on1));
				SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
				// Stop playing turn-off sound (in case it was running).
				SOUND_stop(&(zvm_ctx.sound_turn_off));
				zvm_ctx.state = ZVM_STATE_TURNON_TO_ON1;
			}
		}
		break;
	case ZVM_STATE_TURNON_TO_ON1:
		if (zvm_ctx.on == 0) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
			// Start playing turn-off sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_off), 0.0);
			SOUND_play(&(zvm_ctx.sound_turn_off));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_off));
			zvm_ctx.state = ZVM_STATE_TURNOFF;
		}
		else {
			// Perform turn-on fade-out and On1 fade-in.
			on1_fade_end = SOUND_fade_in(&(zvm_ctx.sound_on1), ZVM_FADE_DURATION_MS);
			turn_on_fade_end = SOUND_fade_out(&(zvm_ctx.sound_turn_on), ZVM_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((on1_fade_end > 0) && (turn_on_fade_end > 0)) {
				// Stop turn-on sound.
				SOUND_stop(&(zvm_ctx.sound_turn_on));
				zvm_ctx.state = ZVM_STATE_ON1;
			}
		}
		break;
	case ZVM_STATE_ON1:
		if (zvm_ctx.on == 0) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
			// Start playing turn-off sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_off), 0.0);
			SOUND_play(&(zvm_ctx.sound_turn_off));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_off));
			zvm_ctx.state = ZVM_STATE_TURNOFF;
		}
		else {
			if ((SOUND_get_position_ms(&(zvm_ctx.sound_on1))) > ((SOUND_get_length_ms(&(zvm_ctx.sound_on1))) - ZVM_FADE_DURATION_MS - ZVM_FADE_MARGIN_MS)) {
				// Save On1 volume.
				SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
				// Start playing On2 sound.
				SOUND_set_volume(&(zvm_ctx.sound_on2), 0.0);
				SOUND_play(&(zvm_ctx.sound_on2));
				SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
				zvm_ctx.state = ZVM_STATE_ON1_TO_ON2;
			}
		}
		break;
	case ZVM_STATE_ON1_TO_ON2:
		if (zvm_ctx.on == 0) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
			// Start playing turn-off sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_off), 0.0);
			SOUND_play(&(zvm_ctx.sound_turn_off));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_off));
			zvm_ctx.state = ZVM_STATE_TURNOFF;
		}
		else {
			// Perform On1 fade-out and On2 fade-in.
			on2_fade_end = SOUND_fade_in(&(zvm_ctx.sound_on2), ZVM_FADE_DURATION_MS);
			on1_fade_end = SOUND_fade_out(&(zvm_ctx.sound_on1), ZVM_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((on2_fade_end > 0) && (on1_fade_end > 0)) {
				// Stop On1 sound.
				SOUND_stop(&(zvm_ctx.sound_on1));
				zvm_ctx.state = ZVM_STATE_ON2;
			}
		}
		break;
	case ZVM_STATE_ON2:
		if (zvm_ctx.on == 0) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
			// Start playing turn-off sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_off), 0.0);
			SOUND_play(&(zvm_ctx.sound_turn_off));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_off));
			zvm_ctx.state = ZVM_STATE_TURNOFF;
		}
		else {
			if ((SOUND_get_position_ms(&(zvm_ctx.sound_on2))) > ((SOUND_get_length_ms(&(zvm_ctx.sound_on2))) - ZVM_FADE_DURATION_MS - ZVM_FADE_MARGIN_MS)) {
				// Save On2 volume.
				SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
				// Start playing On1 sound.
				SOUND_set_volume(&(zvm_ctx.sound_on1), 0.0);
				SOUND_play(&(zvm_ctx.sound_on1));
				SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
				zvm_ctx.state = ZVM_STATE_ON2_TO_ON1;
			}
		}
		break;
	case ZVM_STATE_ON2_TO_ON1:
		if (zvm_ctx.on == 0) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_on));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on1));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_on2));
			// Start playing turn-off sound.
			SOUND_set_volume(&(zvm_ctx.sound_turn_off), 0.0);
			SOUND_play(&(zvm_ctx.sound_turn_off));
			SOUND_save_fade_parameters(&(zvm_ctx.sound_turn_off));
			zvm_ctx.state = ZVM_STATE_TURNOFF;
		}
		else {
			// Perform On2 fade-out and On1 fade-in.
			on1_fade_end = SOUND_fade_in(&(zvm_ctx.sound_on1), ZVM_FADE_DURATION_MS);
			on2_fade_end = SOUND_fade_out(&(zvm_ctx.sound_on2), ZVM_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((on1_fade_end > 0) && (on2_fade_end > 0)) {
				// Stop On2 sound.
				SOUND_stop(&(zvm_ctx.sound_on2));
				zvm_ctx.state = ZVM_STATE_ON1;
			}
		}
		break;
	case ZVM_STATE_TURNOFF:
		// Perform On1, On2 and turn-on fade-out and turn-off fade-in.
		turn_off_fade_end = SOUND_fade_in(&(zvm_ctx.sound_turn_off), ZVM_FADE_DURATION_MS);
		turn_on_fade_end = SOUND_fade_out(&(zvm_ctx.sound_turn_on), ZVM_FADE_DURATION_MS);
		on1_fade_end = SOUND_fade_out(&(zvm_ctx.sound_on1), ZVM_FADE_DURATION_MS);
		on2_fade_end = SOUND_fade_out(&(zvm_ctx.sound_on2), ZVM_FADE_DURATION_MS);
		// Change state when effect is complete.
		if ((turn_off_fade_end > 0) && (turn_on_fade_end > 0) && (on1_fade_end > 0) && (on2_fade_end > 0)) {
			// Stop TurnOn, On1 and On2 sounds.
			SOUND_stop(&(zvm_ctx.sound_turn_on));
			SOUND_stop(&(zvm_ctx.sound_on1));
			SOUND_stop(&(zvm_ctx.sound_on2));
			zvm_ctx.state = ZVM_STATE_OFF;
		}
		break;
	default:
		// Unknown state.
		zvm_ctx.state = ZVM_STATE_OFF;
		break;
	}
}
