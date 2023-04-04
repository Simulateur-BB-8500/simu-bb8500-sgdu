/*
 * openrails.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __ORTS_H__
#define __ORTS_H__

#include "stdint.h"

/*** ORTS structures ***/

typedef enum {
	ORTS_SUCCESS = 0,
	ORTS_ERROR_NULL_PARAMETER,
	ORTS_ERROR_CURL_INIT,
	ORTS_ERROR_CURL_REQUEST,
	ORTS_ERROR_API_SAMPLE,
	ORTS_ERROR_API_SAMPLE_NOT_FOUND,
	ORTS_ERROR_API_SAMPLE_PARSING,
	ORTS_ERROR_BASE_LAST = 0x0100
} ORTS_status_t;

/*** ORTS functions ***/

ORTS_status_t ORTS_init_server(void);
ORTS_status_t ORTS_task(void);

#endif /* __ORTS_H__ */
