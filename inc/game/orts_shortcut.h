/*
 * orts_shortcut.h
 *
 *  Created on: 4 apr. 2023
 *      Author: Ludo
 */

#ifndef __ORTS_SHORTCUT_H__
#define __ORTS_SHORTCUT_H__

#include "keyboard.h"
#include "winuser.h"

/*** ORTS SHORTCUTS macros ***/

// Press duration to make shortcuts work.
#define ORTS_SHORTCUT_PRESS_DURATION_MS_DEFAULT 	100

// ZPT.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_ZPT_FRONT_TOGGLE = 	(KEYBOARD_shortcut_t) {'P', VK_NONE};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_ZPT_BACK_TOGGLE = 	(KEYBOARD_shortcut_t) {VK_SHIFT, 'P'};
// MPINV.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_MPINV_FORWARD = 		(KEYBOARD_shortcut_t) {'Z', VK_NONE};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_MPINV_BACKWARD = 	(KEYBOARD_shortcut_t) {'S', VK_NONE};
// MP.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_MP_MORE =			(KEYBOARD_shortcut_t) {'D', VK_NONE};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_MP_LESS =			(KEYBOARD_shortcut_t) {'Q', VK_NONE};
// FPB.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_FPB_APPLY =			(KEYBOARD_shortcut_t) {VK_OEM_3, VK_NONE}; // VK_OEM_3 = 'ù' key.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_FPB_RELEASE  =		(KEYBOARD_shortcut_t) {'M', VK_NONE};
// FD.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_FD_APPLY =			(KEYBOARD_shortcut_t) {VK_OEM_1, VK_NONE}; // VK_OEM_1 = '$' key.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_FD_RELEASE =			(KEYBOARD_shortcut_t) {VK_OEM_6, VK_NONE}; // VK_OEM_6 = '^' key.
// BPURG.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_BPURG =				(KEYBOARD_shortcut_t) {VK_BACK, VK_NONE};
// AUX.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_BPEV =				(KEYBOARD_shortcut_t) {'V', VK_NONE};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_SABLAGE =			(KEYBOARD_shortcut_t) {VK_SHIFT, 'X'};
// Lights.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_LIGHTS_ON =			(KEYBOARD_shortcut_t) {'H', VK_NONE};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_LIGHTS_OFF =			(KEYBOARD_shortcut_t) {VK_SHIFT, 'H'};
// Game.
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_MASK_CAB = 			(KEYBOARD_shortcut_t) {VK_SHIFT, '1'};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_LOG =				(KEYBOARD_shortcut_t) {VK_F12, VK_NONE};
static const KEYBOARD_shortcut_t ORTS_SHORTCUT_NOP =				(KEYBOARD_shortcut_t) {VK_NONE, VK_NONE};

#endif /* __ORTS_SHORTCUT_H__ */
