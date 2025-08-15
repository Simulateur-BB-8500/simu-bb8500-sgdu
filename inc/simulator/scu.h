/*
 * scu.h
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#ifndef __SCU_H__
#define __SCU_H__

#include "stdint.h"

/*** SCU structures ***/

/*!******************************************************************
 * \enum SCU_status_t
 * \brief SCU driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    SCU_SUCCESS = 0,
    SCU_ERROR_NULL_PARAMETER,
    SCU_ERROR_UNKNOWN_COMMAND,
    // Low level drivers errors.
    SCU_ERROR_DRIVER_SERIAL,
    SCU_ERROR_DRIVER_BPGD,
    SCU_ERROR_DRIVER_BPSA,
    SCU_ERROR_DRIVER_COMPRESSOR,
    SCU_ERROR_DRIVER_EMERGENCY,
    SCU_ERROR_DRIVER_FD,
    SCU_ERROR_DRIVER_FPB,
    SCU_ERROR_DRIVER_KVB,
    SCU_ERROR_DRIVER_LIGHT,
    SCU_ERROR_DRIVER_MP,
    SCU_ERROR_DRIVER_MPINV,
    SCU_ERROR_DRIVER_PBL2,
    SCU_ERROR_DRIVER_WHISTLE,
    SCU_ERROR_DRIVER_ZDJ,
    SCU_ERROR_DRIVER_ZPT,
    SCU_ERROR_DRIVER_ZVM,
    // Last index.
    SCU_ERROR_LAST
} SCU_status_t;

/*** SCU functions ***/

/*!******************************************************************
 * \fn SCU_status_t SCU_init(char* port)
 * \brief Init SCU driver.
 * \param[in]   port: Serial port name to open.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SCU_status_t SCU_init(char* port);

/*!******************************************************************
 * \fn SCU_status_t SCU_send(uint8_t tx_command)
 * \brief Send a command to SCU board.
 * \param[in]   tx_command: Command to send.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SCU_status_t SCU_send(uint8_t tx_command);

/*!******************************************************************
 * \fn SCU_status_t SCU_process(void)
 * \brief Process SCU driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SCU_status_t SCU_process(void);

/*******************************************************************/
#define SCU_exit_error(error_code) { if (scu_status != SCU_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define SCU_stack_error(void) { if (scu_status != SCU_SUCCESS) { ERROR_stack_add((ERROR_BASE_SCU * ERROR_BASE_STEP) + scu_status); } }

/*******************************************************************/
#define SCU_stack_exit_error(error_code) { SCU_stack_error(); SCU_exit_error(error_code); }

#endif /* __SCU_H__ */
