/*
 * keyboard.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "stdint.h"

/*** KEYBOARD macros ***/

#define VK_NONE	0x00

/*** KEYBOARD structures ***/

typedef struct {
	uint8_t vk_code_0;
	uint8_t vk_code_1;
} KEYBOARD_shortcut_t;

/*** KEYBOARD functions ***/

void KEYBOARD_init(void);
void KEYBOARD_send(const KEYBOARD_shortcut_t* shortcut, uint32_t press_duration_ms);
void KEYBOARD_task(void);

#endif /* __KEYBOARD_H__ */
