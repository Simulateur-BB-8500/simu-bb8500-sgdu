/*
 * comp.c
 *
 *  Created on: 08 may 2020
 *      Author: Ludo
 */

#include "compressor.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

/*** COMPRESSOR local macros ***/

#define COMPRESSOR_FADE_DURATION_MS		1000
#define COMPRESSOR_FADE_DURATION_OFF_MS	500
#define COMPRESSOR_FADE_MARGIN_MS		100

#define COMPRESSOR_REQUEST_TABLE_SIZE	10

/*** COMPRESSOR local structures ***/

/*******************************************************************/
typedef enum {
	COMPRESSOR_INTERNAL_STATE_TURN_OFF,
	COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MIN,
	COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MAX,
	COMPRESSOR_INTERNAL_STATE_ZCD_TURN_ON,
	COMPRESSOR_INTERNAL_STATE_ZCD_ON_0,
	COMPRESSOR_INTERNAL_STATE_ZCD_ON_1,
	COMPRESSOR_INTERNAL_STATE_LAST
} COMPRESSOR_internal_state_t;

/*******************************************************************/
typedef struct {
	SOUND_context_t sound_zca_regulation_min;
	SOUND_context_t sound_zca_regulation_max;
	SOUND_context_t sound_zcd_turn_on;
	SOUND_context_t sound_zcd_on_0;
	SOUND_context_t sound_zcd_on_1;
	SOUND_context_t sound_zcx_turn_off;
	COMPRESSOR_request_t request[COMPRESSOR_REQUEST_TABLE_SIZE];
	uint8_t request_write_idx;
	uint8_t request_read_idx;
	COMPRESSOR_internal_state_t internal_state;
} COMPRESSOR_context_t;

/*** COMPRESSOR local global variables ***/

static COMPRESSOR_context_t compressor_ctx;

/*** COMPRESSOR functions ***/

