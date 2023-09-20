/*
 * comp.h
 *
 *  Created on: 08 may 2020
 *      Author: Ludo
 */

#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

/*** COMPRESSOR structures ***/

/*!******************************************************************
 * \enum COMPRESSOR_status_t
 * \brief COMPRESSOR driver error codes.
 *******************************************************************/
typedef enum {
	// Drivers errors.
	COMPRESSOR_SUCCESS = 0,
	COMPRESSOR_ERROR_SOUND_REQUEST,
	COMPRESSOR_ERROR_INTERNAL_STATE,
	// Low level drivers errors.
	COMPRESSOR_ERROR_DRIVER_SOUND,
	// Last index.
	COMPRESSOR_ERROR_LAST
} COMPRESSOR_status_t;

/*!******************************************************************
 * \enum COMPRESSOR_sound_request_t
 * \brief COMPRESSOR sound requests list.
 *******************************************************************/
typedef enum {
	COMPRESSOR_SOUND_REQUEST_NONE = 0,
	COMPRESSOR_SOUND_REQUEST_ZCA_MIN,
	COMPRESSOR_SOUND_REQUEST_ZCA_MAX,
	COMPRESSOR_SOUND_REQUEST_ZCD,
	COMPRESSOR_SOUND_REQUEST_OFF,
	COMPRESSOR_SOUND_REQUEST_LAST
} COMPRESSOR_sound_request_t;

/*** COMP functions ***/

/*!******************************************************************
 * \fn COMPRESSOR_status_t COMPRESSOR_init(void)
 * \brief Init COMPRESSOR driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
COMPRESSOR_status_t COMPRESSOR_init(void);

/*!******************************************************************
 * \fn COMPRESSOR_status_t COMPRESSOR_set_request(COMPRESSOR_sound_request_t sound_request)
 * \brief Make compressor sound request.
 * \param[in]  	sound_request: Sound request to send.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
COMPRESSOR_status_t COMPRESSOR_set_request(COMPRESSOR_sound_request_t sound_request);

/*!******************************************************************
 * \fn COMPRESSOR_status_t COMPRESSOR_process(void)
 * \brief Main process of COMPRESSOR driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
COMPRESSOR_status_t COMPRESSOR_process(void);

/*******************************************************************/
#define COMPRESSOR_exit_error(error_code) { if (compressor_status != COMPRESSOR_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define COMPRESSOR_stack_error(void) { if (compressor_status != COMPRESSOR_SUCCESS) { ERROR_stack_add((ERROR_BASE_COMPRESSOR * ERROR_BASE_STEP) + compressor_status); } }

/*******************************************************************/
#define COMPRESSOR_stack_exit_error(error_code) { COMPRESSOR_stack_error(); COMPRESSOR_exit_error(error_code); }

#endif /* __COMPRESSOR_H__ */
