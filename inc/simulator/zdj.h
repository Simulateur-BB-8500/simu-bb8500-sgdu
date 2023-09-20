/*
 * zdj.h
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#ifndef __ZDJ_H__
#define __ZDJ_H__

/*** ZDJ structures ***/

/*!******************************************************************
 * \enum ZDJ_status_t
 * \brief ZDJ driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	ZDJ_SUCCESS = 0,
	ZDJ_ERROR_STATE,
	// Low level drivers errors.
	ZDJ_ERROR_DRIVER_SOUND,
	// Last index.
	ZDJ_ERROR_LAST
} ZDJ_status_t;

/*!******************************************************************
 * \enum ZDJ_state_t
 * \brief Whistle states.
 *******************************************************************/
typedef enum {
	ZDJ_STATE_OPEN,
	ZDJ_STATE_LOCK,
	ZDJ_STATE_LAST
} ZDJ_state_t;

/*** ZDJ functions ***/

/*!******************************************************************
 * \fn ZDJ_status_t ZDJ_init(void)
 * \brief Init ZDJ driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ZDJ_status_t ZDJ_init(void);

/*!******************************************************************
 * \fn ZDJ_status_t ZDJ_set_state(ZDJ_state_t state)
 * \brief Set ZDJ state.
 * \param[in]  	state: New state to set.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ZDJ_status_t ZDJ_set_state(ZDJ_state_t state);

/*******************************************************************/
#define ZDJ_exit_error(error_code) { if (zdj_status != ZDJ_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define ZDJ_stack_error(void) { if (zdj_status != ZDJ_SUCCESS) { ERROR_stack_add((ERROR_BASE_ZDJ * ERROR_BASE_STEP) + zdj_status); } }

/*******************************************************************/
#define ZDJ_stack_exit_error(error_code) { ZDJ_stack_error(); ZDJ_exit_error(error_code); }

#endif /* __ZDJ_H__ */
