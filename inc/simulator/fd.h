/*
 * fd.h
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#ifndef __FD_H__
#define __FD_H__

/*** FD structures ***/

/*!******************************************************************
 * \enum FD_status_t
 * \brief FD driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    FD_SUCCESS = 0,
    FD_ERROR_STATE,
    // Low level drivers errors.
    FD_ERROR_DRIVER_KEYBOARD,
    FD_ERROR_DRIVER_SOUND,
    // Last index.
    FD_ERROR_LAST
} FD_status_t;

/*!******************************************************************
 * \enum FD_state_t
 * \brief Locomotive brake states.
 *******************************************************************/
typedef enum {
    FD_STATE_APPLY = 0,
    FD_STATE_NEUTRAL,
    FD_STATE_RELEASE,
    FD_STATE_LAST
} FD_state_t;

/*** FD functions ***/

/*!******************************************************************
 * \fn FD_status_t FD_init(void)
 * \brief Init FD driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
FD_status_t FD_init(void);

/*!******************************************************************
 * \fn FD_status_t FD_set_state(FD_state_t state)
 * \brief Set locomotive brake state.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
FD_status_t FD_set_state(FD_state_t state);

/*!******************************************************************
 * \fn FD_status_t FD_process(void)
 * \brief Main process of FD driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
FD_status_t FD_process(void);

/*******************************************************************/
#define FD_exit_error(error_code) { if (fd_status != FD_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define FD_stack_error(void) { if (fd_status != FD_SUCCESS) { ERROR_stack_add((ERROR_BASE_FD * ERROR_BASE_STEP) + fd_status); } }

/*******************************************************************/
#define FD_stack_exit_error(error_code) { FD_stack_error(); FD_exit_error(error_code); }

#endif /* __FD_H__ */
