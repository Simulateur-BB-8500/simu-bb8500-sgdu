/*
 * keyboard.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/*** KEYBOARD macros ***/

#define KEYBOARD_ARRAY_SIZE		2

/*** KEYBOARD structures ***/

typedef struct {
	unsigned char keyboard_key_code;
	unsigned char keyboard_key_scan;
} KEYBOARD_Key;

/*** KEYBAORD extern variables ***/

static const KEYBOARD_Key KEY_A = (KEYBOARD_Key) {0x41, 0x9E};
static const KEYBOARD_Key KEY_B = (KEYBOARD_Key) {0x42, 0xB0};
static const KEYBOARD_Key KEY_C = (KEYBOARD_Key) {0x43, 0xAE};
static const KEYBOARD_Key KEY_D = (KEYBOARD_Key) {0x44, 0xA0};
static const KEYBOARD_Key KEY_E = (KEYBOARD_Key) {0x45, 0x92};
static const KEYBOARD_Key KEY_F = (KEYBOARD_Key) {0x46, 0xA1};
static const KEYBOARD_Key KEY_G = (KEYBOARD_Key) {0x47, 0xA2};
static const KEYBOARD_Key KEY_H = (KEYBOARD_Key) {0x48, 0xA3};
static const KEYBOARD_Key KEY_I = (KEYBOARD_Key) {0x49, 0x97};
static const KEYBOARD_Key KEY_J = (KEYBOARD_Key) {0x4A, 0xA4};
static const KEYBOARD_Key KEY_K = (KEYBOARD_Key) {0x4B, 0xA5};
static const KEYBOARD_Key KEY_L = (KEYBOARD_Key) {0x4C, 0xA6};
static const KEYBOARD_Key KEY_M = (KEYBOARD_Key) {0x4D, 0xB2};
static const KEYBOARD_Key KEY_N = (KEYBOARD_Key) {0x4E, 0xB1};
static const KEYBOARD_Key KEY_O = (KEYBOARD_Key) {0x4F, 0x98};
static const KEYBOARD_Key KEY_P = (KEYBOARD_Key) {0x50, 0x99};
static const KEYBOARD_Key KEY_Q = (KEYBOARD_Key) {0x51, 0x90};
static const KEYBOARD_Key KEY_R = (KEYBOARD_Key) {0x52, 0x93};
static const KEYBOARD_Key KEY_S = (KEYBOARD_Key) {0x53, 0x9F};
static const KEYBOARD_Key KEY_T = (KEYBOARD_Key) {0x54, 0x94};
static const KEYBOARD_Key KEY_U = (KEYBOARD_Key) {0x55, 0x96};
static const KEYBOARD_Key KEY_V = (KEYBOARD_Key) {0x56, 0xAF};
static const KEYBOARD_Key KEY_W = (KEYBOARD_Key) {0x57, 0x91};
static const KEYBOARD_Key KEY_X = (KEYBOARD_Key) {0x58, 0xAD};
static const KEYBOARD_Key KEY_Y = (KEYBOARD_Key) {0x59, 0x95};
static const KEYBOARD_Key KEY_Z = (KEYBOARD_Key) {0x5A, 0xAC};

/*** KEYBOARD functions ***/

void KEYBOARD_init(void);
void KEYBOARD_send(const KEYBOARD_Key* key, unsigned int press_duration_ms);
void KEYBOARD_task(void);

#endif /* __KEYBOARD_H__ */
