/*
 * kvb.h
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#ifndef __KVB_H__
#define __KVB_H__

/*!******************************************************************
 * \enum KVB_status_t
 * \brief KVB driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    KVB_SUCCESS = 0,
    KVB_ERROR_STATE,
    // Low level drivers errors.
    KVB_ERROR_DRIVER_SOUND,
    // Last index.
    KVB_ERROR_LAST
} KVB_status_t;

/*!******************************************************************
 * \enum KVB_state_t
 * \brief KVB states.
 *******************************************************************/
typedef enum {
    KVB_STATE_ON = 0,
    KVB_STATE_OFF,
    KVB_STATE_LAST
} KVB_state_t;

/*** KVB functions ***/

/*!******************************************************************
 * \fn KVB_status_t KVB_init(void)
 * \brief Init KVB driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
KVB_status_t KVB_init(void);

/*!******************************************************************
 * \fn KVB_status_t KVB_set_state(KVB_state_t state)
 * \brief Set KVB state.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
KVB_status_t KVB_set_state(KVB_state_t state);

/*******************************************************************/
#define KVB_exit_error(error_code) { if (kvb_status != KVB_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define KVB_stack_error(void) { if (kvb_status != KVB_SUCCESS) { ERROR_stack_add((ERROR_BASE_KVB * ERROR_BASE_STEP) + kvb_status); } }

/*******************************************************************/
#define KVB_stack_exit_error(error_code) { KVB_stack_error(); KVB_exit_error(error_code); }

#endif /* __KVB_H__ */
