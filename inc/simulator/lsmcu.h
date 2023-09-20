/*
 * lsmcu.h
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#ifndef __LSMCU_H__
#define __LSMCU_H__

#include "stdint.h"

/*** LSMCU structures ***/

/*!******************************************************************
 * \enum LSMCU_status_t
 * \brief LSMCU driver error codes.
 *******************************************************************/
typedef enum {
	// Driver errors.
	LSMCU_SUCCESS = 0,
	LSMCU_ERROR_NULL_PARAMETER,
	LSMCU_ERROR_UNKNOWN_COMMAND,
	// Low level drivers errors.
	LSMCU_ERROR_DRIVER_SERIAL,
	LSMCU_ERROR_DRIVER_COMPRESSOR,
	LSMCU_ERROR_DRIVER_FD,
	LSMCU_ERROR_DRIVER_FPB,
	LSMCU_ERROR_DRIVER_KVB,
	LSMCU_ERROR_DRIVER_LIGHT,
	LSMCU_ERROR_DRIVER_MP,
	LSMCU_ERROR_DRIVER_MPINV,
	LSMCU_ERROR_DRIVER_WHISTLE,
	LSMCU_ERROR_DRIVER_ZDJ,
	LSMCU_ERROR_DRIVER_ZPT,
	LSMCU_ERROR_DRIVER_ZVM,
	// Last index.
	LSMCU_ERROR_LAST
} LSMCU_status_t;

/*** LSMCU functions ***/

/*!******************************************************************
 * \fn LSMCU_status_t LSMCU_init(char* port)
 * \brief Init LSMCU driver.
 * \param[in]  	port: Serial port name to open.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
LSMCU_status_t LSMCU_init(char* port);

/*!******************************************************************
 * \fn LSMCU_status_t LSMCU_send(uint8_t tx_command)
 * \brief Send a command to LSMCU board.
 * \param[in]  	tx_command: Command to send.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
LSMCU_status_t LSMCU_send(uint8_t tx_command);

/*!******************************************************************
 * \fn LSMCU_status_t LSMCU_process(void)
 * \brief Process LSMCU driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
LSMCU_status_t LSMCU_process(void);

/*******************************************************************/
#define LSMCU_exit_error(error_code) { if (lsmcu_status != LSMCU_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define LSMCU_stack_error(void) { if (lsmcu_status != LSMCU_SUCCESS) { ERROR_stack_add((ERROR_BASE_LSMCU * ERROR_BASE_STEP) + lsmcu_status); } }

/*******************************************************************/
#define LSMCU_stack_exit_error(error_code) { LSMCU_stack_error(); LSMCU_exit_error(error_code); }

#endif /* __LSMCU_H__ */
