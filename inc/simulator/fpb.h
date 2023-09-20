/*
 * fpb.h
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#ifndef __FPB_H__
#define __FPB_H__

/*** FPB structures ***/

/*!******************************************************************
 * \enum FPB_status_t
 * \brief FPB driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	FPB_SUCCESS = 0,
	FPB_ERROR_STATE,
	// Low level drivers errors.
	FPB_ERROR_DRIVER_KEYBOARD,
	FPB_ERROR_DRIVER_SOUND,
	// Last index.
	FPB_ERROR_LAST
} FPB_status_t;

/*!******************************************************************
 * \enum FPB_state_t
 * \brief Train brake states.
 *******************************************************************/
typedef enum {
	FPB_STATE_APPLY = 0,
	FPB_STATE_NEUTRAL,
	FPB_STATE_RELEASE,
	FPB_STATE_LAST
} FPB_state_t;

/*** FPB functions ***/

/*!******************************************************************
 * \fn FPB_status_t FPB_init(void)
 * \brief Init FPB driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
FPB_status_t FPB_init(void);

/*!******************************************************************
 * \fn FPB_status_t FPB_set_state(FPB_state_t state)
 * \brief Set train brake state.
 * \param[in]  	state: New state to set.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
FPB_status_t FPB_set_state(FPB_state_t state);

/*!******************************************************************
 * \fn FPB_status_t FPB_process(void)
 * \brief Main process of FPB driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
FPB_status_t FPB_process(void);

/*******************************************************************/
#define FPB_exit_error(error_code) { if (fpb_status != FPB_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define FPB_stack_error(void) { if (fpb_status != FPB_SUCCESS) { ERROR_stack_add((ERROR_BASE_FPB * ERROR_BASE_STEP) + fpb_status); } }

/*******************************************************************/
#define FPB_stack_exit_error(error_code) { FPB_stack_error(); FPB_exit_error(error_code); }

#endif /* __FPB_H__ */
