/*
 * whistle.h
 *
 *  Created on: 10 may 2020
 *      Author: Ludo
 */

#ifndef __WHISTLE_H__
#define __WHISTLE_H__

/*** WHISTLE structures ***/

/*!******************************************************************
 * \enum WHISTLE_status_t
 * \brief WHISTLE driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    WHISTLE_SUCCESS = 0,
    WHISTLE_ERROR_STATE,
    // Low level drivers errors.
    WHISTLE_ERROR_DRIVER_SOUND,
    // Last index.
    WHISTLE_ERROR_LAST
} WHISTLE_status_t;

/*!******************************************************************
 * \enum WHISTLE_state_t
 * \brief Whistle states.
 *******************************************************************/
typedef enum {
    WHISTLE_STATE_HIGH_TONE,
    WHISTLE_STATE_NEUTRAL,
    WHISTLE_STATE_LOW_TONE,
    WHISTLE_STATE_LAST
} WHISTLE_state_t;

/*** WHISTLE functions ***/

/*!******************************************************************
 * \fn WHISTLE_status_t WHISTLE_init(void)
 * \brief Init whistle driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
WHISTLE_status_t WHISTLE_init(void);

/*!******************************************************************
 * \fn WHISTLE_status_t WHISTLE_set_state(WHISTLE_state_t state)
 * \brief Set whistle state.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
WHISTLE_status_t WHISTLE_set_state(WHISTLE_state_t state);

/*!******************************************************************
 * \fn WHISTLE_status_t WHISTLE_process(void)
 * \brief Main process of WHISTLE driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
WHISTLE_status_t WHISTLE_process(void);

/*******************************************************************/
#define WHISTLE_exit_error(error_code) { if (whistle_status != WHISTLE_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define WHISTLE_stack_error(void) { if (whistle_status != WHISTLE_SUCCESS) { ERROR_stack_add((ERROR_BASE_WHISTLE * ERROR_BASE_STEP) + whistle_status); } }

/*******************************************************************/
#define WHISTLE_stack_exit_error(error_code) { WHISTLE_stack_error(); WHISTLE_exit_error(error_code); }

#endif /* __WHISTLE_H__ */
