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

/*** SERIAL structures ***/

typedef struct {
#ifdef WINDOWS
	HANDLE* handle;
#endif
} SERIAL_Port_t;

/*** SERIAL functions ***/

void SERIAL_Open(SERIAL_Port_t* serial_port, char port[], unsigned int baud_rate);
unsigned char SERIAL_Write(SERIAL_Port_t* serial_port, unsigned char tx_byte);
unsigned char SERIAL_Read(SERIAL_Port_t* serial_port, unsigned char* rx_byte);
void SERIAL_Flush(SERIAL_Port_t* serial_port);
void SERIAL_Close(SERIAL_Port_t* serial_port);

#endif /* SERIAL_H */
