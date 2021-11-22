/*
 * sound.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "sound.h"

#include "fmod.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

/*** SOUND local macros ***/

#define SOUND_FMOD_NUMBER_OF_CHANNELS			32
#define SOUND_AUDIO_FILE_NAME_MAXIMUM_LENGTH	100
#ifdef WINDOWS
#define SOUND_AUDIO_FILES_FOLDER_PATH			"C:/Users/Ludovic/Documents/Eclipse/LSSGKCU/wav/"
#endif
#ifdef LINUX
#define SOUND_AUDIO_FILES_FOLDER_PATH			"/home/ludo/git/lssgiu/wav/"
#endif
//#define SOUND_LOG

/*** SOUND local global variables ***/

static FMOD_SYSTEM* sound_fmod_system;
#ifdef SOUND_LOG
unsigned int a = 0;
unsigned int b = 0;
#endif

/*** SOUND functions ***/

/* INITIALISE FMOD SOUND SYSTEM.
 * @param:	None.
 * @return:	None.
 */
void SOUND_FmodSystemInit() {
	FMOD_System_Create(&sound_fmod_system, FMOD_VERSION);
	FMOD_System_Init(sound_fmod_system, SOUND_FMOD_NUMBER_OF_CHANNELS, FMOD_INIT_NORMAL, (void*) 0);
}

/* CREATE AN FMOD SOUND.
 * @param sound_ctx:		Pointer to sound structure.
 * @param audio_file_name:	Audio file full name ("x.wav").
 * @param maximum_volume:	Maximum normalized volume of the sound (0.0 to 1.0), see mixer.h.
 * @return:					None.
 */
void SOUND_Init(SOUND_Context* sound_ctx, const char* audio_file_name, float maximum_volume) {
	// Create full name.
	char audio_file_full_name[SOUND_AUDIO_FILE_NAME_MAXIMUM_LENGTH] = SOUND_AUDIO_FILES_FOLDER_PATH;
	strcat(audio_file_full_name, audio_file_name);
	// Structure initialisation.
	FMOD_RESULT fmod_result = FMOD_System_CreateSound(sound_fmod_system, audio_file_full_name, FMOD_2D | FMOD_CREATESTREAM, NULL, &(sound_ctx -> sound_fmod_sound));
	FMOD_Sound_GetLength((sound_ctx -> sound_fmod_sound), &(sound_ctx -> sound_length_ms), FMOD_TIMEUNIT_MS);
	sound_ctx -> sound_current_volume = 0.0;
	SOUND_SetVolume(sound_ctx, 0.0);
	sound_ctx -> sound_maximum_volume = maximum_volume;
	sound_ctx -> sound_fade_start_volume = 0.0;
	sound_ctx -> sound_fade_start_position_ms = 0;
	printf("SOUND *** Opening audio file %s: ", audio_file_name);
	if (fmod_result == FMOD_OK) {
		printf("OK.\n");
	}
	else {
		printf("Error.\n");
	}
	fflush(stdout);
}

/* PLAY A SOUND.
 * @param sound_ctx:	Sound to play.
 * @return: 			None.
 */
void SOUND_Play(SOUND_Context* sound_ctx) {
	FMOD_System_PlaySound(sound_fmod_system, (sound_ctx -> sound_fmod_sound), NULL, 0, &(sound_ctx -> sound_fmod_channel));
}

/* STOP A SOUND.
 * @param sound_ctx:	Sound to play.
 * @return: 			None.
 */
void SOUND_Stop(SOUND_Context* sound_ctx) {
	FMOD_Channel_Stop(sound_ctx -> sound_fmod_channel);
}

/* SET THE VOLUME OF A SOUND.
 * @param sound_ctx:	Sound to control.
 * @param new_volume: 	New volume of the sound.
 * @return:				None.
 */
void SOUND_SetVolume(SOUND_Context* sound_ctx, float new_volume) {
	FMOD_Channel_SetVolume((sound_ctx -> sound_fmod_channel), (new_volume * (sound_ctx -> sound_maximum_volume)));
	sound_ctx -> sound_current_volume = new_volume;
}

/* GET THE DURATION OF A SOUND.
 * @param sound_ctx:	Sound to analyse.
 * @return length_ms: 	Sound duration in ms.
 */
unsigned int SOUND_GetLengthMs(SOUND_Context* sound_ctx) {
	return (sound_ctx -> sound_length_ms);
}

/* GET THE CURRENT POSITION IN A SOUND.
 * @param sound_ctx:	Sound to analyse.
 * @return position_ms:	Current reading position in ms.
 */
unsigned int SOUND_GetPositionMs(SOUND_Context* sound_ctx) {
	unsigned int position_ms = 0;
	FMOD_Channel_GetPosition((sound_ctx -> sound_fmod_channel), &position_ms, FMOD_TIMEUNIT_MS);
	return position_ms;
}

/* SET THE CURRENT POSITION OF A SOUND.
 * @param sound_ctx:		Sound to control.
 * @param new_position_ms:	New reading position in ms.
 * @return: 				None.
 */
