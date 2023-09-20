/*
 * mpinv.h
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#ifndef __MPINV_H__
#define __MPINV_H__

/*** MPINV structures ***/

/*!******************************************************************
 * \enum MPINV_status_t
 * \brief MPINV driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	MPINV_SUCCESS = 0,
	MPINV_ERROR_POSITION,
	// Low level drivers errors.
	MPINV_ERROR_DRIVER_KEYBOARD,
	MPINV_ERROR_DRIVER_SOUND,
	// Last index.
	MPINV_ERROR_LAST
} MPINV_status_t;

/*!******************************************************************
 * \enum MPINV_position_t
 * \brief MPINV gear position.
 *******************************************************************/
typedef enum {
	MPINV_POSITION_BACKWARD = 0,
	MPINV_POSITION_NEUTRAL,
	MPINV_POSITION_FORWARD,
	MPINV_POSITION_LAST
} MPINV_position_t;

/*** MPINV functions ***/

/*!******************************************************************
 * \fn MPINV_status_t MPINV_init(void)
 * \brief Init MPINV driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
MPINV_status_t MPINV_init(void);

/*!******************************************************************
 * \fn MPINV_status_t MPINV_set_position(MPINV_position_t position)
 * \brief Set MPINV position.
 * \param[in]  	position: New position to set.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
MPINV_status_t MPINV_set_position(MPINV_position_t position);

/*******************************************************************/
#define MPINV_exit_error(error_code) { if (mpinv_status == 0) { status = error_code; goto errors; } }

/*******************************************************************/
#define MPINV_stack_error(void) { if (mpinv_status == 0) { ERROR_stack_add((ERROR_BASE_MPINV * ERROR_BASE_STEP) + mpinv_status); } }

/*******************************************************************/
#define MPINV_stack_exit_error(error_code) { MPINV_stack_error(); MPINV_exit_error(error_code); }

#endif /* __MPINV_H__ */
