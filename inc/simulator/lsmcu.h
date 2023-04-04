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

typedef enum {
	LSMCU_SUCCESS = 0,
	LSMCU_ERROR_SERIAL_OPEN,
	LSMCU_ERROR_BASE_LAST = 0x0100
} LSMCU_status_t;

/*** LSMCU functions ***/

LSMCU_status_t LSMCU_init(char* port);
void LSMCU_send(uint8_t tx_command);
void LSMCU_task(void);

#endif /* __LSMCU_H__ */
