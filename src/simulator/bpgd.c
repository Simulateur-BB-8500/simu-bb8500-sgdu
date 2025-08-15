/*
 * bpgd.c
 *
 *  Created on: 25 dec. 2023
 *      Author: Ludo
 */

#include "bpgd.h"

#include "error.h"
#include "log.h"
#include "mixer.h"
#include "orts_shortcut.h"
#include "sound.h"
#include "stdint.h"
#include "time.h"

/*** BPGD local macros ***/

#define BPGD_KEYBOARD_PRESS_PERIOD_MS   500

/*** BPGD local structures ***/

/*******************************************************************/
typedef struct {
    SOUND_context_t sound_turn_on;
    BPGD_state_t state;
    uint32_t keyboard_time;
} BPGD_context_t;

/*** BPGD local global variables ***/

static BPGD_context_t bpgd_ctx;

/*** BPGD functions ***/

/*******************************************************************/
BPGD_status_t BPGD_init(void) {
    // Local variables.
    BPGD_status_t status = BPGD_SUCCESS;
    SOUND_status_t sound_status = SOUND_SUCCESS;
    // Init context.
    bpgd_ctx.state = BPGD_STATE_LAST;
    bpgd_ctx.keyboard_time = 0;
    // Init sounds.
    sound_status = SOUND_init(&(bpgd_ctx.sound_turn_on), "pbl2_turn_on.wav", PBL2_AUDIO_GAIN);
    SOUND_stack_exit_error(BPGD_ERROR_DRIVER_SOUND);
errors:
    LOG_ERROR(status, BPGD_SUCCESS);
    return status;
}

/*******************************************************************/
BPGD_status_t BPGD_set_state(BPGD_state_t state) {
    // Local variables.
    BPGD_status_t status = BPGD_SUCCESS;
    SOUND_status_t sound_status = SOUND_SUCCESS;
    // Check state.
    switch (state) {
    case BPGD_STATE_ON:
        // Check state change.
        if (bpgd_ctx.state != BPGD_STATE_ON) {
#ifdef LOG_BPGD
            LOG_trace(LOG_COLOR_WHITE, "state=BPGD_STATE_ON");
#endif
            // Play sound.
            sound_status = SOUND_play(&(bpgd_ctx.sound_turn_on), 0);
            SOUND_stack_exit_error(BPGD_ERROR_DRIVER_SOUND);
        }
        break;
    default:
        status = BPGD_ERROR_STATE;
        goto errors;
    }
    // Update local state.
    bpgd_ctx.state = state;
errors:
    LOG_ERROR(status, BPGD_SUCCESS);
    return status;
}

/*******************************************************************/
BPGD_status_t BPGD_process(void) {
    // Local variables.
    BPGD_status_t status = BPGD_SUCCESS;
    SOUND_status_t sound_status = SOUND_SUCCESS;
    KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
    // Process sounds.
    sound_status = SOUND_process(&(bpgd_ctx.sound_turn_on));
    SOUND_stack_exit_error(BPGD_ERROR_DRIVER_SOUND);
    // Automatically come back to off state when the sound is finished.
    if ((bpgd_ctx.state == BPGD_STATE_ON) && (bpgd_ctx.sound_turn_on.is_playing == 0)) {
        bpgd_ctx.state = BPGD_STATE_OFF;
    }
    // Synchronize game.
    if ((bpgd_ctx.state == BPGD_STATE_ON) && (TIME_get_milliseconds() > (bpgd_ctx.keyboard_time + BPGD_KEYBOARD_PRESS_PERIOD_MS))) {
        // Update time.
        bpgd_ctx.keyboard_time = TIME_get_milliseconds();
        // Release FPB.
        keyboard_status = KEYBOARD_single_press(&ORTS_SHORTCUT_FPB_RELEASE, ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT);
        KEYBOARD_stack_exit_error(BPGD_ERROR_DRIVER_KEYBOARD);
    }
errors:
    LOG_ERROR(status, BPGD_SUCCESS);
    return status;
}

