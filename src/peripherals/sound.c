/*
 * sound.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "sound.h"

#include "error.h"
#include "fmod.h"
#include "fmod_common.h"
#include "log.h"
#include "math.h"
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

/*** SOUND local macros ***/

#define SOUND_FMOD_NUMBER_OF_CHANNELS			32
#define SOUND_AUDIO_FILE_NAME_MAXIMUM_LENGTH	100
#define SOUND_AUDIO_FILES_FOLDER_PATH			"C:/Users/User/Documents/git/ls-agiu/audio/"

#define SOUND_FADE_IN_START_OFFSET				0.01
//#define SOUND_FADE_EQUATION_LINEAR
#define SOUND_FADE_EQUATION_TRIGONOMETRIC
//#define SOUND_FADE_EQUATION_ELLIPTIC

/*** SOUND local global variables ***/

static FMOD_SYSTEM* sound_fmod_system;

/*** SOUND local functions ***/

/*******************************************************************/
#define FMOD_exit_error(error_code) { if (fmod_status != FMOD_OK) { status = error_code; goto errors; } }

/*******************************************************************/
#define FMOD_stack_error(void) { if (fmod_status != FMOD_OK) { ERROR_stack_add((ERROR_BASE_FMOD * ERROR_BASE_STEP) + fmod_status); } }

/*******************************************************************/
#define FMOD_stack_exit_error(error_code) { FMOD_stack_error(); FMOD_exit_error(error_code); }

/*******************************************************************/
SOUND_status_t _SOUND_set_volume(SOUND_context_t* sound_ctx, float new_volume) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	// Check parameters.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	if (new_volume > SOUND_AUDIO_VOLUME_MAX) {
		status = SOUND_ERROR_VOLUME_OVERFLOW;
		goto errors;
	}
	// Check flag.
	if ((sound_ctx -> is_playing) != 0) {
		// Set channel volume.
		fmod_status = FMOD_Channel_SetVolume((sound_ctx -> fmod_channel), (new_volume * (sound_ctx -> mixer_gain) * (sound_ctx -> gain)));
		FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	}
	// Update object.
	sound_ctx -> volume = new_volume;
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t _SOUND_set_fade_parameters(SOUND_context_t* sound_ctx, SOUND_fade_type_t fade_type, uint32_t fade_duration_ms) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	// Check parameters.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	if (fade_type >= SOUND_FADE_TYPE_LAST) {
		status = SOUND_ERROR_FADE_TYPE;
		goto errors;
	}
	// Update current position.
	status = SOUND_update(sound_ctx);
	if (status != SOUND_SUCCESS) goto errors;
	// Configure fade effect.
	(sound_ctx -> fade_effect).type = fade_type;
	(sound_ctx -> fade_effect).duration_ms = fade_duration_ms;
	(sound_ctx -> fade_effect).start_volume = (sound_ctx -> volume);
	(sound_ctx -> fade_effect).start_position_ms = (sound_ctx -> position_ms);
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t _SOUND_play(SOUND_context_t* sound_ctx) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	// Check parameter.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Check flag.
	if ((sound_ctx -> is_playing == 0)) {
		// Play sound.
		fmod_status = FMOD_System_PlaySound(sound_fmod_system, (sound_ctx -> fmod_sound), NULL, 0, &(sound_ctx -> fmod_channel));
		FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	}
	// Update parameters.
	(sound_ctx -> is_playing) = 1;
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t _SOUND_stop(SOUND_context_t* sound_ctx) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	// Check parameter.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Check flag.
	if ((sound_ctx -> is_playing != 0)) {
		// Stop sound.
		fmod_status = FMOD_Channel_Stop(sound_ctx -> fmod_channel);
		FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	}
errors:
	// Update flags.
	(sound_ctx -> is_playing) = 0;
	(sound_ctx -> play_request) = 0;
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*** SOUND functions ***/

