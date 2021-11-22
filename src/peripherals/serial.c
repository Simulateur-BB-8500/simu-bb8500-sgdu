/*
 * serial.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "strings.h"
#include "serial.h"
#include "stdio.h"

/*** SERIAL macros ***/

#ifdef WINDOWS
#define SERIAL_PORT_TIMEOUT_MS			10
#define SERIAL_PORT_NAME_MAX_LENGTH		5 // Maximum length = "COMxx" = 5.
#define SERIAL_PATH_HEADER_LENGTH 		4 // Length = "\\.\" = 4.
#endif
//#define SERIAL_LOG

/*** SERIAL functions ***/

/* OPEN A SERIAL COMMUNICATION THROUGH AN USB PORT AND CHECK THE CONNECTION.
 * @param handle:		Pointer to HANDLE.
 * @param port:			Port number to open ("COMxx").
 * @param baud_rate:	Baud rate in symbol per seconds.
 * @return :			None.
 */
void SERIAL_Open(SERIAL_Port_t* serial_port, char* port, unsigned int baud_rate) {
#ifdef WINDOWS
	if ((serial_port -> handle) != NULL) {
		(*(serial_port -> handle)) = INVALID_HANDLE_VALUE;
		// Build COM port path = "\\.\COMxx".
		char port_name[SERIAL_PATH_HEADER_LENGTH + SERIAL_PORT_NAME_MAX_LENGTH + 1]; // +1 for '\0'.
		port_name[0] = '\\';
		port_name[1] = '\\';
		port_name[2] = '.';
		port_name[3] = '\\';
		unsigned char i = 0;
		for (i=0; i<SERIAL_PORT_NAME_MAX_LENGTH ; i++) {
			port_name[SERIAL_PATH_HEADER_LENGTH + i] = port[i];
			if (port[i] == '\0') {
				break;
			}
		}
		// Fill remaining characters with '\0'.
		for (; i<(SERIAL_PORT_NAME_MAX_LENGTH + 1) ; i++) {
			port_name[SERIAL_PATH_HEADER_LENGTH + i] = '\0';
		}
#ifdef SERIAL_LOG
		printf("SERIAL *** Opening port %s: ", port);
#endif
		// Create handle.
		(*(serial_port -> handle)) = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		// Configure serial port settings.
		DCB config;
		GetCommState((*handle), &config);
		config.DCBlength = sizeof(DCB);
		config.BaudRate = baud_rate;
		config.ByteSize = 8;
		SetCommState((*handle), &config);
		// Configure timeouts.
		COMMTIMEOUTS timeouts = {0};
		timeouts.ReadIntervalTimeout = SERIAL_PORT_TIMEOUT_MS;
		timeouts.ReadTotalTimeoutConstant = SERIAL_PORT_TIMEOUT_MS;
		timeouts.ReadTotalTimeoutMultiplier = SERIAL_PORT_TIMEOUT_MS;
		timeouts.WriteTotalTimeoutConstant = SERIAL_PORT_TIMEOUT_MS;
		timeouts.WriteTotalTimeoutMultiplier = SERIAL_PORT_TIMEOUT_MS;
		SetCommTimeouts((*handle), &timeouts);
#ifdef SERIAL_LOG
		if ((*(serial_port -> handle)) == INVALID_HANDLE_VALUE) {
			printf("Error.\n");
		}
		else {
			printf("OK.\n");
		}
#endif
	}
#ifdef SERIAL_LOG
	else {
		printf("Error (null handle pointer).\n");
	}
	fflush(stdout);
#endif
#endif
}

/* SEND DATA TO SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @param tx_byte:	Byte to send.
 * @return result: 	0 if function failed, non-zero value otherwise.
 */
unsigned char SERIAL_Write(SERIAL_Port_t* serial_port, unsigned char tx_byte) {
	// Local variables.
	unsigned char result = 0;
#ifdef WINDOWS
	if (((serial_port -> handle) != NULL) && ((*(serial_port -> handle)) != INVALID_HANDLE_VALUE)) {
		result = WriteFile((*(serial_port -> handle)), &tx_byte, 1, NULL, NULL);
	}
#ifdef SERIAL_LOG
	if (result == 0) {
		printf("SERIAL *** Write byte %d error.\n", tx_byte);
	}
	else {
		printf("SERIAL *** Write byte %d success.\n", tx_byte);
	}
	fflush(stdout);
#endif
#endif
	return result;
}

/* READ DATA FROM SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @param rx_byte:	Pointer to byte that will contain received data.
 * @return result:	0 if function failed, non-zero value otherwise.
 */
unsigned char SERIAL_Read(SERIAL_Port_t* serial_port, unsigned char* rx_byte) {
	unsigned char result = 0;
#ifdef WINDOWS
	if (((serial_port -> handle) != NULL) && ((*(serial_port -> handle)) != INVALID_HANDLE_VALUE)) {
		result = ReadFile((*(serial_port -> handle)), rx_byte, 1, NULL, NULL);
#ifdef SERIAL_LOG
		printf("SERIAL *** Read byte 0x%x.\n", rx_byte);
		fflush(stdout);
#endif
	}
#endif
	return result;
}

/* CLEAN SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @return:			None.
 */
void SERIAL_Flush(SERIAL_Port_t* serial_port) {
#ifdef WINDOWS
	if (((serial_port -> handle) != NULL) && ((*(serial_port -> handle)) != INVALID_HANDLE_VALUE)) {
		PurgeComm((*(serial_port -> handle)), PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	}
#endif
}

/* CLOSE SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @return:			None.
 */
void SERIAL_Close(SERIAL_Port_t* serial_port) {
#ifdef WINDOWS
	if (((serial_port -> handle) != NULL) && ((*(serial_port -> handle)) != INVALID_HANDLE_VALUE)) {
		CloseHandle((*(serial_port -> handle)));
	}
#endif
}
