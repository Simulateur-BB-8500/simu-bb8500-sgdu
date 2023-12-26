/*
 * error.h
 *
 *  Created on: 12 mar. 2022
 *      Author: Ludo
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#include "stdint.h"

/*** ERROR macros ***/

#define ERROR_BASE_STEP		0x00010000

/*** ERROR structures ***/

/*!******************************************************************
 * \enum ERROR_source_t
 * \brief Board error sources.
 *******************************************************************/
typedef enum {
	ERROR_BASE_NONE = 0,
	// External libraries.
	ERROR_BASE_CURL,
	ERROR_BASE_FMOD,
	// Peripherals.
	ERROR_BASE_KEYBOARD,
	ERROR_BASE_SERIAL,
	ERROR_BASE_SOUND,
	ERROR_BASE_TIME,
	ERROR_BASE_WINDOWS,
	// Simulator.
	ERROR_BASE_BPGD,
	ERROR_BASE_COMPRESSOR,
	ERROR_BASE_EMERGENCY,
	ERROR_BASE_FD,
	ERROR_BASE_FPB,
	ERROR_BASE_KVB,
	ERROR_BASE_LIGHT,
	ERROR_BASE_LSMCU,
	ERROR_BASE_MP,
	ERROR_BASE_MPINV,
	ERROR_BASE_PBL2,
	ERROR_BASE_TRACK,
	ERROR_BASE_WHISTLE,
	ERROR_BASE_ZDJ,
	ERROR_BASE_ZPT,
	ERROR_BASE_ZVM,
	// Game.
	ERROR_BASE_ORTS,
	// Last base.
	ERROR_BASE_LAST
} ERROR_base_t;

/*!******************************************************************
 * \enum ERROR_code_t
 * \brief Board error code type.
 *******************************************************************/
typedef uint32_t ERROR_code_t;

/*** ERROR functions ***/

/*!******************************************************************
 * \fn void ERROR_stack_init(void)
 * \brief Init error stack.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void ERROR_stack_init(void);

/*!******************************************************************
 * \fn void ERROR_stack_add(ERROR_code_t code)
 * \brief Add error to stack.
 * \param[in]  	code: Error to stack.
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void ERROR_stack_add(ERROR_code_t code);

/*!******************************************************************
 * \fn ERROR_code_t ERROR_stack_read(void)
 * \brief Read error stack.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Last error code stored.
 *******************************************************************/
ERROR_code_t ERROR_stack_read(void);

/*!******************************************************************
 * \fn uint8_t ERROR_stack_is_empty(void)
 * \brief Check if error stack is empty.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		1 if the error stack is empty, 0 otherwise.
 *******************************************************************/
uint8_t ERROR_stack_is_empty(void);

#endif /* __ERROR_H__ */
