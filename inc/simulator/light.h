/*
 * lights.h
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#ifndef __LIGHT_H__
#define __LIGHT_H__

/*** LIGHT structures ***/

/*!******************************************************************
 * \enum LIGHT_status_t
 * \brief LIGHT driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	LIGHT_SUCCESS = 0,
	LIGHT_ERROR_TYPE,
	LIGHT_ERROR_STATE,
	// Low level drivers errors.
	LIGHT_ERROR_DRIVER_KEYBOARD,
	// Last index.
	LIGHT_ERROR_LAST
} LIGHT_status_t;

/*!******************************************************************
 * \enum LIGHT_type_t
 * \brief LIGHT types list.
 *******************************************************************/
typedef enum {
	LIGHT_TYPE_ZFG = 0,
	LIGHT_TYPE_ZFD,
	LIGHT_TYPE_ZPR,
	LIGHT_TYPE_ZLFRG,
	LIGHT_TYPE_ZLFRD,
	LIGHT_TYPE_LAST
} LIGHT_type_t;

/*!******************************************************************
 * \enum LIGHT_state_t
 * \brief Motor fan states.
 *******************************************************************/
typedef enum {
	LIGHT_STATE_OFF = 0,
	LIGHT_STATE_ON,
	LIGHT_STATE_LAST
} LIGHT_state_t;

/*** LIGHT functions ***/

/*!******************************************************************
 * \fn LIGHT_status_t LIGHT_init(void)
 * \brief Init LIGHT driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
LIGHT_status_t LIGHT_init(void);

/*!******************************************************************
 * \fn LIGHT_status_t LIGHT_set_state(LIGHT_type_t type, LIGHT_state_t state)
 * \brief Set light state.
 * \param[in]	light: Light to control.
 * \param[in]  	state: New state to set.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
LIGHT_status_t LIGHT_set_state(LIGHT_type_t type, LIGHT_state_t state);

/*******************************************************************/
#define LIGHT_exit_error(error_code) { if (light_status == 0) { status = error_code; goto errors; } }

/*******************************************************************/
#define LIGHT_stack_error(void) { if (light_status == 0) { ERROR_stack_add((ERROR_BASE_LIGHT * ERROR_BASE_STEP) + light_status); } }

/*******************************************************************/
#define LIGHT_stack_exit_error(error_code) { LIGHT_stack_error(); LIGHT_exit_error(error_code); }

#endif /* __LIGHT_H__ */
