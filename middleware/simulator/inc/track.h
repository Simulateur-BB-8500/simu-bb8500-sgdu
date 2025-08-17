/*
 * track.h
 *
 *  Created on: 22 sep. 2023
 *      Author: Ludo
 */

#ifndef __TRACK_H__
#define __TRACK_H__

#include "stdint.h"

/*** TRACK structures ***/

/*!******************************************************************
 * \enum TRACK_status_t
 * \brief TRACK driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    TRACK_SUCCESS = 0,
    TRACK_ERROR_INTERNAL_STATE,
    // Low level drivers errors.
    TRACK_ERROR_DRIVER_SOUND,
    // Last index.
    TRACK_ERROR_LAST
} TRACK_status_t;

/*!******************************************************************
 * \fn TRACK_status_t TRACK_init(void)
 * \brief Init TRACK driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRACK_status_t TRACK_init(void);

/*!******************************************************************
 * \fn TRACK_status_t TRACK_set_state(TRACK_state_t state)
 * \brief Set current speed.
 * \param[in]   speed_kmh: Current speed in km/h.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRACK_status_t TRACK_set_speed(uint8_t speed_kmh);

/*!******************************************************************
 * \fn TRACK_status_t TRACK_process(void)
 * \brief Process TRACK driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRACK_status_t TRACK_process(void);

/*******************************************************************/
#define TRACK_exit_error(error_code) { if (track_status != TRACK_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define TRACK_stack_error(void) { if (track_status != TRACK_SUCCESS) { ERROR_stack_add((ERROR_BASE_TRACK * ERROR_BASE_STEP) + track_status); } }

/*******************************************************************/
#define TRACK_stack_exit_error(error_code) { TRACK_stack_error(); TRACK_exit_error(error_code); }

#endif /* __TRACK_H__ */
