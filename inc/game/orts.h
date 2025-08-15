/*
 * orts.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __ORTS_H__
#define __ORTS_H__

#include "stdint.h"

/*** ORTS structures ***/

/*!******************************************************************
 * \enum ORTS_status_t
 * \brief ORTS driver error codes.
 *******************************************************************/
typedef enum {
	// Driver errors.
	ORTS_SUCCESS = 0,
	ORTS_ERROR_NULL_PARAMETER,
	ORTS_ERROR_CURL_INIT,
	ORTS_ERROR_UNIT_NOT_FOUND,
	ORTS_ERROR_DATA_PARSING,
	ORTS_ERROR_API_SAMPLE_INDEX,
	// Low level drivers errors.
	ORTS_ERROR_DRIVER_CURL,
	ORTS_ERROR_DRIVER_SCU,
	ORTS_ERROR_DRIVER_FPB,
	ORTS_ERROR_DRIVER_TRACK,
	// Last index.
	ORTS_ERROR_LAST
} ORTS_status_t;

/*** ORTS functions ***/

/*!******************************************************************
 * \fn ORTS_status_t ORTS_get_curl_version(uint8_t* major, uint8_t* minor, uint8_t* patch)
 * \brief Get HTTP request driver version.
 * \param[in]  	none
 * \param[out] 	product: CURL product version.
 * \param[out] 	major: CURL major version.
 * \param[out] 	minor: CURL minor version.
 * \retval		Function execution status.
 *******************************************************************/
ORTS_status_t ORTS_get_curl_version(uint8_t* major, uint8_t* minor, uint8_t* patch);

/*!******************************************************************
 * \fn ORTS_status_t ORTS_get_cjson_version(uint8_t* major, uint8_t* minor, uint8_t* patch)
 * \brief Get JSON parser version.
 * \param[in]  	none
 * \param[out] 	product: CURL product version.
 * \param[out] 	major: CURL major version.
 * \param[out] 	minor: CURL minor version.
 * \retval		Function execution status.
 *******************************************************************/
ORTS_status_t ORTS_get_cjson_version(uint8_t* major, uint8_t* minor, uint8_t* patch);

/*!******************************************************************
 * \fn ORTS_status_t ORTS_init(void)
 * \brief Init ORTS server.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ORTS_status_t ORTS_init(void);

/*!******************************************************************
 * \fn ORTS_status_t ORTS_process(void)
 * \brief Process ORTS server.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
ORTS_status_t ORTS_process(void);

/*******************************************************************/
#define ORTS_exit_error(error_code) { if (orts_status != ORTS_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define ORTS_stack_error(void) { if (orts_status != ORTS_SUCCESS) { ERROR_stack_add((ERROR_BASE_ORTS * ERROR_BASE_STEP) + orts_status); } }

/*******************************************************************/
#define ORTS_stack_exit_error(error_code) { ORTS_stack_error(); ORTS_exit_error(error_code); }

#endif /* __ORTS_H__ */
