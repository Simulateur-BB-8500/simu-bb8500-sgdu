/*
 * pbl2.h
 *
 *  Created on: 21 sep. 2023
 *      Author: Ludo
 */

#ifndef __PBL2_H__
#define __PBL2_H__

/*** PBL2 structures ***/

/*!******************************************************************
 * \enum PBL2_status_t
 * \brief PBL2 driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    PBL2_SUCCESS = 0,
    PBL2_ERROR_STATE,
    // Low level drivers errors.
    PBL2_ERROR_DRIVER_KEYBOARD,
    PBL2_ERROR_DRIVER_SOUND,
    // Last index.
    PBL2_ERROR_LAST
} PBL2_status_t;

/*!******************************************************************
 * \enum PBL2_state_t
 * \brief Train brake states.
 *******************************************************************/
typedef enum {
    PBL2_STATE_ON = 0,
    PBL2_STATE_OFF,
    PBL2_STATE_LAST
} PBL2_state_t;

/*** PBL2 functions ***/

/*!******************************************************************
 * \fn PBL2_status_t PBL2_init(void)
 * \brief Init PBL2 driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
PBL2_status_t PBL2_init(void);

/*!******************************************************************
 * \fn PBL2_status_t PBL2_set_state(PBL2_state_t state)
 * \brief Set train brake state.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
PBL2_status_t PBL2_set_state(PBL2_state_t state);

/*!******************************************************************
 * \fn PBL2_status_t PBL2_process(void)
 * \brief Main process of PBL2 driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
PBL2_status_t PBL2_process(void);

/*******************************************************************/
#define PBL2_exit_error(error_code) { if (pbl2_status != PBL2_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define PBL2_stack_error(void) { if (pbl2_status != PBL2_SUCCESS) { ERROR_stack_add((ERROR_BASE_PBL2 * ERROR_BASE_STEP) + pbl2_status); } }

/*******************************************************************/
#define PBL2_stack_exit_error(error_code) { PBL2_stack_error(); PBL2_exit_error(error_code); }

#endif /* __PBL2_H__ */
