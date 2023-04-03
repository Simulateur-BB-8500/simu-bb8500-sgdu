/*
 * comp.c
 *
 *  Created on: 8 may 2020
 *      Author: Ludo
 */

#include "compressor.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** COMPRESSOR local macros ***/

#define COMPRESSOR_FADE_DURATION_MS		1000
#define COMPRESSOR_FADE_MARGIN_MS		1000 // Added to fade duration.
#define COMPRESSOR_AUTO_OFF_MARGIN_MS	2000 // Automatically return to off state when sound position reaches its duration minus this margin.

/*** COMPRESSOR local structures ***/

typedef enum {
	COMPRESSOR_STATE_OFF,
	COMPRESSOR_STATE_ZCA_REG_MIN_MAX,
	COMPRESSOR_STATE_ZCA_OFF,
	COMPRESSOR_STATE_DIRECT_TURNON,
	COMPRESSOR_STATE_DIRECT_TURNON_TO_ON1,
	COMPRESSOR_STATE_DIRECT_ON1,
	COMPRESSOR_STATE_DIRECT_ON1_TO_ON2,
	COMPRESSOR_STATE_DIRECT_ON2,
	COMPRESSOR_STATE_DIRECT_ON2_TO_ON1,
	COMPRESSOR_STATE_DIRECT_TURNOFF
} COMPRESSOR_state_t;

typedef enum {
	COMPRESSOR_SOUND_REQUEST_REG_MIN,
	COMPRESSOR_SOUND_REQUEST_REG_MAX,
	COMPRESSOR_SOUND_REQUEST_DIRECT,
	COMPRESSOR_SOUND_REQUEST_OFF
} COMPRESSOR_sound_request_t;

typedef struct {
	// Sounds.
	SOUND_context_t sound_zca_reg_min;
	SOUND_context_t sound_zca_reg_max;
	SOUND_context_t sound_zca_off;
	SOUND_context_t sound_zcd_on;
	SOUND_context_t sound_zcd_0;
	SOUND_context_t sound_zcd_1;
	SOUND_context_t sound_zcd_off;
	// State machine.
	COMPRESSOR_state_t comp_state;
	COMPRESSOR_sound_request_t sound_request;
} COMPRESSOR_context_t;

/*** COMPRESSOR local global variables ***/

static COMPRESSOR_context_t compressor_ctx;

/*** COMPRESSOR functions ***/

/* INIT COMPRESSOR SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void COMPRESSOR_init(void) {
	// Init sounds.
	SOUND_init(&(compressor_ctx.sound_zca_reg_min), "zca_reg_min.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_init(&(compressor_ctx.sound_zca_reg_max), "zca_reg_max.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_init(&(compressor_ctx.sound_zca_off), "zca_off.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_init(&(compressor_ctx.sound_zcd_on), "zcd_on.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_init(&(compressor_ctx.sound_zcd_0), "zcd.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_init(&(compressor_ctx.sound_zcd_1), "zcd.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_init(&(compressor_ctx.sound_zcd_off), "zcd_off.wav", COMPRESSOR_AUDIO_GAIN);
	// Init state machine.
	compressor_ctx.comp_state = COMPRESSOR_STATE_OFF;
	compressor_ctx.sound_request = COMPRESSOR_SOUND_REQUEST_OFF;
}

/* PLAY MINIMUM REGULATION SOUND.
 * @param:	None.
 * @return:	None.
 */
void COMPRESSOR_play_zca_regulation_min(void) {
	compressor_ctx.sound_request = COMPRESSOR_SOUND_REQUEST_REG_MIN;
	printf("COMPRESSOR *** Regulation min request.\n");
	fflush(stdout);
}

/* PLAY MAXIMUM REGULATION SOUND.
 * @param:	None.
 * @return:	None.
 */
void COMPRESSOR_play_zca_regulation_max(void) {
	compressor_ctx.sound_request = COMPRESSOR_SOUND_REQUEST_REG_MAX;
	printf("COMPRESSOR *** Regulation max request.\n");
	fflush(stdout);
}

/* PLAY DIRECT COMPRESSOR SOUND.
 * @param:	None.
 * @return:	None.
 */
