/*
 * serial.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "windows.h"
#include "stdint.h"

/*** SERIAL structures ***/

/*!******************************************************************
 * \enum SERIAL_status_t
 * \brief SERIAL driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    SERIAL_SUCCESS,
    SERIAL_ERROR_NULL_PARAMETER,
    SERIAL_ERROR_INVALID_HANDLE,
    // Low level drivers errors.
    SERIAL_ERROR_DRIVER_WINDOWS,
    // Last index.
    SERIAL_ERROR_LAST
} SERIAL_status_t;

/*!******************************************************************
 * \enum SERIAL_port_t
 * \brief Serial port structure.
 *******************************************************************/
typedef struct {
    HANDLE handle;
} SERIAL_port_t;

/*** SERIAL functions ***/

/*!******************************************************************
 * \fn SERIAL_status_t SERIAL_open(SERIAL_port_t* serial_port, char* port)
 * \brief Open serial port.
 * \param[in]   serial_port: Pointer to the serial port to open.
 * \param[in]   port: Port name ("COMxx").
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SERIAL_status_t SERIAL_open(SERIAL_port_t* serial_port, char* port);

/*!******************************************************************
 * \fn SERIAL_status_t SERIAL_close(SERIAL_port_t* serial_port)
 * \brief Close serial port.
 * \param[in]   serial_port: Pointer to the serial port to close.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SERIAL_status_t SERIAL_close(SERIAL_port_t* serial_port);

/*!******************************************************************
 * \fn SERIAL_status_t SERIAL_write(SERIAL_port_t* serial_port, uint8_t tx_byte)
 * \brief Write data on serial port.
 * \param[in]   serial_port: Pointer to the serial port to write on.
 * \param[in]   tx_byte: Byte to send.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SERIAL_status_t SERIAL_write(SERIAL_port_t* serial_port, uint8_t tx_byte);

/*!******************************************************************
 * \fn SERIAL_status_t SERIAL_read(SERIAL_port_t* serial_port, uint8_t* rx_byte)
 * \brief Read data from serial port.
 * \param[in]   serial_port: Pointer to the serial port to read from.
 * \param[in]   rx_byte: Pointer to byte that will contain the received data if available.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SERIAL_status_t SERIAL_read(SERIAL_port_t* serial_port, uint8_t* rx_byte);

/*!******************************************************************
 * \fn SERIAL_status_t SERIAL_flush(SERIAL_port_t* serial_port)
 * \brief Flush serial port.
 * \param[in]   serial_port: Pointer to the serial port to flush.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SERIAL_status_t SERIAL_flush(SERIAL_port_t* serial_port);

/*******************************************************************/
#define SERIAL_exit_error(error_code) { if (serial_status != SERIAL_SUCCESS) { status = error_code; goto errors; } }

/*******************************************************************/
#define SERIAL_stack_error(void) { if (serial_status != SERIAL_SUCCESS) { ERROR_stack_add((ERROR_BASE_SERIAL * ERROR_BASE_STEP) + serial_status); } }

/*******************************************************************/
#define SERIAL_stack_exit_error(error_code) { SERIAL_stack_error(); SERIAL_exit_error(error_code); }

#endif /* __SERIAL_H__ */
