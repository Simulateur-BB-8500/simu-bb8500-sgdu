/*
 * openrails.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __OPENRAILS_H__
#define __OPENRAILS_H__

#include "keyboard.h"
#include "stdint.h"
#include "winuser.h"

/*** OPENRAILS macros ***/

// Press duration to make the shortcuts work (in milliseconds).
#define OPENRAILS_PRESS_DURATION_MS_DEFAULT 	100
#define OPENRAILS_PRESS_DURATION_MS_MP 			300

/*** OPENRAILS structures ***/

typedef enum {
	OPENRAILS_SUCCESS = 0,
	OPENRAILS_ERROR_CURL_INIT,
	OPENRAILS_ERROR_CURL_REQUEST,
	OPENRAILS_ERROR_BASE_LAST = 0x0100
} OPENRAILS_status_t;

typedef enum {
	OPENRAILS_API_SAMPLE_SPEED = 0,
	OPENRAILS_API_SAMPLE_LAST
} OPENRAILS_api_sample_t;

/*** OPENRAILS shortcuts ***/

// ZPT.
static const KEYBOARD_shortcut_t OPENRAILS_ZPT_FRONT_TOGGLE = 	(KEYBOARD_shortcut_t) {'P', VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_ZPT_BACK_TOGGLE = 	(KEYBOARD_shortcut_t) {VK_SHIFT, 'P'};
// MPINV.
static const KEYBOARD_shortcut_t OPENRAILS_MPINV_FORWARD = 		(KEYBOARD_shortcut_t) {'Z', VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_MPINV_BACKWARD = 	(KEYBOARD_shortcut_t) {'S', VK_NONE};
// MP.
static const KEYBOARD_shortcut_t OPENRAILS_MP_T_MORE =			(KEYBOARD_shortcut_t) {'D', VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_MP_T_LESS =			(KEYBOARD_shortcut_t) {'Q', VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_MP_0 =				(KEYBOARD_shortcut_t) {VK_CONTROL, 'Q'};
// FPB.
static const KEYBOARD_shortcut_t OPENRAILS_FPB_APPLY =			(KEYBOARD_shortcut_t) {VK_OEM_3, VK_NONE}; // VK_OEM_3 = 'ù' key.
static const KEYBOARD_shortcut_t OPENRAILS_FPB_RELEASE  =		(KEYBOARD_shortcut_t) {'M', VK_NONE};
// FD.
static const KEYBOARD_shortcut_t OPENRAILS_FD_APPLY =			(KEYBOARD_shortcut_t) {VK_OEM_1, VK_NONE}; // VK_OEM_1 = '$' key.
static const KEYBOARD_shortcut_t OPENRAILS_FD_RELEASE =			(KEYBOARD_shortcut_t) {VK_OEM_6, VK_NONE}; // VK_OEM_6 = '^' key.
// BPURG.
static const KEYBOARD_shortcut_t OPENRAILS_BPURG =				(KEYBOARD_shortcut_t) {VK_BACK, VK_NONE};
// AUX.
static const KEYBOARD_shortcut_t OPENRAILS_BPEV =				(KEYBOARD_shortcut_t) {'V', VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_SABLAGE =			(KEYBOARD_shortcut_t) {VK_SHIFT, 'X'};
// Lights.
static const KEYBOARD_shortcut_t OPENRAILS_LIGHTS_ON =			(KEYBOARD_shortcut_t) {'H', VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_LIGHTS_OFF =			(KEYBOARD_shortcut_t) {VK_SHIFT, 'H'};
// Game.
static const KEYBOARD_shortcut_t OPENRAILS_MASK_CAB = 			(KEYBOARD_shortcut_t) {VK_SHIFT, '1'};
static const KEYBOARD_shortcut_t OPENRAILS_LOG =				(KEYBOARD_shortcut_t) {VK_F12, VK_NONE};
static const KEYBOARD_shortcut_t OPENRAILS_NOP =				(KEYBOARD_shortcut_t) {VK_NONE, VK_NONE};

/*** OPENRAILS functions ***/

OPENRAILS_status_t OPENRAILS_init_server(void);
OPENRAILS_status_t OPENRAILS_get_api_sample(OPENRAILS_api_sample_t api_sample, uint32_t* value);

#endif /* __OPENRAILS_H__ */
