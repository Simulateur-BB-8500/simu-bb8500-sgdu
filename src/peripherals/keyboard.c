/*
 * keyboard.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "keyboard.h"

#include "stdio.h"
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
} KEYBOARD_State;

typedef struct {
	KEYBOARD_Key keyboard_key_buf[KEYBOARD_BUFFER_SIZE];
	unsigned int keyboard_press_duration_buf[KEYBOARD_BUFFER_SIZE];
	unsigned char keyboard_buf_write_idx;
	unsigned char keyboard_buf_read_idx;
	KEYBOARD_State keyboard_state;
	unsigned long keyboard_state_switch_time;
} KEYBOARD_Context;

/*** KEYBOARD local global variables ***/

static KEYBOARD_Context keyboard_ctx;

/*** KEYBOARD local functions ***/

/* PRESS A KEYBOARD KEY.
 * @param key:	Key to press.
 * @return:		None.
 */
static void KEYBOARD_Press(const KEYBOARD_Key* key) {
#ifdef WINDOWS
	keybd_event((key -> keyboard_key_code), (key -> keyboard_key_scan), 0, 0);
#endif
#ifdef KEYBOARD_LOG
	printf("KEYBOARD *** Press key 0x%x\n", (key -> keyboard_key_code));
	fflush(stdout);
#endif
}

/* RELEASE A KEYBOARD KEY.
 * @param key:	Key to release.
 * @return:		None.
 */
static void KEYBOARD_Release(const KEYBOARD_Key* key) {
#ifdef WINDOWS
	keybd_event((key -> keyboard_key_code), (key -> keyboard_key_scan), KEYEVENTF_KEYUP, 0);
#endif
#ifdef KEYBOARD_LOG
	printf("KEYBOARD *** Release key 0x%x\n", (key -> keyboard_key_code));
	fflush(stdout);
#endif
}

/*** KEYBOARD functions ***/

/*** KEYBOARD MODULE INITIALIZATION.
 * @param:	None.
 * @return:	None.
 */
void KEYBOARD_Init(void) {
	// Init context.
	keyboard_ctx.keyboard_buf_read_idx = 0;
	keyboard_ctx.keyboard_buf_write_idx = 0;
	keyboard_ctx.keyboard_state = KEYBOARD_STATE_READY;
	keyboard_ctx.keyboard_state_switch_time = 0;
}

/* APPEND A NEW KEY TO THE KEYBOARD BUFFER.
 * @param key:					Key to press.
 * @param press_duration_ms:	Key press duration in ms.
 * @return:						None.
 */
void KEYBOARD_Send(const KEYBOARD_Key* key, unsigned int press_duration_ms) {
	// Fill buffers.
	keyboard_ctx.keyboard_key_buf[keyboard_ctx.keyboard_buf_write_idx].keyboard_key_code = (key -> keyboard_key_code);
	keyboard_ctx.keyboard_key_buf[keyboard_ctx.keyboard_buf_write_idx].keyboard_key_scan = (key -> keyboard_key_scan);
	keyboard_ctx.keyboard_press_duration_buf[keyboard_ctx.keyboard_buf_write_idx] = press_duration_ms;
	// Increment index and manage rollover.
	keyboard_ctx.keyboard_buf_write_idx++;
	if (keyboard_ctx.keyboard_buf_write_idx >= KEYBOARD_BUFFER_SIZE) {
		keyboard_ctx.keyboard_buf_write_idx = 0;
	}
}

/* MAIN TASK OF KEYBOARD MODULE.
 * @param:	None.
 * @return:	None.
 */
void KEYBOARD_Task(void) {
	// Perform state machine.
	switch (keyboard_ctx.keyboard_state) {
	case KEYBOARD_STATE_READY:
		// Check indexes.
		if (keyboard_ctx.keyboard_buf_read_idx != keyboard_ctx.keyboard_buf_write_idx) {
			// Press key.
			KEYBOARD_Press(&keyboard_ctx.keyboard_key_buf[keyboard_ctx.keyboard_buf_read_idx]);
			// Save start time.
			keyboard_ctx.keyboard_state_switch_time = TIME_GetMs();
			// Change state.
			keyboard_ctx.keyboard_state = KEYBOARD_STATE_KEY_PRESSED;
		}
		break;
	case KEYBOARD_STATE_KEY_PRESSED:
		// Check duration.
		if (TIME_GetMs() > (keyboard_ctx.keyboard_state_switch_time + keyboard_ctx.keyboard_press_duration_buf[keyboard_ctx.keyboard_buf_read_idx])) {
			// Release key.
			KEYBOARD_Release(&keyboard_ctx.keyboard_key_buf[keyboard_ctx.keyboard_buf_read_idx]);
			// Increment index and manage rollover.
			keyboard_ctx.keyboard_buf_read_idx++;
			if (keyboard_ctx.keyboard_buf_read_idx >= KEYBOARD_BUFFER_SIZE) {
				keyboard_ctx.keyboard_buf_read_idx = 0;
			}
			// Go to idle.
			keyboard_ctx.keyboard_state_switch_time = TIME_GetMs();
			keyboard_ctx.keyboard_state = KEYBOARD_STATE_IDLE;
		}
		break;
	case KEYBOARD_STATE_IDLE:
		// Check duration.
		if (TIME_GetMs() > (keyboard_ctx.keyboard_state_switch_time + KEYBOARD_IDLE_STATE_DURATION_MS)) {
			// Go back to ready state.
			keyboard_ctx.keyboard_state = KEYBOARD_STATE_READY;
		}
		break;
	default:
		// Unknown state.
		keyboard_ctx.keyboard_state = KEYBOARD_STATE_READY;
	}
}
