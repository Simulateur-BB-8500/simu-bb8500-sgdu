/*
 * emergency.h
 *
 *  Created on: 26 dec. 2023
 *      Author: Ludo
 */

#ifndef __EMERGENCY_H__
#define __EMERGENCY_H__

/*** EMERGENCY structures ***/

/*!******************************************************************
 * \enum EMERGENCY_status_t
 * \brief EMERGENCY driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    EMERGENCY_SUCCESS = 0,
    EMERGENCY_ERROR_STATE,
    // Low level drivers errors.
    EMERGENCY_ERROR_DRIVER_KEYBOARD,
    EMERGENCY_ERROR_DRIVER_SOUND,
    // Last index.
    EMERGENCY_ERROR_LAST
} EMERGENCY_status_t;

/*!******************************************************************
 * \enum EMERGENCY_state_t
 * \brief Train brake states.
 *******************************************************************/
typedef enum {
    EMERGENCY_STATE_ON = 0,
    EMERGENCY_STATE_OFF,
    EMERGENCY_STATE_LAST
} EMERGENCY_state_t;

/*** EMERGENCY functions ***/

/*!******************************************************************
 * \fn EMERGENCY_status_t EMERGENCY_init(void)
 * \brief Init emergency brake driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
EMERGENCY_status_t EMERGENCY_init(void);

/*!******************************************************************
 * \fn EMERGENCY_status_t EMERGENCY_set_state(EMERGENCY_state_t state)
 * \brief Set emergency brake state.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
EMERGENCY_status_t EMERGENCY_set_state(EMERGENCY_state_t state);

/*******************************************************************/
#define EMERGENCY_exit_error(error_code) { if (emergency_status != EMERGENCY_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define EMERGENCY_stack_error(void) { if (emergency_status != EMERGENCY_SUCCESS) { ERROR_stack_add((ERROR_BASE_EMERGENCY * ERROR_BASE_STEP) + emergency_status); } }

/*******************************************************************/
#define EMERGENCY_stack_exit_error(error_code) { EMERGENCY_stack_error(); EMERGENCY_exit_error(error_code); }

#endif /* __EMERGENCY_H__ */
