/*
 * zvm.h
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#ifndef __ZVM_H__
#define __ZVM_H__

/*** ZVM structures ***/

/*!******************************************************************
 * \enum ZVM_status_t
 * \brief ZVM driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	ZVM_SUCCESS = 0,
	ZVM_ERROR_STATE,
	ZVM_ERROR_INTERNAL_STATE,
	// Low level drivers errors.
	ZVM_ERROR_DRIVER_SOUND,
	// Last index.
	ZVM_ERROR_LAST
} ZVM_status_t;

/*!******************************************************************
 * \enum ZVM_state_t
 * \brief Motor fan states.
 *******************************************************************/
typedef enum {
	ZVM_STATE_OFF = 0,
	ZVM_STATE_ON,
	ZVM_STATE_LAST
} ZVM_state_t;

/*** ZVM functions ***/

/*!******************************************************************
 * \fn ZVM_status_t ZVM_init(void)
 * \brief Init ZVM driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ZVM_status_t ZVM_init(void);

/*!******************************************************************
 * \fn ZVM_status_t ZVM_set_state(ZVM_state_t state)
 * \brief Set motor fans state.
 * \param[in]  	state: New state to set.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ZVM_status_t ZVM_set_state(ZVM_state_t state);

/*!******************************************************************
 * \fn ZVM_status_t ZVM_process(void)
 * \brief Process ZVM driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ZVM_status_t ZVM_process(void);

/*******************************************************************/
#define ZVM_exit_error(error_code) { if (zvm_status != ZVM_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define ZVM_stack_error(void) { if (zvm_status != ZVM_SUCCESS) { ERROR_stack_add((ERROR_BASE_ZVM * ERROR_BASE_STEP) + zvm_status); } }

/*******************************************************************/
#define ZVM_stack_exit_error(error_code) { ZVM_stack_error(); ZVM_exit_error(error_code); }

#endif /* __ZVM_H__ */
