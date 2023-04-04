/*
 * keyboard.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "keyboard.h"

#include "stdio.h"
#include "stdint.h"
#include "time.h"
#ifdef WINDOWS
#include "windows.h"
#endif

/*** KEYBOARD local macros ***/

#define KEYBOARD_BUFFER_SIZE				16
#define KEYBOARD_IDLE_STATE_DURATION_MS		100 // Minimum delay between each key press.
//#define KEYBOARD_LOG

/*** KEYBOARD local structures ***/

typedef enum {
	KEYBOARD_STATE_READY,
	KEYBOARD_STATE_KEY_PRESSED,
	KEYBOARD_STATE_IDLE
} KEYBOARD_state_t;

typedef struct {
	KEYBOARD_shortcut_t shortcut_buf[KEYBOARD_BUFFER_SIZE];
	uint32_t press_duration_buf[KEYBOARD_BUFFER_SIZE];
	uint8_t write_idx;
	uint8_t read_idx;
	KEYBOARD_state_t state;
	unsigned long state_switch_time;
} KEYBOARD_context_t;

/*** KEYBOARD local global variables ***/

static KEYBOARD_context_t keyboard_ctx;

/*** KEYBOARD local functions ***/

/* PRESS A KEYBOARD KEY.
 * @param shortcut:	Shortcut to press.
 * @return:			None.
 */
static void _KEYBOARD_press(const KEYBOARD_shortcut_t* shortcut) {
#ifdef WINDOWS
	if ((shortcut -> vk_code_0) != VK_NONE) {
		keybd_event((shortcut -> vk_code_0), MapVirtualKey((shortcut -> vk_code_0), MAPVK_VK_TO_VSC), 0, 0);
	}
	if ((shortcut -> vk_code_1) != VK_NONE) {
		keybd_event((shortcut -> vk_code_1), MapVirtualKey((shortcut -> vk_code_1), MAPVK_VK_TO_VSC), 0, 0);
	}
#endif
#ifdef KEYBOARD_LOG
	printf("KEYBOARD *** Press key 0x%x\n", (key -> KEYBOARD_key_t_code));
	fflush(stdout);
#endif
}

/* RELEASE A KEYBOARD KEY.
 * @param shortcut:	Shortcut to release.
 * @return:			None.
 */
static void _KEYBOARD_release(const KEYBOARD_shortcut_t* shortcut) {
#ifdef WINDOWS
	if ((shortcut -> vk_code_0) != VK_NONE) {
		keybd_event((shortcut -> vk_code_0), MapVirtualKey((shortcut -> vk_code_0), MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	}
	if ((shortcut -> vk_code_1) != VK_NONE) {
		keybd_event((shortcut -> vk_code_1), MapVirtualKey((shortcut -> vk_code_1), MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	}
#endif
#ifdef KEYBOARD_LOG
	printf("KEYBOARD *** Release key 0x%x\n", (key -> KEYBOARD_key_t_code));
	fflush(stdout);
#endif
}

/*** KEYBOARD functions ***/

/*** KEYBOARD MODULE INITIALIZATION.
 * @param:	None.
 * @return:	None.
 */
void KEYBOARD_init(void) {
	// Init context.
	keyboard_ctx.read_idx = 0;
	keyboard_ctx.write_idx = 0;
	keyboard_ctx.state = KEYBOARD_STATE_READY;
	keyboard_ctx.state_switch_time = 0;
}

/* APPEND A NEW KEY TO THE KEYBOARD BUFFER.
 * @param shortcut:				Shortcut to press.
 * @param press_duration_ms:	Key press duration in ms.
 * @return:						None.
 */
void KEYBOARD_send(const KEYBOARD_shortcut_t* shortcut, uint32_t press_duration_ms) {
	// Fill buffers.
	keyboard_ctx.shortcut_buf[keyboard_ctx.write_idx].vk_code_0 = (shortcut -> vk_code_0);
	keyboard_ctx.shortcut_buf[keyboard_ctx.write_idx].vk_code_1 = (shortcut -> vk_code_1);
	keyboard_ctx.press_duration_buf[keyboard_ctx.write_idx] = press_duration_ms;
	// Increment index and manage rollover.
	keyboard_ctx.write_idx++;
	if (keyboard_ctx.write_idx >= KEYBOARD_BUFFER_SIZE) {
		keyboard_ctx.write_idx = 0;
	}
}

/* MAIN TASK OF KEYBOARD MODULE.
 * @param:	None.
 * @return:	None.
 */
void KEYBOARD_task(void) {
	// Perform state machine.
	switch (keyboard_ctx.state) {
	case KEYBOARD_STATE_READY:
		// Check indexes.
		if (keyboard_ctx.read_idx != keyboard_ctx.write_idx) {
			// Press key.
			_KEYBOARD_press(&keyboard_ctx.shortcut_buf[keyboard_ctx.read_idx]);
			// Save start time.
			keyboard_ctx.state_switch_time = TIME_get_ms();
			// Change state.
			keyboard_ctx.state = KEYBOARD_STATE_KEY_PRESSED;
		}
		break;
	case KEYBOARD_STATE_KEY_PRESSED:
		// Check duration.
		if (TIME_get_ms() > (keyboard_ctx.state_switch_time + keyboard_ctx.press_duration_buf[keyboard_ctx.read_idx])) {
			// Release key.
			_KEYBOARD_release(&keyboard_ctx.shortcut_buf[keyboard_ctx.read_idx]);
			// Increment index and manage rollover.
			keyboard_ctx.read_idx++;
			if (keyboard_ctx.read_idx >= KEYBOARD_BUFFER_SIZE) {
				keyboard_ctx.read_idx = 0;
			}
			// Go to idle.
			keyboard_ctx.state_switch_time = TIME_get_ms();
			keyboard_ctx.state = KEYBOARD_STATE_IDLE;
		}
		break;
	case KEYBOARD_STATE_IDLE:
		// Check duration.
		if (TIME_get_ms() > (keyboard_ctx.state_switch_time + KEYBOARD_IDLE_STATE_DURATION_MS)) {
			// Go back to ready state.
			keyboard_ctx.state = KEYBOARD_STATE_READY;
		}
		break;
	default:
		// Unknown state.
		keyboard_ctx.state = KEYBOARD_STATE_READY;
	}
}
