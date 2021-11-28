/*
 * serial.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef SERIAL_H
#define SERIAL_H

#ifdef WINDOWS
#include "windows.h"
#endif
#ifdef LINUX
#include <termios.h>
#endif

/*** SERIAL structures ***/

typedef enum {
	SERIAL_SUCCESS,
	SERIAL_ERROR_OPEN,
	SERIAL_ERROR_WRITE,
	SERIAL_ERROR_READ
} SERIAL_Error_t;

typedef struct {
#ifdef WINDOWS
	HANDLE handle;
#endif
#ifdef LINUX
	int descriptor;
	struct termios tty;
#endif
} SERIAL_Port_t;

/*** SERIAL functions ***/

SERIAL_Error_t SERIAL_Open(SERIAL_Port_t* serial_port, char* port);
SERIAL_Error_t SERIAL_Write(SERIAL_Port_t* serial_port, unsigned char tx_byte);
SERIAL_Error_t SERIAL_Read(SERIAL_Port_t* serial_port, unsigned char* rx_byte);
void SERIAL_Flush(SERIAL_Port_t* serial_port);
void SERIAL_Close(SERIAL_Port_t* serial_port);

#endif /* SERIAL_H */
