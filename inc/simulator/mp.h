/*
 * mp.h
 *
 *  Created on: 09 may 2020
 *      Author: Ludo
 */

#ifndef __MP_H__
#define __MP_H__

#include "stdint.h"

/*** MP structures ***/

/*!******************************************************************
 * \enum MP_status_t
 * \brief MP driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	MP_SUCCESS = 0,
	MP_ERROR_EVENT,
	// Low level drivers errors.
	MP_ERROR_DRIVER_KEYBOARD,
	MP_ERROR_DRIVER_SOUND,
	// Last index.
	MP_ERROR_LAST
} MP_status_t;

/*!******************************************************************
 * \enum MP_event_t
 * \brief MP events list.
 *******************************************************************/
typedef enum {
	MP_EVENT_0 = 0,
	MP_EVENT_T_MORE,
	MP_EVENT_T_LESS,
	MP_EVENT_P,
	MP_EVENT_F_MORE,
	MP_EVENT_F_LESS,
	MP_EVENT_LAST
} MP_event_t;

/*** MP functions ***/

/*!******************************************************************
 * \fn MP_status_t MP_init(void)
 * \brief Init MP driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
MP_status_t MP_init(void);

/*!******************************************************************
 * \fn MP_status_t MP_set_event(MP_event_t event)
 * \brief Set throttle event.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
MP_status_t MP_set_event(MP_event_t event);

/*!******************************************************************
 * \fn MP_status_t MP_set_current_position(int32_t drive_level, int32_t dynamic_brake_level)
 * \brief Set throttle position.
 * \param[in]  	drive_level: Current drive level in percent.
 * \param[in]	dynamic_brake_level: Current dynamic brake level in percent.
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void MP_set_current_position(int32_t drive_level, int32_t dynamic_brake_level);

/*!******************************************************************
 * \fn MP_status_t MP_process(void)
 * \brief Main process of MP driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
MP_status_t MP_process(void);

/*******************************************************************/
#define MP_exit_error(error_code) { if (mp_status != MP_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define MP_stack_error(void) { if (mp_status != MP_SUCCESS) { ERROR_stack_add((ERROR_BASE_MP * ERROR_BASE_STEP) + mp_status); } }

/*******************************************************************/
#define MP_stack_exit_error(error_code) { MP_stack_error(); MP_exit_error(error_code); }

#endif /* __MP_H__ */