void COMPRESSOR_play_zcd(void) {
	compressor_ctx.sound_request = COMPRESSOR_SOUND_REQUEST_DIRECT;
	printf("COMPRESSOR *** Direct request.\n");
	fflush(stdout);
}

/* PLAY TURN-OFF SOUND.
 * @param:	None.
 * @return:	None.
 */
void COMPRESSOR_off(void) {
	compressor_ctx.sound_request = COMPRESSOR_SOUND_REQUEST_OFF;
	printf("COMPRESSOR *** Regulation turn-off request.\n");
	fflush(stdout);
}

/* MAIN TASK OF COMPRESSOR SOUND MODULE.
 * @param:	None.
 * @return:	None.
 */
void COMPRESSOR_task(void) {
	// Local variables.
	uint8_t zca_reg_min_fade_end = 0;
	uint8_t zca_reg_max_fade_end = 0;
	uint8_t zca_off_fade_end = 0;
	uint8_t direct_turnon_fade_end = 0;
	uint8_t direct_on1_fade_end = 0;
	uint8_t direct_on2_fade_end = 0;
	uint8_t direct_turnoff_fade_end = 0;
	// Perform internal state machine.
	switch (compressor_ctx.comp_state) {
	case COMPRESSOR_STATE_OFF:
		if (compressor_ctx.sound_request == COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Start playing turn-on sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_on), 1.0); // No fade-in effect required.
			SOUND_play(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNON;
		}
		else {
			if (compressor_ctx.sound_request == COMPRESSOR_SOUND_REQUEST_REG_MIN) {
				// Start playing minimum regulation sound.
				SOUND_set_volume(&(compressor_ctx.sound_zca_reg_min), 1.0); // No fade-in effect required.
				SOUND_play(&(compressor_ctx.sound_zca_reg_min));
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_min));
				compressor_ctx.comp_state = COMPRESSOR_STATE_ZCA_REG_MIN_MAX;
			}
			else {
				if (compressor_ctx.sound_request == COMPRESSOR_SOUND_REQUEST_REG_MAX) {
					// Start playing maximum regulation sound.
					SOUND_set_volume(&(compressor_ctx.sound_zca_reg_max), 1.0); // No fade-in effect required.
					SOUND_play(&(compressor_ctx.sound_zca_reg_max));
					SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_max));
					compressor_ctx.comp_state = COMPRESSOR_STATE_ZCA_REG_MIN_MAX;
				}
			}
		}
		break;
	case COMPRESSOR_STATE_ZCA_REG_MIN_MAX:
		// Auto to direct mode.
		if (compressor_ctx.sound_request == COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all auto sounds.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_min));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_max));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_off));
			// Start playing turn-on sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_on), 1.0); // No fade-in effect required.
			SOUND_play(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNON;
		}
		else {
			if (compressor_ctx.sound_request == COMPRESSOR_SOUND_REQUEST_OFF) {
				// Save all regulation sounds.
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_min));
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_max));
				// Start playing turn-off sound.
				SOUND_set_volume(&(compressor_ctx.sound_zca_off), 0.0);
				SOUND_play(&(compressor_ctx.sound_zca_off));
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_off));
				compressor_ctx.comp_state = COMPRESSOR_STATE_ZCA_OFF;
			}
			else {
				if ((SOUND_get_position_ms(&(compressor_ctx.sound_zca_reg_min)) > (SOUND_get_length_ms(&(compressor_ctx.sound_zca_reg_min)) - COMPRESSOR_AUTO_OFF_MARGIN_MS)) ||
					(SOUND_get_position_ms(&(compressor_ctx.sound_zca_reg_max)) > (SOUND_get_length_ms(&(compressor_ctx.sound_zca_reg_max)) - COMPRESSOR_AUTO_OFF_MARGIN_MS))) {
					// Automatically return to off state.
					compressor_ctx.sound_request = COMPRESSOR_SOUND_REQUEST_OFF;
					compressor_ctx.comp_state = COMPRESSOR_STATE_OFF;
				}
			}
		}
		break;
	case COMPRESSOR_STATE_ZCA_OFF:
		// Auto to direct mode.
		if (compressor_ctx.sound_request == COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all auto sounds.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_min));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_reg_max));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zca_off));
			// Start playing direct turn-on sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_on), 1.0); // No fade-in effect required.
			SOUND_play(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNON;
		}
		else {
			// Perform turn-off fade-in and all other fade-out.
			zca_off_fade_end = SOUND_fade_in(&(compressor_ctx.sound_zca_off), COMPRESSOR_FADE_DURATION_MS);
			zca_reg_min_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zca_reg_min), COMPRESSOR_FADE_DURATION_MS);
			zca_reg_max_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zca_reg_max), COMPRESSOR_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((zca_off_fade_end > 0) && (zca_reg_min_fade_end > 0) && (zca_reg_max_fade_end > 0)) {
				// Stop regulation min and max sounds.
				SOUND_stop(&(compressor_ctx.sound_zca_reg_min));
				SOUND_stop(&(compressor_ctx.sound_zca_reg_max));
				compressor_ctx.comp_state = COMPRESSOR_STATE_OFF;
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_TURNON:
		// Perform required fade-out effects.
		if (compressor_ctx.sound_request != COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
			// Start playing turn-off sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_off), 0.0);
			SOUND_play(&(compressor_ctx.sound_zcd_off));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_off));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNOFF;
		}
		else {
			if ((SOUND_get_position_ms(&(compressor_ctx.sound_zcd_on))) > ((SOUND_get_length_ms(&(compressor_ctx.sound_zcd_on))) - COMPRESSOR_FADE_DURATION_MS - COMPRESSOR_FADE_MARGIN_MS)) {
				// Save turn-on volume.
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
				// Start playing On1 sound.
				SOUND_set_volume(&(compressor_ctx.sound_zcd_0), 0.0);
				SOUND_play(&(compressor_ctx.sound_zcd_0));
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
				// Stop playing turn-off sound (in case it was running).
				SOUND_stop(&(compressor_ctx.sound_zcd_off));
				compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNON_TO_ON1;
			}
			else {
				// Perform all required fade-out effects.
				SOUND_fade_out(&(compressor_ctx.sound_zca_reg_min), COMPRESSOR_FADE_DURATION_MS);
				SOUND_fade_out(&(compressor_ctx.sound_zca_reg_max), COMPRESSOR_FADE_DURATION_MS);
				SOUND_fade_out(&(compressor_ctx.sound_zca_off), COMPRESSOR_FADE_DURATION_MS);
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_TURNON_TO_ON1:
		if (compressor_ctx.sound_request != COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Stop any auto sound.
			SOUND_stop(&(compressor_ctx.sound_zca_reg_min));
			SOUND_stop(&(compressor_ctx.sound_zca_reg_max));
			SOUND_stop(&(compressor_ctx.sound_zca_off));
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
			// Start playing turn-off sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_off), 0.0);
			SOUND_play(&(compressor_ctx.sound_zcd_off));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_off));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNOFF;
		}
		else {
			// Perform turn-on fade-out and On1 fade-in.
			direct_on1_fade_end = SOUND_fade_in(&(compressor_ctx.sound_zcd_0), COMPRESSOR_FADE_DURATION_MS);
			direct_turnon_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zcd_on), COMPRESSOR_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((direct_on1_fade_end > 0) && (direct_turnon_fade_end > 0)) {
				// Stop turn-on sound.
				SOUND_stop(&(compressor_ctx.sound_zcd_on));
				compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_ON1;
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_ON1:
		if (compressor_ctx.sound_request != COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
			// Start playing turn-off sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_off), 0.0);
			SOUND_play(&(compressor_ctx.sound_zcd_off));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_off));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNOFF;
		}
		else {
			if ((SOUND_get_position_ms(&(compressor_ctx.sound_zcd_0))) > ((SOUND_get_length_ms(&(compressor_ctx.sound_zcd_0))) - COMPRESSOR_FADE_DURATION_MS - COMPRESSOR_FADE_MARGIN_MS)) {
				// Save On1 volume.
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
				// Start playing On2 sound.
				SOUND_set_volume(&(compressor_ctx.sound_zcd_1), 0.0);
				SOUND_play(&(compressor_ctx.sound_zcd_1));
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
				compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_ON1_TO_ON2;
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_ON1_TO_ON2:
		if (compressor_ctx.sound_request != COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
			// Start playing turn-off sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_off), 0.0);
			SOUND_play(&(compressor_ctx.sound_zcd_off));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_off));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNOFF;
		}
		else {
			// Perform On1 fade-out and On2 fade-in.
			direct_on2_fade_end = SOUND_fade_in(&(compressor_ctx.sound_zcd_1), COMPRESSOR_FADE_DURATION_MS);
			direct_on1_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zcd_0), COMPRESSOR_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((direct_on2_fade_end > 0) && (direct_on1_fade_end > 0)) {
				// Stop On1 sound.
				SOUND_stop(&(compressor_ctx.sound_zcd_0));
				compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_ON2;
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_ON2:
		if (compressor_ctx.sound_request != COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
			// Start playing turn-off sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_off), 0.0);
			SOUND_play(&(compressor_ctx.sound_zcd_off));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_off));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNOFF;
		}
		else {
			if ((SOUND_get_position_ms(&(compressor_ctx.sound_zcd_1))) > ((SOUND_get_length_ms(&(compressor_ctx.sound_zcd_1))) - COMPRESSOR_FADE_DURATION_MS - COMPRESSOR_FADE_MARGIN_MS)) {
				// Save On2 volume.
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
				// Start playing On1 sound.
				SOUND_set_volume(&(compressor_ctx.sound_zcd_0), 0.0);
				SOUND_play(&(compressor_ctx.sound_zcd_0));
				SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
				compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_ON2_TO_ON1;
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_ON2_TO_ON1:
		if (compressor_ctx.sound_request != COMPRESSOR_SOUND_REQUEST_DIRECT) {
			// Save all other sounds volume.
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_on));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_0));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_1));
			// Start playing turn-off sound.
			SOUND_set_volume(&(compressor_ctx.sound_zcd_off), 0.0);
			SOUND_play(&(compressor_ctx.sound_zcd_off));
			SOUND_save_fade_parameters(&(compressor_ctx.sound_zcd_off));
			compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_TURNOFF;
		}
		else {
			// Perform On2 fade-out and On1 fade-in.
			direct_on1_fade_end = SOUND_fade_in(&(compressor_ctx.sound_zcd_0), COMPRESSOR_FADE_DURATION_MS);
			direct_on2_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zcd_1), COMPRESSOR_FADE_DURATION_MS);
			// Change state when effect is complete.
			if ((direct_on1_fade_end > 0) && (direct_on2_fade_end > 0)) {
				// Stop On2 sound.
				SOUND_stop(&(compressor_ctx.sound_zcd_1));
				compressor_ctx.comp_state = COMPRESSOR_STATE_DIRECT_ON1;
			}
		}
		break;
	case COMPRESSOR_STATE_DIRECT_TURNOFF:
		// Perform turn-off fade-in and all other fade-out.
		direct_turnoff_fade_end = SOUND_fade_in(&(compressor_ctx.sound_zcd_off), COMPRESSOR_FADE_DURATION_MS);
		direct_turnon_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zcd_on), COMPRESSOR_FADE_DURATION_MS);
		direct_on1_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zcd_0), COMPRESSOR_FADE_DURATION_MS);
		direct_on2_fade_end = SOUND_fade_out(&(compressor_ctx.sound_zcd_1), COMPRESSOR_FADE_DURATION_MS);
		// Change state when effect is complete.
		if ((direct_turnoff_fade_end > 0) && (direct_turnon_fade_end > 0) && (direct_on1_fade_end > 0) && (direct_on2_fade_end > 0)) {
			// Stop TurnOn, On1 and On2 sounds.
			SOUND_stop(&(compressor_ctx.sound_zcd_on));
			SOUND_stop(&(compressor_ctx.sound_zcd_0));
			SOUND_stop(&(compressor_ctx.sound_zcd_1));
			compressor_ctx.comp_state = COMPRESSOR_STATE_OFF;
		}
		break;
	default:
		// Unknown state.
		compressor_ctx.comp_state = COMPRESSOR_STATE_OFF;
		break;
	}
}