/*******************************************************************/
SOUND_status_t SOUND_init_fmod_system(void) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	// Create system.
	fmod_status = FMOD_System_Create(&sound_fmod_system, FMOD_VERSION);
	FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	// Init system.
	fmod_status = FMOD_System_Init(sound_fmod_system, SOUND_FMOD_NUMBER_OF_CHANNELS, FMOD_INIT_NORMAL, NULL);
	FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_init(SOUND_context_t* sound_ctx, const char* audio_file_name, float mixer_gain) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	char audio_file_full_name[SOUND_AUDIO_FILE_NAME_MAXIMUM_LENGTH] = SOUND_AUDIO_FILES_FOLDER_PATH;
	// Check parameters.
	if ((sound_ctx == NULL) || (audio_file_name == NULL)) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	if (mixer_gain > SOUND_AUDIO_GAIN_MAX) {
		status = SOUND_ERROR_AUDIO_GAIN;
		goto errors;
	}
	// Init object.
	(sound_ctx -> play_request) = 0;
	(sound_ctx -> is_playing) = 0;
	(sound_ctx -> length_ms) = 0;
	(sound_ctx -> position_ms) = 0;
	(sound_ctx -> volume) = SOUND_AUDIO_VOLUME_MIN;
	(sound_ctx -> mixer_gain) = mixer_gain;
	(sound_ctx -> gain) = SOUND_AUDIO_GAIN_MAX;
	(sound_ctx -> fade_effect).type = SOUND_FADE_TYPE_OUT;
	(sound_ctx -> fade_effect).duration_ms = 0;
	(sound_ctx -> fade_effect).start_position_ms = 0;
	(sound_ctx -> fade_effect).start_volume = SOUND_AUDIO_VOLUME_MIN;
	// Create full name.
	strcat(audio_file_full_name, audio_file_name);
	// Structure initialization.
	fmod_status = FMOD_System_CreateSound(sound_fmod_system, audio_file_full_name, FMOD_2D | FMOD_CREATESTREAM, NULL, &(sound_ctx -> fmod_sound));
	FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	// Read audio length.
	fmod_status = FMOD_Sound_GetLength((sound_ctx -> fmod_sound), &(sound_ctx -> length_ms), FMOD_TIMEUNIT_MS);
	FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
#ifdef LOG_SOUND
	LOG("Open audio file %s (length=%dms)", audio_file_name, (sound_ctx -> length_ms));
#endif
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_single_play(SOUND_context_t* sound_ctx) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	// Stop sound.
	FMOD_Channel_Stop(sound_ctx -> fmod_channel);
	// Play sound.
	fmod_status = FMOD_System_PlaySound(sound_fmod_system, (sound_ctx -> fmod_sound), NULL, 0, &(sound_ctx -> fmod_channel));
	FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	// Set default volume.
	fmod_status = FMOD_Channel_SetVolume((sound_ctx -> fmod_channel), (SOUND_AUDIO_VOLUME_MAX * (sound_ctx -> mixer_gain) * (sound_ctx -> gain)));
	FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_play(SOUND_context_t* sound_ctx, uint32_t fade_duration_ms) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	// Check parameter.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Set request flag.
	(sound_ctx -> play_request) = 1;
	// Set fade parameters.
	status = _SOUND_set_fade_parameters(sound_ctx, SOUND_FADE_TYPE_IN, fade_duration_ms);
	if (status != SOUND_SUCCESS) goto errors;
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_stop(SOUND_context_t* sound_ctx, uint32_t fade_duration_ms) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	// Check parameter.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Set fade parameters.
	status = _SOUND_set_fade_parameters(sound_ctx, SOUND_FADE_TYPE_OUT, fade_duration_ms);
	if (status != SOUND_SUCCESS) goto errors;
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_set_gain(SOUND_context_t* sound_ctx, float gain) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	// Check parameter.
	if (gain > SOUND_AUDIO_GAIN_MAX) {
		status = SOUND_ERROR_AUDIO_GAIN;
		goto errors;
	}
	(sound_ctx -> gain) = gain;
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_update(SOUND_context_t* sound_ctx) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	FMOD_RESULT fmod_status = FMOD_OK;
	// Check parameters.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Read current position.
	if ((sound_ctx -> is_playing) != 0) {
		fmod_status = FMOD_Channel_GetPosition((sound_ctx -> fmod_channel), &(sound_ctx -> position_ms), FMOD_TIMEUNIT_MS);
		FMOD_stack_exit_error(SOUND_ERROR_DRIVER_FMOD);
	}
	else {
		(sound_ctx -> position_ms) = 0;
	}
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}

