/*
 * serial.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef WINDOWS
#include "windows.h"
#endif
#ifdef LINUX
#include <termios.h>
#endif
#include "stdint.h"

/*** SERIAL structures ***/

typedef enum {
	SERIAL_SUCCESS,
	SERIAL_ERROR_OPEN,
	SERIAL_ERROR_WRITE,
	SERIAL_ERROR_READ
} SERIAL_status_t;

typedef struct {
#ifdef WINDOWS
	HANDLE handle;
#endif
#ifdef LINUX
	int descriptor;
	struct termios tty;
#endif
} SERIAL_port_t;

/*** SERIAL functions ***/

SERIAL_status_t SERIAL_open(SERIAL_port_t* serial_port, char* port);
SERIAL_status_t SERIAL_write(SERIAL_port_t* serial_port, uint8_t tx_byte);
SERIAL_status_t SERIAL_read(SERIAL_port_t* serial_port, uint8_t* rx_byte);
void SERIAL_flush(SERIAL_port_t* serial_port);
void SERIAL_close(SERIAL_port_t* serial_port);

#endif /* __SERIAL_H__ */
