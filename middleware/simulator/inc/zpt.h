/*
 * zpt.h
 *
 *  Created on: 26 mar. 2020
 *      Author: Ludo
 */

#ifndef __ZPT_H__
#define __ZPT_H__

/*** ZPT structures ***/

/*!******************************************************************
 * \enum ZPT_status_t
 * \brief ZPT driver error codes.
 *******************************************************************/
typedef enum {
    // Drivers errors.
    ZPT_SUCCESS = 0,
    ZPT_ERROR_PANTOGRAPH,
    ZPT_ERROR_STATE,
    // Low level drivers errors.
    ZPT_ERROR_DRIVER_KEYBOARD,
    ZPT_ERROR_DRIVER_SOUND,
    // Last index.
    ZPT_ERROR_LAST
} ZPT_status_t;

/*!******************************************************************
 * \enum ZPT_pantograph_t
 * \brief Pantograph types.
 *******************************************************************/
typedef enum {
    ZPT_PANTOGRAPH_REAR = 0,
    ZPT_PANTOGRAPH_FRONT,
    ZPT_PANTOGRAPH_LAST
} ZPT_pantograph_t;

/*!******************************************************************
 * \enum ZPT_state_t
 * \brief Pantograph states.
 *******************************************************************/
typedef enum {
    ZPT_STATE_DOWN = 0,
    ZPT_STATE_UP,
    ZPT_STATE_LAST
} ZPT_state_t;

/*** ZPT functions ***/

/*!******************************************************************
 * \fn ZPT_status_t ZPT_init(void)
 * \brief Init ZPT driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
ZPT_status_t ZPT_init(void);

/*!******************************************************************
 * \fn ZPT_status_t ZPT_set_position(ZPT_pantograph_t pantograph, ZPT_state_t state)
 * \brief Set pantograph position.
 * \param[in]   pantograph: Pantograph to control.
 * \param[in]   state: New state to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
ZPT_status_t ZPT_set_position(ZPT_pantograph_t pantograph, ZPT_state_t state);

/*******************************************************************/
#define ZPT_exit_error(error_code) { if (zpt_status != ZPT_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define ZPT_stack_error(void) { if (zpt_status != ZPT_SUCCESS) { ERROR_stack_add((ERROR_BASE_ZPT * ERROR_BASE_STEP) + zpt_status); } }

/*******************************************************************/
#define ZPT_stack_exit_error(error_code) { ZPT_stack_error(); ZPT_exit_error(error_code); }

#endif /* __ZPT_H__ */