/*******************************************************************/
SOUND_status_t SOUND_process(SOUND_context_t* sound_ctx) {
	// Local variables.
	SOUND_status_t status = SOUND_SUCCESS;
	double new_volume = 0.0;
	double alpha = 0.0;
	double beta = 0.0;
	double gamma = 0.0;
	double p = 0.0;
	// Check parameter.
	if (sound_ctx == NULL) {
		status = SOUND_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Nothing to do if request flag is not set.
	if ((sound_ctx -> play_request) == 0) goto errors;
	// Update sound data.
	status = SOUND_update(sound_ctx);
	if (status != SOUND_SUCCESS) goto errors;
	// Copy parameters for better readability.
	alpha = (sound_ctx -> fade_effect).start_volume;
	beta = (float) ((sound_ctx -> fade_effect).start_position_ms);
	gamma = (float) ((sound_ctx -> fade_effect).duration_ms);
	p = (float) (sound_ctx -> position_ms);
	// Check current position.
	if (p >= (sound_ctx -> length_ms)) {
		new_volume = 0.0;
	}
	else {
		if (p >= (beta + gamma)) {
			// Clamp volume.
			switch ((sound_ctx -> fade_effect).type) {
			case SOUND_FADE_TYPE_IN:
				new_volume = SOUND_AUDIO_VOLUME_MAX;
				break;
			case SOUND_FADE_TYPE_OUT:
				new_volume = SOUND_AUDIO_VOLUME_MIN;
				break;
			default:
				status = SOUND_ERROR_FADE_TYPE;
				goto errors;
			}
		}
		else {
			if (p >= beta) {
				// Compute fade volume.
				switch ((sound_ctx -> fade_effect).type) {
				case SOUND_FADE_TYPE_IN:
					// Compute new volume.
#ifdef SOUND_FADE_EQUATION_LINEAR
					new_volume = alpha + ((SOUND_AUDIO_VOLUME_MAX - alpha) * ((p - beta)) / (gamma));
#endif
#ifdef SOUND_FADE_EQUATION_TRIGONOMETRIC
					new_volume = alpha + ((SOUND_AUDIO_VOLUME_MAX - alpha) * sin((M_PI * (p - beta)) / (2 * gamma)));
#endif
#ifdef SOUND_FADE_EQUATION_ELLIPTIC
					new_volume = alpha + ((SOUND_AUDIO_VOLUME_MAX - alpha) * sqrt(1.0 - pow(((p - beta - gamma) / (gamma)), 2.0)));
#endif
					// Add offset to ensure first computed volume is not 0.
					if (new_volume == 0.0) {
						new_volume = SOUND_FADE_IN_START_OFFSET;
					}
					break;
				case SOUND_FADE_TYPE_OUT:
					// Compute new volume.
#ifdef SOUND_FADE_EQUATION_LINEAR
					new_volume = alpha - (((alpha - SOUND_AUDIO_VOLUME_MIN) * (p - beta)) / (gamma));
#endif
#ifdef SOUND_FADE_EQUATION_TRIGONOMETRIC
					new_volume = SOUND_AUDIO_VOLUME_MIN + ((alpha - SOUND_AUDIO_VOLUME_MIN) * cos((M_PI * (p - beta)) / (2 * gamma)));
#endif
#ifdef SOUND_FADE_EQUATION_ELLIPTIC
					new_volume = SOUND_AUDIO_VOLUME_MIN + ((alpha - SOUND_AUDIO_VOLUME_MIN) * sqrt(1.0 - pow(((p - beta) / (gamma)), 2.0)));
#endif
					break;
				default:
					status = SOUND_ERROR_FADE_TYPE;
					goto errors;
				}
			}
			// Note: nothing to do if the current position is before the start position.
		}
	}
	// Start or stop depending on volume.
	if (new_volume > SOUND_AUDIO_VOLUME_MIN) {
		status = _SOUND_play(sound_ctx);
		if (status != SOUND_SUCCESS) goto errors;
	}
	else {
		status = _SOUND_stop(sound_ctx);
		if (status != SOUND_SUCCESS) goto errors;
	}
	// Set volume.
	status = _SOUND_set_volume(sound_ctx, (float) new_volume);
	if (status != SOUND_SUCCESS) goto errors;
errors:
	LOG_ERROR(status, SOUND_SUCCESS);
	return status;
}
