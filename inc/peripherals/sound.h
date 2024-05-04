/*
 * sound.h
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#include "fmod.h"
#include "stdint.h"

/*** SOUND macros ***/

#define SOUND_AUDIO_GAIN_MAX	1.0

#define SOUND_AUDIO_VOLUME_MIN	0.0
#define SOUND_AUDIO_VOLUME_MAX	1.0

/*** SOUND structures ***/

/*!******************************************************************
 * \enum SOUND_status_t
 * \brief SOUND driver error codes.
 *******************************************************************/
typedef enum {
	// Driver errors.
	SOUND_SUCCESS = 0,
	SOUND_ERROR_NULL_PARAMETER,
	SOUND_ERROR_AUDIO_GAIN,
	SOUND_ERROR_VOLUME_OVERFLOW,
	SOUND_ERROR_POSITION_OVERFLOW,
	SOUND_ERROR_FADE_TYPE,
	// Low level drivers errors.
	SOUND_ERROR_DRIVER_FMOD,
	// Last index.
	SERIAL_ERROR_LAST
} SOUND_status_t;

/*!******************************************************************
 * \enum SOUND_fade_type_t
 * \brief SOUND fade effect types.
 *******************************************************************/
typedef enum {
	SOUND_FADE_TYPE_IN = 0,
	SOUND_FADE_TYPE_OUT,
	SOUND_FADE_TYPE_LAST
} SOUND_fade_type_t;

/*!******************************************************************
 * \enum SOUND_fade_effect_t
 * \brief SOUND fade effect parameters.
 *******************************************************************/
typedef struct {
	SOUND_fade_type_t type;
	uint32_t duration_ms;
	uint32_t start_position_ms;
	float start_volume;
} SOUND_fade_effect_t;

/*!******************************************************************
 * \enum SOUND_context_t
 * \brief Sound context structure.
 *******************************************************************/
typedef struct {
	FMOD_CHANNEL* fmod_channel;
	FMOD_SOUND* fmod_sound;
	uint8_t play_request;
	uint8_t is_playing;
	uint32_t length_ms;
	uint32_t position_ms;
	float volume;
	float mixer_gain;
	float gain;
	SOUND_fade_effect_t fade_effect;
} SOUND_context_t;

/*** SOUND functions ***/

/*!******************************************************************
 * \fn SOUND_status_t SOUND_get_fmod_version(uint16_t* product, uint8_t* major, uint8_t* minor)
 * \brief Get sound driver version.
 * \param[in]  	none
 * \param[out] 	product: FMOD product version.
 * \param[out] 	major: FMOD major version.
 * \param[out] 	minor: FMOD minor version.
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_get_fmod_version(uint16_t* product, uint8_t* major, uint8_t* minor);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_init_fmod_system(void)
 * \brief Init sound driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_init_fmod_system(void);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_init(SOUND_context_t* sound_ctx, const char* audio_file_path, float mixer_gain)
 * \brief Init sound object.
 * \param[in]  	sound_ctx: Sound to initialize.
 * \param[in]	audio_file_name: Audio file name.
 * \param[in]	mixer_gain: Normalized static mixer gain (0.0 to 1.0).
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_init(SOUND_context_t* sound_ctx, const char* audio_file_name, float mixer_gain);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_single_play(SOUND_context_t* sound_ctx)
 * \brief Play sound.
 * \param[in]  	sound_ctx: Sound to play.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_single_play(SOUND_context_t* sound_ctx);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_play(SOUND_context_t* sound_ctx)
 * \brief Play sound.
 * \param[in]  	sound_ctx: Sound to play.
 * \param[in]	fade_duration_ms: Fade in effect duration (disabled with 0).
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_play(SOUND_context_t* sound_ctx, uint32_t fade_duration_ms);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_stop(SOUND_context_t* sound_ctx)
 * \brief Stop sound.
 * \param[in]  	sound_ctx: Sound to stop.
 * \param[in]	fade_duration_ms: Fade in effect duration (disabled with 0).
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_stop(SOUND_context_t* sound_ctx, uint32_t fade_duration_ms);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_set_gain(SOUND_context_t* sound_ctx, float gain)
 * \brief Set sound dynamic gain.
 * \param[in]  	sound_ctx: Sound to stop.
 * \param[in]	gain: New gain to apply.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_set_gain(SOUND_context_t* sound_ctx, float gain);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_update(SOUND_context_t* sound_ctx)
 * \brief Update sound data.
 * \param[in]  	sound_ctx: Sound to update.
 * \param[out]	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_update(SOUND_context_t* sound_ctx);

/*!******************************************************************
 * \fn SOUND_status_t SOUND_process(SOUND_context_t* sound_ctx)
 * \brief Process sound effects.
 * \param[in]  	sound_ctx: Sound to process.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SOUND_status_t SOUND_process(SOUND_context_t* sound_ctx);

/*******************************************************************/
#define SOUND_exit_error(error_code) { if (sound_status != SOUND_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define SOUND_stack_error(void) { if (sound_status != SOUND_SUCCESS) { ERROR_stack_add((ERROR_BASE_SOUND * ERROR_BASE_STEP) + sound_status); } }

/*******************************************************************/
#define SOUND_stack_exit_error(error_code) { SOUND_stack_error(); SOUND_exit_error(error_code); }

#endif /* __SOUND_H__ */
