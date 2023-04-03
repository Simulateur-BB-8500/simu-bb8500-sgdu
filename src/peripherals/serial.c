/*
 * serial.c
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#include "serial.h"

#include "string.h"
#include "stdint.h"
#include "stdio.h"
#ifdef WINDOWS
#include "windef.h"
#include "windows.h"
#endif
#ifdef LINUX
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

/*** SERIAL macros ***/

#ifdef WINDOWS
#define SERIAL_port_tIMEOUT_MS			10
#define SERIAL_PATH_HEADER				"\\\\.\\"
#define SERIAL_PORT_NAME_MAX_LENGTH		5 // Maximum length = "COMxx" = 5.
#define SERIAL_PORT_BAUD_RATE			9600
#endif
#ifdef LINUX
#define SERIAL_PATH_HEADER				"/dev/tty"
#define SERIAL_PORT_NAME_MAX_LENGTH		5 // Maximum length = "USBxx" or "ACMxx" = 5.
#define SERIAL_PORT_BAUD_RATE			B9600
#endif
//#define SERIAL_LOG

/*** SERIAL functions ***/

/* OPEN A SERIAL COMMUNICATION THROUGH AN USB PORT AND CHECK THE CONNECTION.
 * @param handle:		Pointer to HANDLE.
 * @param port:			Port number to open ("COMxx" or "USBxx").
 * @return status:		Opening status.
 */
SERIAL_status_t SERIAL_open(SERIAL_port_t* serial_port, char* port) {
	// Local variables.
	SERIAL_status_t status = SERIAL_ERROR_OPEN;
	char port_name[strlen(SERIAL_PATH_HEADER) + SERIAL_PORT_NAME_MAX_LENGTH + 1]; // +1 for '\0'.
	// Check parameters.
	if ((serial_port == NULL) || (port == NULL)) goto errors;
	// Build port full path.
	sprintf(port_name, "%s%s", SERIAL_PATH_HEADER, port);
#ifdef SERIAL_LOG
	printf("SERIAL *** Opening port %s: ", port_name);
#endif
#ifdef WINDOWS
	(serial_port -> handle) = INVALID_HANDLE_VALUE;
	// Create handle.
	(serial_port -> handle) = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	// Configure serial port settings.
	DCB config;
	GetCommState((serial_port -> handle), &config);
	config.DCBlength = sizeof(DCB);
	config.BaudRate = SERIAL_PORT_BAUD_RATE;
	config.ByteSize = 8;
	SetCommState((serial_port -> handle), &config);
	// Configure timeouts.
	COMMTIMEOUTS timeouts = {0};
	timeouts.ReadIntervalTimeout = SERIAL_port_tIMEOUT_MS;
	timeouts.ReadTotalTimeoutConstant = SERIAL_port_tIMEOUT_MS;
	timeouts.ReadTotalTimeoutMultiplier = SERIAL_port_tIMEOUT_MS;
	timeouts.WriteTotalTimeoutConstant = SERIAL_port_tIMEOUT_MS;
	timeouts.WriteTotalTimeoutMultiplier = SERIAL_port_tIMEOUT_MS;
	SetCommTimeouts((serial_port -> handle), &timeouts);
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) goto errors;
#endif
#ifdef LINUX
	// Open serial port.
	(serial_port -> descriptor) = open(port_name, O_RDWR);
	if ((serial_port -> descriptor) < 0) goto errors;
	// Configure serial port.
	if (tcgetattr((serial_port -> descriptor), &(serial_port -> tty)) < 0) goto errors;
	(serial_port -> tty).c_cflag &= ~PARENB;
	(serial_port -> tty).c_cflag &= ~CSTOPB;
	(serial_port -> tty).c_cflag &= ~CSIZE;
	(serial_port -> tty).c_cflag |= CS8;
	(serial_port -> tty).c_cflag &= ~CRTSCTS;
	(serial_port -> tty).c_cflag |= CREAD | CLOCAL;
	(serial_port -> tty).c_lflag &= ~ICANON;
	(serial_port -> tty).c_lflag &= ~ECHO;
	(serial_port -> tty).c_lflag &= ~ISIG;
	(serial_port -> tty).c_iflag &= ~(IXON | IXOFF | IXANY);
	(serial_port -> tty).c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
	(serial_port -> tty).c_oflag &= ~OPOST;
	(serial_port -> tty).c_oflag &= ~ONLCR;
	(serial_port -> tty).c_cc[VTIME] = 0;
	(serial_port -> tty).c_cc[VMIN] = 0;
	// Baud rate.
	cfsetispeed(&(serial_port -> tty), SERIAL_PORT_BAUD_RATE);
	cfsetospeed(&(serial_port -> tty), SERIAL_PORT_BAUD_RATE);
	// Save settings.
	if (tcsetattr((serial_port -> descriptor), TCSANOW, &(serial_port -> tty)) != 0) goto errors;
