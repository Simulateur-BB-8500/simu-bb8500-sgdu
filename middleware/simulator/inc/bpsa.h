/*
 * bpsa.h
 *
 *  Created on: 02 jan. 2024
 *      Author: Ludo
 */

#ifndef __BPSA_H__
#define __BPSA_H__

#include "stdint.h"

/*** BPSA structures ***/

/*!******************************************************************
 * \enum BPSA_status_t
 * \brief BPSA driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    BPSA_SUCCESS = 0,
    BPSA_ERROR_STATE,
    // Low level drivers errors.
    BPSA_ERROR_DRIVER_KEYBOARD,
    // Last index.
    BPSA_ERROR_LAST
} BPSA_status_t;

/*!******************************************************************
 * \enum BPSA_event_t
 * \brief BPSA events list.
 *******************************************************************/
typedef enum {
    BPSA_STATE_OFF = 0,
    BPSA_STATE_ON,
    BPSA_STATE_LAST
} BPSA_state_t;

/*** BPSA functions ***/

/*!******************************************************************
 * \fn BPSA_status_t BPSA_init(void)
 * \brief Init BPSA driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
BPSA_status_t BPSA_init(void);

/*!******************************************************************
 * \fn BPSA_status_t BPSA_set_state(BPSA_state_t state)
 * \brief Set throttle event.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
BPSA_status_t BPSA_set_state(BPSA_state_t state);

/*******************************************************************/
#define BPSA_exit_error(error_code) { if (bpsa_status != BPSA_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define BPSA_stack_error(void) { if (bpsa_status != BPSA_SUCCESS) { ERROR_stack_add((ERROR_BASE_BPSA * ERROR_BASE_STEP) + bpsa_status); } }

/*******************************************************************/
#define BPSA_stack_exit_error(error_code) { BPSA_stack_error(); BPSA_exit_error(error_code); }

#endif /* __BPSA_H__ */
