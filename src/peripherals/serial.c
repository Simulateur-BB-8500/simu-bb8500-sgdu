/*
 * serial.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "serial.h"

#include "error.h"
#include "log.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"
#include "windef.h"
#include "windows.h"

/*** SERIAL macros ***/

#define SERIAL_PORT_TIMEOUT_MS			10
#define SERIAL_PATH_HEADER				"\\\\.\\"
#define SERIAL_PORT_NAME_MAX_LENGTH		5 // Maximum length = "COMxx" = 5.
#define SERIAL_PORT_BAUD_RATE			115200

/*** SERIAL local functions ***/

/*******************************************************************/
#define WINDOWS_exit_error(error_code) { if (windows_status == 0) { status = error_code; goto errors; } }

/*******************************************************************/
#define WINDOWS_stack_error(void) { if (windows_status == 0) { ERROR_stack_add((ERROR_BASE_WINDOWS * ERROR_BASE_STEP) + windows_status); } }

/*******************************************************************/
#define WINDOWS_stack_exit_error(error_code) { WINDOWS_stack_error(); WINDOWS_exit_error(error_code); }

/*** SERIAL functions ***/

/*******************************************************************/
SERIAL_status_t SERIAL_open(SERIAL_port_t* serial_port, char* port) {
	// Local variables.
	SERIAL_status_t status = SERIAL_SUCCESS;
	BOOL windows_status = 0;
	DCB config;
	COMMTIMEOUTS timeouts = {0};
	char port_name[strlen(SERIAL_PATH_HEADER) + SERIAL_PORT_NAME_MAX_LENGTH + 1]; // +1 for '\0'.
	// Check parameters.
	if ((serial_port == NULL) || (port == NULL)) {
		status = SERIAL_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Build port full path.
	sprintf(port_name, "%s%s", SERIAL_PATH_HEADER, port);
	// Create handle.
	(serial_port -> handle) = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	// Read current parameters.
	windows_status = GetCommState((serial_port -> handle), &config);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
	// Configure serial port settings.
	config.DCBlength = sizeof(DCB);
	config.BaudRate = SERIAL_PORT_BAUD_RATE;
	config.ByteSize = 8;
	// Set parameters.
	windows_status = SetCommState((serial_port -> handle), &config);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
	// Configure timeouts.
	timeouts.ReadIntervalTimeout = SERIAL_PORT_TIMEOUT_MS;
	timeouts.ReadTotalTimeoutConstant = SERIAL_PORT_TIMEOUT_MS;
	timeouts.ReadTotalTimeoutMultiplier = SERIAL_PORT_TIMEOUT_MS;
	timeouts.WriteTotalTimeoutConstant = SERIAL_PORT_TIMEOUT_MS;
	timeouts.WriteTotalTimeoutMultiplier = SERIAL_PORT_TIMEOUT_MS;
	// Set parameters.
	windows_status = SetCommTimeouts((serial_port -> handle), &timeouts);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
	// Check handle.
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) {
		status = SERIAL_ERROR_INVALID_HANDLE;
		goto errors;
	}
errors:
	LOG_ERROR(status, SERIAL_SUCCESS);
	return status;
}

/*******************************************************************/
SERIAL_status_t SERIAL_close(SERIAL_port_t* serial_port) {
	// Local variables.
	SERIAL_status_t status = SERIAL_SUCCESS;
	BOOL windows_status = 0;
	// Check parameter.
	if (serial_port == NULL) {
		status = SERIAL_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Check handle.
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) {
		status = SERIAL_ERROR_INVALID_HANDLE;
		goto errors;
	}
	// Close port.
	windows_status = CloseHandle(serial_port -> handle);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
errors:
	LOG_ERROR(status, SERIAL_SUCCESS);
	return status;
}

/*******************************************************************/
SERIAL_status_t SERIAL_write(SERIAL_port_t* serial_port, uint8_t tx_byte) {
	// Local variables.
	SERIAL_status_t status = SERIAL_SUCCESS;
	BOOL windows_status = 0;
	// Check parameters.
	if (serial_port == NULL) {
		status = SERIAL_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Check handle.
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) {
		status = SERIAL_ERROR_INVALID_HANDLE;
		goto errors;
	}
	// Write byte.
	windows_status = WriteFile((serial_port -> handle), &tx_byte, 1, NULL, NULL);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
#ifdef LOG_SERIAL
	LOG_trace(LOG_COLOR_WHITE, "tx_byte=%d", tx_byte);
#endif
errors:
	LOG_ERROR(status, SERIAL_SUCCESS);
	return status;
}

/*******************************************************************/
SERIAL_status_t SERIAL_read(SERIAL_port_t* serial_port, uint8_t* rx_byte) {
	// Local variables.
	SERIAL_status_t status = SERIAL_SUCCESS;
	BOOL windows_status = 0;
	DWORD number_of_read_bytes = 0;
	// Check parameters.
	if ((serial_port == NULL) || (rx_byte == NULL)) {
		status = SERIAL_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Check handle.
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) {
		status = SERIAL_ERROR_INVALID_HANDLE;
		goto errors;
	}
	// Read byte.
	windows_status = ReadFile((serial_port -> handle), rx_byte, 1, &number_of_read_bytes, NULL);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
#ifdef LOG_SERIAL
	LOG_trace(LOG_COLOR_WHITE, "rx_byte=%d", (*rx_byte));
#endif
errors:
	LOG_ERROR(status, SERIAL_SUCCESS);
	return status;
}

/*******************************************************************/
SERIAL_status_t SERIAL_flush(SERIAL_port_t* serial_port) {
	// Local variables.
	SERIAL_status_t status = SERIAL_SUCCESS;
	BOOL windows_status = 0;
	// Check parameter.
	if (serial_port == NULL) {
		status = SERIAL_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Check handle.
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) {
		status = SERIAL_ERROR_INVALID_HANDLE;
		goto errors;
	}
	// Flush port.
	windows_status = PurgeComm((serial_port -> handle), PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	WINDOWS_stack_exit_error(SERIAL_ERROR_DRIVER_WINDOWS);
errors:
	LOG_ERROR(status, SERIAL_SUCCESS);
	return status;
}
