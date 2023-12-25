/*
 * orts.c
 *
 *  Created on: 4 apr. 2023
 *      Author: Ludo
 */

#include "orts.h"

#include "curl/curl.h"
#include "error.h"
#include "fpb.h"
#include "log.h"
#include "lsagiu.h"
#include "lsmcu.h"
#include "stdint.h"
#include "string.h"
#include "time.h"
#include "track.h"

/*** ORTS macros ***/

#define ORTS_SERVER_ADDRESS				"http://localhost:2150/API/HUD/7"
#define ORTS_CURL_BUFFER_SIZE			16384
#define ORTS_STRING_BUFFER_SIZE			32

#define ORTS_HUD7_SPEED_DATA_KEYWORD	"Chemin"
#define ORTS_HUD7_SPEED_UNIT			"km/h"

#define ORTS_CURL_REQUEST_PERIOD_MS		1000
#define ORTS_REQUEST_TIMEOUT_S			1

/*** ORTS local structures ***/

/*******************************************************************/
typedef enum {
	ORTS_DATA_INDEX_SPEED_KMH = 0,
	ORTS_DATA_INDEX_SPEED_LIMIT_KMH,
	ORTS_DATA_INDEX_LAST
} ORTS_api_sample_t;

/*******************************************************************/
typedef struct {
	CURL* curl;
	char curl_data[ORTS_CURL_BUFFER_SIZE];
	uint32_t curl_data_index;
	uint64_t request_next_time;
	uint8_t data[ORTS_DATA_INDEX_LAST];
} ORTS_context_t;

/*** ORTS local global variables ***/

static ORTS_context_t orts_ctx;

/*** ORTS local functions ***/

/*******************************************************************/
#define CURL_exit_error(error_code) { if (curl_status != CURLE_OK) { status = error_code; goto errors; } }

/*******************************************************************/
#define CURL_stack_error(void) { if (curl_status != CURLE_OK) { ERROR_stack_add((ERROR_BASE_CURL * ERROR_BASE_STEP) + curl_status); } }

/*******************************************************************/
#define CURL_stack_exit_error(error_code) { CURL_stack_error(); CURL_exit_error(error_code); }


/*******************************************************************/
size_t _ORTS_write_api_data(char* ptr, size_t size, size_t nmemb, void* user_data) {
	// Local variables.
	uint32_t idx = 0;
	// Copy data into local buffer.
	for (idx=0 ; idx<nmemb ; idx++) {
		orts_ctx.curl_data[orts_ctx.curl_data_index] = ptr[idx];
		orts_ctx.curl_data_index = (orts_ctx.curl_data_index + 1) % ORTS_CURL_BUFFER_SIZE;
	}
	return nmemb;
}

/*******************************************************************/
ORTS_status_t _ORTS_update_data(void) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	char* data_ptr = NULL;
	const char * separators = "\"";
	int32_t sscanf_count = 0;
	int32_t temp = 0;
	uint8_t field_index = 0;
	// Search key word in CURL data.
	data_ptr = strstr(orts_ctx.curl_data, ORTS_HUD7_SPEED_DATA_KEYWORD);
	// Check result.
	if (data_ptr == NULL) {
		status = ORTS_ERROR_DATA_PARSING;
		goto errors;
	}
	// Parsing.
	data_ptr = strtok(data_ptr, separators);
	// Fields loop.
	while (data_ptr != NULL) {
		// Check if speed unit is present.
		if (strstr(data_ptr, ORTS_HUD7_SPEED_UNIT) != NULL) {
			// Convert string to value.
			sscanf_count = sscanf(data_ptr, "%d", &temp);
			// Check result.
			if (sscanf_count == 0) {
				status = ORTS_ERROR_DATA_PARSING;
				goto errors;
			}
			// Update data.
			orts_ctx.data[field_index] = (uint8_t) temp;
			field_index++;
			// Check index.
			if (field_index >= ORTS_DATA_INDEX_LAST) {
				break;
			}
		}
		// Go to next field.
		data_ptr = strtok (NULL, separators);
	}
errors:
	return status;
}

/*** ORTS functions ***/

/*******************************************************************/
ORTS_status_t ORTS_init(void) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	// Init context.
	orts_ctx.curl_data_index = 0;
	orts_ctx.request_next_time = 0;
	// Init CURL library.
	orts_ctx.curl = curl_easy_init();
	// Check returned object.
	if (orts_ctx.curl == NULL) {
		status = ORTS_ERROR_CURL_INIT;
		goto errors;
	}
errors:
#ifdef LOG_ORTS
	LOG_STATUS(status, ORTS_SUCCESS, "OK");
#endif
	return status;
}

/*******************************************************************/
ORTS_status_t ORTS_process(void) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	LSMCU_status_t lsmcu_status = LSMCU_SUCCESS;
	FPB_status_t fpb_status = FPB_SUCCESS;
	TRACK_status_t track_status = TRACK_SUCCESS;
	CURLcode curl_status;
	uint32_t idx = 0;
	// Check period.
	if (TIME_get_milliseconds() >= orts_ctx.request_next_time) {
		// Update next time.
		orts_ctx.request_next_time = TIME_get_milliseconds() + ORTS_CURL_REQUEST_PERIOD_MS;
		// Reset data.
		for (idx=0 ; idx<ORTS_CURL_BUFFER_SIZE ; idx++) {
			orts_ctx.curl_data[idx] = '\0';
		}
		orts_ctx.curl_data_index = 0;
		// Check CURL object.
		if ((orts_ctx.curl) != NULL) {
			// Configure request.
			curl_easy_setopt(orts_ctx.curl, CURLOPT_URL, ORTS_SERVER_ADDRESS);
			curl_easy_setopt(orts_ctx.curl, CURLOPT_TIMEOUT, ORTS_REQUEST_TIMEOUT_S);
			curl_easy_setopt(orts_ctx.curl, CURLOPT_WRITEFUNCTION, _ORTS_write_api_data);
			// Perform request.
			curl_status = curl_easy_perform(orts_ctx.curl);
			CURL_stack_exit_error(ORTS_ERROR_DRIVER_CURL);
			// Update data.
			status = _ORTS_update_data();
			if (status != ORTS_SUCCESS) goto errors;
			// Send data to LSMCU.
			lsmcu_status = LSMCU_send(LSMCU_TCH_SPEED_OFFSET + orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH]);
			LSMCU_stack_exit_error(ORTS_ERROR_DRIVER_LSMCU);
			lsmcu_status = LSMCU_send(LSMCU_SPEED_LIMIT_OFFSET + (orts_ctx.data[ORTS_DATA_INDEX_SPEED_LIMIT_KMH] / LSAGIU_SPEED_LIMIT_FACTOR));
			LSMCU_stack_exit_error(ORTS_ERROR_DRIVER_LSMCU);
			// Send data to other modules.
			track_status = TRACK_set_speed(orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH]);
			TRACK_stack_exit_error(ORTS_ERROR_DRIVER_TRACK);
			fpb_status = FPB_set_speed(orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH]);
			FPB_stack_exit_error(ORTS_ERROR_DRIVER_TRACK);
		}
	}
errors:
#ifdef LOG_ORTS
	LOG_STATUS(status, ORTS_SUCCESS, "speed=%dkm/h speed_limit=%dkm/h", orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH], orts_ctx.data[ORTS_DATA_INDEX_SPEED_LIMIT_KMH]);
#endif
	return status;
}