void SOUND_SetPosition(SOUND_Context* sound_ctx, unsigned int new_position_ms) {
	FMOD_Channel_SetPosition((sound_ctx -> sound_fmod_channel), new_position_ms, FMOD_TIMEUNIT_MS);
}

/* CHECK IS A SOUND IS CURRENTLY PLAYING.
 * @param sound_ctx:	Sound to analyse.
 * @return is_playing:	Non-zero value if the sound is currently playing, 0 otherwise.
 */
FMOD_BOOL SOUND_IsPlaying(SOUND_Context* sound_ctx) {
	FMOD_BOOL is_playing = 0;
	FMOD_Channel_IsPlaying((sound_ctx -> sound_fmod_channel), &is_playing);
	return is_playing;
}

/* SAVE FADE EFFECT PARAMETERS.
 * @param sound_ctx:	Sound to control.
 * @return:				None.
 */
void SOUND_SaveFadeParameters(SOUND_Context* sound_ctx) {
	sound_ctx -> sound_fade_start_position_ms = SOUND_GetPositionMs(sound_ctx);
	sound_ctx -> sound_fade_start_volume = (sound_ctx -> sound_current_volume);
}

/* PERFORM FADE-IN EFFECT.
 * @param sound_ctx:		Sound to control.
 * @param fade_duration_ms:	Fade effect duration in ms.
 * @return fade_end			'1' if the fade effect is finished, '0' otherwise.
 */
unsigned char SOUND_FadeIn(SOUND_Context* sound_ctx, unsigned int fade_duration_ms) {
	float fade_in_volume = (sound_ctx -> sound_current_volume);
	unsigned char fade_end = 0;
	// Ensure sound is playing and current position is greater or equal the start position.
	if ((SOUND_GetPositionMs(sound_ctx) >= (sound_ctx -> sound_fade_start_position_ms)) && (SOUND_IsPlaying(sound_ctx) > 0)) {
		if ((SOUND_GetPositionMs(sound_ctx) >= ((sound_ctx -> sound_fade_start_position_ms) + fade_duration_ms)) || (SOUND_GetPositionMs(sound_ctx) >= (sound_ctx -> sound_length_ms))) {
			// Clamp zone.
			fade_in_volume = 1.0;
			fade_end = 1;
		}
		else {
			// Fade zone: apply linear equation.
			fade_in_volume = (float) ((sound_ctx -> sound_fade_start_volume) + ((1.0 - (sound_ctx -> sound_fade_start_volume)) * ((float) (SOUND_GetPositionMs(sound_ctx)) - (float) (sound_ctx -> sound_fade_start_position_ms))) / ((float) fade_duration_ms));
		}
		// Apply new volume.
		SOUND_SetVolume(sound_ctx, fade_in_volume);
#ifdef SOUND_LOG
		if (TIME_GetMs() > a) {
			a = TIME_GetMs() + 100;
			printf("fade_in_volume=%f\n", fade_in_volume);
			fflush(stdout);
		}
#endif
	}
	else {
		// Error.
		fade_end = 1;
	}
	// Return end flag.
	return fade_end;
}

/* PERFORM FADE-OUT EFFECT.
 * @param sound_ctx:		Sound to control.
 * @param fade_duration_ms:	Fade effect duration in ms.
 * @return fade_end			'1' if the fade effect is finished, '0' otherwise.
 */
unsigned char SOUND_FadeOut(SOUND_Context* sound_ctx, unsigned int fade_duration_ms) {
	float fade_out_volume = (sound_ctx -> sound_current_volume);
	unsigned char fade_end = 0;
	// Ensure sound is playing and current position is greater or equal the start position.
	if ((SOUND_GetPositionMs(sound_ctx) >= (sound_ctx -> sound_fade_start_position_ms)) && (SOUND_IsPlaying(sound_ctx) > 0)) {
		if ((SOUND_GetPositionMs(sound_ctx) >= ((sound_ctx -> sound_fade_start_position_ms) + fade_duration_ms)) || (SOUND_GetPositionMs(sound_ctx) >= (sound_ctx -> sound_length_ms))) {
			// Clamp zone.
			fade_out_volume = 0.0;
			fade_end = 1;
		}
		else {
			// Fade zone: apply linear equation.
			fade_out_volume = (float) ((sound_ctx -> sound_fade_start_volume) - ((sound_ctx -> sound_fade_start_volume) * ((float) (SOUND_GetPositionMs(sound_ctx)) - (float) (sound_ctx -> sound_fade_start_position_ms))) / ((float) fade_duration_ms));
		}
		// Apply new volume.
		SOUND_SetVolume(sound_ctx, fade_out_volume);
#ifdef SOUND_LOG
		if (TIME_GetMs() > b) {
			b = TIME_GetMs() + 100;
			printf("fade_out_volume=%f\n", fade_out_volume);
			fflush(stdout);
		}
#endif
	}
	else {
		// Error.
		fade_end = 1;
	}
	// Return end flag.
	return fade_end;
}