/*******************************************************************/
COMPRESSOR_status_t COMPRESSOR_init(void) {
	// Local variables.
	COMPRESSOR_status_t status = COMPRESSOR_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	uint8_t idx = 0;
	// Init state machine.
	compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_TURN_OFF;
	for (idx=0 ; idx<COMPRESSOR_REQUEST_TABLE_SIZE ; idx++) compressor_ctx.request[idx] = COMPRESSOR_REQUEST_NONE;
	compressor_ctx.request_write_idx = 0;
	compressor_ctx.request_read_idx = 0;
	// Init sounds.
	sound_status = SOUND_init(&(compressor_ctx.sound_zca_regulation_min), "zca_regulation_min.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(compressor_ctx.sound_zca_regulation_max), "zca_regulation_max.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(compressor_ctx.sound_zcd_turn_on), "zcd_turn_on.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(compressor_ctx.sound_zcd_on_0), "zcd_on.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(compressor_ctx.sound_zcd_on_1), "zcd_on.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_init(&(compressor_ctx.sound_zcx_turn_off), "zcx_turn_off.wav", COMPRESSOR_AUDIO_GAIN);
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
errors:
	LOG_ERROR(status, COMPRESSOR_SUCCESS);
	return status;
}

/*******************************************************************/
COMPRESSOR_status_t COMPRESSOR_set_request(COMPRESSOR_request_t request) {
	// Local variables.
	COMPRESSOR_status_t status = COMPRESSOR_SUCCESS;
	// Check parameter.
	if (request >= COMPRESSOR_REQUEST_LAST) {
		status = COMPRESSOR_ERROR_REQUEST;
		goto errors;
	}
	// Register request and increment index.
	compressor_ctx.request[compressor_ctx.request_write_idx] = request;
	compressor_ctx.request_write_idx = (compressor_ctx.request_write_idx + 1) % COMPRESSOR_REQUEST_TABLE_SIZE;
#ifdef LOG_COMPRESSOR
	LOG("request=%d", request);
#endif
errors:
	LOG_ERROR(status, COMPRESSOR_SUCCESS);
	return status;
}

/*******************************************************************/
COMPRESSOR_status_t COMPRESSOR_process(void) {
	// Local variables.
	COMPRESSOR_status_t status = COMPRESSOR_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	// Check request index.
	if (compressor_ctx.request_read_idx != compressor_ctx.request_write_idx) {
		// Process request.
		switch (compressor_ctx.request[compressor_ctx.request_read_idx]) {
		case COMPRESSOR_REQUEST_NONE:
			break;
		case COMPRESSOR_REQUEST_ZCA_REGULATION_MIN:
			// Check state.
			if (compressor_ctx.internal_state != COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MIN) {
				// Play sound.
				sound_status = SOUND_play(&(compressor_ctx.sound_zca_regulation_min), 0);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Stop all other sounds.
				sound_status = SOUND_stop(&(compressor_ctx.sound_zca_regulation_max), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_turn_on), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_1), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcx_turn_off), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Update state.
				compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MIN;
			}
			break;
		case COMPRESSOR_REQUEST_ZCA_REGULATION_MAX:
			// Check state.
			if (compressor_ctx.internal_state != COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MAX) {
				// Play sound.
				sound_status = SOUND_play(&(compressor_ctx.sound_zca_regulation_max), 0);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Stop all other sounds.
				sound_status = SOUND_stop(&(compressor_ctx.sound_zca_regulation_min), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_turn_on), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_1), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcx_turn_off), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Update state.
				compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MAX;
			}
			break;
		case COMPRESSOR_REQUEST_ZCD_ON:
			// Check state.
			if (compressor_ctx.internal_state != COMPRESSOR_INTERNAL_STATE_ZCD_TURN_ON) {
				// Play sound.
				sound_status = SOUND_play(&(compressor_ctx.sound_zcd_turn_on), 0);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Stop all other sounds.
				sound_status = SOUND_stop(&(compressor_ctx.sound_zca_regulation_min), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zca_regulation_max), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_1), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcx_turn_off), COMPRESSOR_FADE_DURATION_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Update state.
				compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_ZCD_TURN_ON;
			}
			break;
		case COMPRESSOR_REQUEST_ZCX_OFF:
			// Check state.
			if (compressor_ctx.internal_state != COMPRESSOR_INTERNAL_STATE_TURN_OFF) {
				// Play sound.
				sound_status = SOUND_play(&(compressor_ctx.sound_zcx_turn_off), COMPRESSOR_FADE_DURATION_OFF_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Stop all other sounds.
				sound_status = SOUND_stop(&(compressor_ctx.sound_zca_regulation_min), COMPRESSOR_FADE_DURATION_OFF_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zca_regulation_max), COMPRESSOR_FADE_DURATION_OFF_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_turn_on), COMPRESSOR_FADE_DURATION_OFF_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_OFF_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_1), COMPRESSOR_FADE_DURATION_OFF_MS);
				SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
				// Update state.
				compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_TURN_OFF;
			}
			break;
		default:
			status = COMPRESSOR_ERROR_REQUEST;
			goto errors;
		}
		// Clear request and increment index.
		compressor_ctx.request[compressor_ctx.request_read_idx] = COMPRESSOR_REQUEST_NONE;
		compressor_ctx.request_read_idx = (compressor_ctx.request_read_idx + 1) % COMPRESSOR_REQUEST_TABLE_SIZE;
	}
	// Update all sounds.
	sound_status = SOUND_update(&(compressor_ctx.sound_zca_regulation_min));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(compressor_ctx.sound_zca_regulation_max));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(compressor_ctx.sound_zcd_turn_on));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(compressor_ctx.sound_zcd_on_0));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(compressor_ctx.sound_zcd_on_1));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_update(&(compressor_ctx.sound_zcx_turn_off));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	// Perform state machine.
	switch (compressor_ctx.internal_state) {
	case COMPRESSOR_INTERNAL_STATE_TURN_OFF:
		// Nothing to do.
		break;
	case COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MIN:
		// Check ZCA end.
		if (compressor_ctx.sound_zca_regulation_min.is_playing == 0) {
			// Automatically come back to OFF state.
			compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_TURN_OFF;
		}
		break;
	case COMPRESSOR_INTERNAL_STATE_ZCA_REGULATION_MAX:
		// Check ZCA end.
		if (compressor_ctx.sound_zca_regulation_max.is_playing == 0) {
			// Automatically come back to OFF state.
			compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_TURN_OFF;
		}
		break;
	case COMPRESSOR_INTERNAL_STATE_ZCD_TURN_ON:
		// Check position.
		if ((compressor_ctx.sound_zcd_turn_on.position_ms) > (compressor_ctx.sound_zcd_turn_on.length_ms - COMPRESSOR_FADE_DURATION_MS - COMPRESSOR_FADE_MARGIN_MS)) {
			// Perform overlap.
			sound_status = SOUND_play(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_MS);
			SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_turn_on), COMPRESSOR_FADE_DURATION_MS);
			SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
			// Update state.
			compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_ZCD_ON_0;
		}
		break;
	case COMPRESSOR_INTERNAL_STATE_ZCD_ON_0:
		// Check position.
		if ((compressor_ctx.sound_zcd_on_0.position_ms) > (compressor_ctx.sound_zcd_on_0.length_ms - COMPRESSOR_FADE_DURATION_MS - COMPRESSOR_FADE_MARGIN_MS)) {
			// Perform overlap.
			sound_status = SOUND_play(&(compressor_ctx.sound_zcd_on_1), COMPRESSOR_FADE_DURATION_MS);
			SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_MS);
			SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
			// Update state.
			compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_ZCD_ON_1;
		}
		break;
	case COMPRESSOR_INTERNAL_STATE_ZCD_ON_1:
		// Check position.
		if ((compressor_ctx.sound_zcd_on_1.position_ms) > (compressor_ctx.sound_zcd_on_1.length_ms - COMPRESSOR_FADE_DURATION_MS - COMPRESSOR_FADE_MARGIN_MS)) {
			// Perform overlap.
			sound_status = SOUND_play(&(compressor_ctx.sound_zcd_on_0), COMPRESSOR_FADE_DURATION_MS);
			SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
			sound_status = SOUND_stop(&(compressor_ctx.sound_zcd_on_1), COMPRESSOR_FADE_DURATION_MS);
			SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
			// Update state.
			compressor_ctx.internal_state = COMPRESSOR_INTERNAL_STATE_ZCD_ON_0;
		}
		break;
	default:
		status = COMPRESSOR_ERROR_INTERNAL_STATE;
		goto errors;
	}
	// Process all sounds.
	sound_status = SOUND_process(&(compressor_ctx.sound_zca_regulation_min));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(compressor_ctx.sound_zca_regulation_max));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(compressor_ctx.sound_zcd_turn_on));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(compressor_ctx.sound_zcd_on_0));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(compressor_ctx.sound_zcd_on_1));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
	sound_status = SOUND_process(&(compressor_ctx.sound_zcx_turn_off));
	SOUND_stack_exit_error(COMPRESSOR_ERROR_DRIVER_SOUND);
errors:
	LOG_ERROR(status, COMPRESSOR_SUCCESS);
	return status;
}
