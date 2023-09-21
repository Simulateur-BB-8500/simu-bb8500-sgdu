/*
 * keyboard.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "keyboard.h"

#include "log.h"
#include "stdio.h"
#include "stdint.h"
#include "time.h"
#include "windows.h"

/*** KEYBOARD local macros ***/

#define KEYBOARD_BUFFER_SIZE				16
#define KEYBOARD_IDLE_STATE_DURATION_MS		100 // Minimum delay between each key press.

/*** KEYBOARD local structures ***/

/*******************************************************************/
typedef enum {
	KEYBOARD_STATE_READY,
	KEYBOARD_STATE_KEY_PRESSED,
	KEYBOARD_STATE_IDLE
} KEYBOARD_state_t;

/*******************************************************************/
typedef struct {
	KEYBOARD_shortcut_t shortcut_buf[KEYBOARD_BUFFER_SIZE];
	uint32_t press_duration_buf[KEYBOARD_BUFFER_SIZE];
	uint8_t write_idx;
	uint8_t read_idx;
	KEYBOARD_state_t state;
	uint64_t state_switch_time;
} KEYBOARD_context_t;

/*** KEYBOARD local global variables ***/

static KEYBOARD_context_t keyboard_ctx;

/*** KEYBOARD functions ***/

/*******************************************************************/
KEYBOARD_status_t KEYBOARD_init(void) {
	// Local variables.
	KEYBOARD_status_t status = KEYBOARD_SUCCESS;
	// Init context.
	keyboard_ctx.read_idx = 0;
	keyboard_ctx.write_idx = 0;
	keyboard_ctx.state = KEYBOARD_STATE_READY;
	keyboard_ctx.state_switch_time = 0;
#ifdef LOG_KEYBOARD
	LOG_STATUS(status, KEYBOARD_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
KEYBOARD_status_t KEYBOARD_press(const KEYBOARD_shortcut_t* shortcut) {
	// Local variables.
	KEYBOARD_status_t status = KEYBOARD_SUCCESS;
	// Check parameter.
	if (shortcut == NULL) {
		status = KEYBOARD_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Press first key.
	if ((shortcut -> vk_code_0) != VK_NONE) {
		keybd_event((shortcut -> vk_code_0), MapVirtualKey((shortcut -> vk_code_0), MAPVK_VK_TO_VSC), 0, 0);
	}
	// Press second key.
	if ((shortcut -> vk_code_1) != VK_NONE) {
		keybd_event((shortcut -> vk_code_1), MapVirtualKey((shortcut -> vk_code_1), MAPVK_VK_TO_VSC), 0, 0);
	}
	LOG("[0x%02X 0x%02X]", (shortcut -> vk_code_0), (shortcut -> vk_code_1));
errors:
#ifdef LOG_KEYBOARD
	LOG_STATUS(status, KEYBOARD_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
KEYBOARD_status_t KEYBOARD_release(const KEYBOARD_shortcut_t* shortcut) {
	// Local variables.
	KEYBOARD_status_t status = KEYBOARD_SUCCESS;
	// Check parameter.
	if (shortcut == NULL) {
		status = KEYBOARD_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Release first key.
	if ((shortcut -> vk_code_0) != VK_NONE) {
		keybd_event((shortcut -> vk_code_0), MapVirtualKey((shortcut -> vk_code_0), MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	}
	// Release second key.
	if ((shortcut -> vk_code_1) != VK_NONE) {
		keybd_event((shortcut -> vk_code_1), MapVirtualKey((shortcut -> vk_code_1), MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	}
	LOG("[0x%02X 0x%02X]", (shortcut -> vk_code_0), (shortcut -> vk_code_1));
errors:
#ifdef LOG_KEYBOARD
	LOG_STATUS(status, KEYBOARD_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
KEYBOARD_status_t KEYBOARD_single_press(const KEYBOARD_shortcut_t* shortcut, uint32_t press_duration_ms) {
	// Local variables.
	KEYBOARD_status_t status = KEYBOARD_SUCCESS;
	// Check parameters.
	if (shortcut == NULL) {
		status = KEYBOARD_ERROR_NULL_PARAMETER;
		goto errors;
	}
	if (press_duration_ms == 0) {
		status = KEYBOARD_ERROR_PRESS_DURATION;
		goto errors;
	}
	// Fill buffers.
	keyboard_ctx.shortcut_buf[keyboard_ctx.write_idx].vk_code_0 = (shortcut -> vk_code_0);
	keyboard_ctx.shortcut_buf[keyboard_ctx.write_idx].vk_code_1 = (shortcut -> vk_code_1);
	keyboard_ctx.press_duration_buf[keyboard_ctx.write_idx] = press_duration_ms;
	// Increment index and manage rollover.
	keyboard_ctx.write_idx++;
	if (keyboard_ctx.write_idx >= KEYBOARD_BUFFER_SIZE) {
		keyboard_ctx.write_idx = 0;
	}
	LOG("[0x%02X 0x%02X]", (shortcut -> vk_code_0), (shortcut -> vk_code_1));
errors:
#ifdef LOG_KEYBOARD
	LOG_STATUS(status, KEYBOARD_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
KEYBOARD_status_t KEYBOARD_process(void) {
	// Local variables.
	KEYBOARD_status_t status = KEYBOARD_SUCCESS;
	// Perform state machine.
	switch (keyboard_ctx.state) {
	case KEYBOARD_STATE_READY:
		// Check indexes.
		if (keyboard_ctx.read_idx != keyboard_ctx.write_idx) {
			// Press key.
			status = KEYBOARD_press(&keyboard_ctx.shortcut_buf[keyboard_ctx.read_idx]);
			if (status != KEYBOARD_SUCCESS) goto errors;
			// Save start time.
			keyboard_ctx.state_switch_time = TIME_get_milliseconds();
			// Change state.
			keyboard_ctx.state = KEYBOARD_STATE_KEY_PRESSED;
		}
		break;
	case KEYBOARD_STATE_KEY_PRESSED:
		// Check duration.
		if (TIME_get_milliseconds() > (keyboard_ctx.state_switch_time + keyboard_ctx.press_duration_buf[keyboard_ctx.read_idx])) {
			// Release key.
			status = KEYBOARD_release(&keyboard_ctx.shortcut_buf[keyboard_ctx.read_idx]);
			if (status != KEYBOARD_SUCCESS) goto errors;
			// Increment index and manage rollover.
			keyboard_ctx.read_idx++;
			if (keyboard_ctx.read_idx >= KEYBOARD_BUFFER_SIZE) {
				keyboard_ctx.read_idx = 0;
			}
			// Go to idle.
			keyboard_ctx.state_switch_time = TIME_get_milliseconds();
			keyboard_ctx.state = KEYBOARD_STATE_IDLE;
		}
		break;
	case KEYBOARD_STATE_IDLE:
		// Check duration.
		if (TIME_get_milliseconds() > (keyboard_ctx.state_switch_time + KEYBOARD_IDLE_STATE_DURATION_MS)) {
			// Go back to ready state.
			keyboard_ctx.state = KEYBOARD_STATE_READY;
		}
		break;
	default:
		// Unknown state.
		keyboard_ctx.state = KEYBOARD_STATE_READY;
		status = KEYBOARD_ERROR_STATE;
		goto errors;
	}
errors:
#ifdef LOG_KEYBOARD
	LOG_STATUS(status, KEYBOARD_SUCCESS, "OK");
#endif
	return status;
}