#endif
	// Update status.
	status = SERIAL_SUCCESS;
errors:
#ifdef SERIAL_LOG
	printf("%s\n", ((status == SERIAL_SUCCESS) ? "OK" : "Error"));
#endif
	return status;
}

/* SEND DATA TO SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @param tx_byte:	Byte to send.
 * @return result: 	0 if function failed, non-zero value otherwise.
 */
SERIAL_status_t SERIAL_write(SERIAL_port_t* serial_port, uint8_t tx_byte) {
	// Local variables.
	SERIAL_status_t status = SERIAL_ERROR_WRITE;
	// Check parameters.
	if (serial_port == NULL) goto errors;
#ifdef SERIAL_LOG
	printf("SERIAL *** TX byte 0x%x: ", tx_byte);
#endif
#ifdef WINDOWS
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) goto errors;
	if (WriteFile((serial_port -> handle), &tx_byte, 1, NULL, NULL) == 0) goto errors;
#endif
#ifdef LINUX
	if ((serial_port -> descriptor) < 0) goto errors;
	if (write((serial_port -> descriptor), &tx_byte, 1) == 0) goto errors;
#endif
	// Update status.
	status = SERIAL_SUCCESS;
errors:
#ifdef SERIAL_LOG
	printf("%s\n", ((status == SERIAL_SUCCESS) ? "OK" : "Error"));
#endif
	return status;
}

/* READ DATA FROM SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @param rx_byte:	Pointer to byte that will contain received data.
 * @return result:	0 if function failed, non-zero value otherwise.
 */
SERIAL_status_t SERIAL_read(SERIAL_port_t* serial_port, uint8_t* rx_byte) {
	// Local variables.
	SERIAL_status_t status = SERIAL_ERROR_READ;
#ifdef WINDOWS
	DWORD number_of_read_bytes = 0;
#endif
	// Check parameters.
	if (serial_port == NULL) goto errors;
#ifdef WINDOWS
	if ((serial_port -> handle) == INVALID_HANDLE_VALUE) goto errors;
	if (ReadFile((serial_port -> handle), rx_byte, 1, &number_of_read_bytes, NULL) == 0) goto errors;
	if (number_of_read_bytes == 0) goto errors;
#endif
#ifdef LINUX
	if ((serial_port -> descriptor) < 0) goto errors;
	if (read((serial_port -> descriptor), rx_byte, 1) == 0) goto errors;
#endif
	// Update status.
	status = SERIAL_SUCCESS;
#ifdef SERIAL_LOG
	printf("SERIAL *** RX byte 0x%x.\n", (*rx_byte));
#endif
errors:
	return status;
}

/* CLEAN SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @return:			None.
 */
void SERIAL_flush(SERIAL_port_t* serial_port) {
#ifdef WINDOWS
	if ((serial_port != NULL) && ((serial_port -> handle) != INVALID_HANDLE_VALUE)) {
		PurgeComm((serial_port -> handle), PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	}
#endif
}

/* CLOSE SERIAL PORT.
 * @param handle:	Pointer to handle.
 * @return:			None.
 */
void SERIAL_close(SERIAL_port_t* serial_port) {
	// Check parameter.
	if (serial_port == NULL) goto errors;
#ifdef WINDOWS
	if ((serial_port -> handle) != INVALID_HANDLE_VALUE) {
		CloseHandle(serial_port -> handle);
	}
#endif
#ifdef LINUX
	close((serial_port -> descriptor));
#endif
errors:
	return;
}
