/*
 * bpgd.h
 *
 *  Created on: 25 dec. 2023
 *      Author: Ludo
 */

#ifndef __BPGD_H__
#define __BPGD_H__

/*** BPGD structures ***/

/*!******************************************************************
 * \enum BPGD_status_t
 * \brief BPGD driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	BPGD_SUCCESS = 0,
	BPGD_ERROR_STATE,
	// Low level drivers errors.
	BPGD_ERROR_DRIVER_SOUND,
	BPGD_ERROR_DRIVER_KEYBOARD,
	// Last index.
	BPGD_ERROR_LAST
} BPGD_status_t;

/*!******************************************************************
 * \enum BPGD_state_t
 * \brief Train brake states.
 *******************************************************************/
typedef enum {
	BPGD_STATE_ON = 0,
	BPGD_STATE_OFF,
	BPGD_STATE_LAST
} BPGD_state_t;

/*** BPGD functions ***/

/*!******************************************************************
 * \fn BPGD_status_t BPGD_init(void)
 * \brief Init BPGD driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
BPGD_status_t BPGD_init(void);

/*!******************************************************************
 * \fn BPGD_status_t BPGD_set_state(BPGD_state_t state)
 * \brief Set train brake state.
 * \param[in]  	state: New state to set.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
BPGD_status_t BPGD_set_state(BPGD_state_t state);

/*!******************************************************************
 * \fn BPGD_status_t BPGD_process(void)
 * \brief Main process of BPGD driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
BPGD_status_t BPGD_process(void);

/*******************************************************************/
#define BPGD_exit_error(error_code) { if (bpgd_status != BPGD_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define BPGD_stack_error(void) { if (bpgd_status != BPGD_SUCCESS) { ERROR_stack_add((ERROR_BASE_BPGD * ERROR_BASE_STEP) + bpgd_status); } }

/*******************************************************************/
#define BPGD_stack_exit_error(error_code) { BPGD_stack_error(); BPGD_exit_error(error_code); }

#endif /* __BPGD_H__ */
