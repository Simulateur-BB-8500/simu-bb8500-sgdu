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

typedef enum {
	// Driver errors.
	KEYBOARD_SUCCESS = 0,
	KEYBOARD_ERROR_NULL_PARAMETER,
	KEYBOARD_ERROR_PRESS_DURATION,
	KEYBOARD_ERROR_STATE,
	// Last index.
	KEYBOARD_ERROR_LAST
} KEYBOARD_status_t;

/*!******************************************************************
 * \enum KEYBOARD_shortcut_t
 * \brief Keyboard shortcut keys structure.
 *******************************************************************/
typedef struct {
	uint8_t vk_code_0;
	uint8_t vk_code_1;
} KEYBOARD_shortcut_t;

/*** KEYBOARD functions ***/

/*!******************************************************************
 * \fn void KEYBOARD_init(void)
 * \brief Init keyboard peripheral.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
KEYBOARD_status_t KEYBOARD_init(void);

/*!******************************************************************
 * \fn void KEYBOARD_press(const KEYBOARD_shortcut_t* shortcut)
 * \brief Press a keyboard shortcut.
 * \param[in]  	shortcut: Keys to press.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
KEYBOARD_status_t KEYBOARD_press(const KEYBOARD_shortcut_t* shortcut);

/*!******************************************************************
 * \fn void KEYBOARD_release(const KEYBOARD_shortcut_t* shortcut)
 * \brief Release a keyboard shortcut.
 * \param[in]  	shortcut: Keys to release.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
KEYBOARD_status_t KEYBOARD_release(const KEYBOARD_shortcut_t* shortcut);

/*!******************************************************************
 * \fn void KEYBOARD_single_press(const KEYBOARD_shortcut_t* shortcut, uint32_t press_duration_ms)
 * \brief Perform a single keyboard shortcut press.
 * \param[in]  	shortcut: Keys to press.
 * \param[in]	press_duration_ms: Press duration in milliseconds.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
KEYBOARD_status_t KEYBOARD_single_press(const KEYBOARD_shortcut_t* shortcut, uint32_t press_duration_ms);

/*!******************************************************************
 * \fn void KEYBOARD_process(void)
 * \brief Main task of keyboard driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
KEYBOARD_status_t KEYBOARD_process(void);

/*******************************************************************/
#define KEYBOARD_exit_error(error_code) { if (keyboard_status != KEYBOARD_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define KEYBOARD_stack_error(void) { if (keyboard_status != KEYBOARD_SUCCESS) { ERROR_stack_add((ERROR_BASE_KEYBOARD * ERROR_BASE_STEP) + keyboard_status); } }

/*******************************************************************/
#define KEYBOARD_stack_exit_error(error_code) { KEYBOARD_stack_error(); KEYBOARD_exit_error(error_code); }

#endif /* __KEYBOARD_H__ */
