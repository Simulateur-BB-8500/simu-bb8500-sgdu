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

/*** SOUND structure ***/

typedef struct {
	FMOD_CHANNEL* fmod_channel;
	FMOD_SOUND* fmod_sound;
	uint32_t length_ms;
	float current_volume;
	float maximum_volume;
	float fade_start_volume;
	uint32_t fade_start_position_ms;
} SOUND_context_t;

/*** SOUND functions ***/

void SOUND_fmod_system_init();
void SOUND_init(SOUND_context_t* sound_ctx, const char* audio_file_path, float maximum_volume);
void SOUND_play(SOUND_context_t* sound_ctx);
void SOUND_stop(SOUND_context_t* sound_ctx);
void SOUND_set_volume(SOUND_context_t* sound_ctx, float new_volume);
uint32_t SOUND_get_length_ms(SOUND_context_t* sound_ctx);
uint32_t SOUND_get_position_ms(SOUND_context_t* sound_ctx);
void SOUND_set_position_ms(SOUND_context_t* sound_ctx, uint32_t new_position_ms);
FMOD_BOOL SOUND_is_playing(SOUND_context_t* sound_ctx);
void SOUND_save_fade_parameters(SOUND_context_t* sound_ctx);
uint8_t SOUND_fade_in(SOUND_context_t* sound_ctx, uint32_t fade_duration_ms);
uint8_t SOUND_fade_out(SOUND_context_t* sound_ctx, uint32_t fade_duration_ms);

#endif /* __SOUND_H__ */
