/*
 * orts.c
 *
 *  Created on: 4 apr. 2023
 *      Author: Ludo
 */

#include "orts.h"

#include "cJSON.h"
#include "curl/curl.h"
#include "error.h"
#include "fpb.h"
#include "log.h"
#include "lsagiu.h"
#include "lsmcu.h"
#include "mp.h"
#include "stdint.h"
#include "string.h"
#include "time.h"
#include "track.h"

/*** ORTS macros ***/

#define ORTS_SERVER_ADDRESS				"http://localhost:2150/API/HUD/7"

#define ORTS_CURL_BUFFER_SIZE			16384

#define ORTS_API_REQUEST_PERIOD_MS		500
#define ORTS_API_REQUEST_TIMEOUT_S		1

#define ORTS_API_UNIT_KMH				"km/h"
#define ORTS_API_UNIT_PERCENT			"%"

/*** ORTS local structures ***/

/*******************************************************************/
typedef enum {
	ORTS_API_TABLE_INDEX_COMMON = 0,
	ORTS_API_TABLE_INDEX_EXTRA,
	ORTS_API_TABLE_INDEX_LAST
} ORTS_api_table_index_t;

/*******************************************************************/
typedef enum {
	ORTS_API_SAMPLE_INDEX_DRIVE_PERCENT = 17,
	ORTS_API_SAMPLE_INDEX_DYNAMIC_BRAKE_PERCENT = 26,
	ORTS_API_SAMPLE_INDEX_SPEED_KMH = 41,
	ORTS_API_SAMPLE_INDEX_SPEED_LIMIT_KMH = 42,
} ORTS_api_sample_index_t;

/*******************************************************************/
typedef enum {
	ORTS_DATA_INDEX_SPEED_KMH = 0,
	ORTS_DATA_INDEX_SPEED_LIMIT_KMH,
	ORTS_DATA_INDEX_DRIVE_PERCENT,
	ORTS_DATA_INDEX_DYNAMIC_BRAKE_PERCENT,
	ORTS_DATA_INDEX_LAST
} ORTS_data_index_t;

/*******************************************************************/
typedef enum {
	ORTS_TX_DATA_INDEX_SPEED_KMH = ORTS_DATA_INDEX_SPEED_KMH,
	ORTS_TX_DATA_INDEX_SPEED_LIMIT_KMH = ORTS_DATA_INDEX_SPEED_LIMIT_KMH,
	ORTS_TX_DATA_INDEX_LAST
} ORTS_tx_data_index_t;

/*******************************************************************/
typedef struct {
	CURL* curl;
	char curl_data[ORTS_CURL_BUFFER_SIZE];
	uint32_t curl_data_index;
	uint32_t request_next_time;
	int32_t data[ORTS_DATA_INDEX_LAST];
	ORTS_tx_data_index_t tx_data_index;
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
ORTS_status_t _ORTS_parse_value_unit(char* json_data, char* expected_unit, int32_t* value) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	int32_t sscanf_count = 0;
	int32_t tmp_i32 = 0;
	char* data_ptr = json_data;
	// Check if unit is present.
	if (strstr(data_ptr, expected_unit) == NULL) {
		status = ORTS_ERROR_UNIT_NOT_FOUND;
		goto errors;
	}
	// Convert string to value.
	sscanf_count = sscanf(data_ptr, "%d", &tmp_i32);
	// Check result.
	if (sscanf_count == 0) {
		status = ORTS_ERROR_DATA_PARSING;
		goto errors;
	}
	(*value) = tmp_i32;
errors:
	LOG_ERROR(status, ORTS_SUCCESS);
	return status;
}

/*******************************************************************/
ORTS_status_t _ORTS_parse_api_sample(uint32_t table_index, uint32_t value_index, char* json_data) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	int32_t value = 0;
	// Check table index.
	switch (table_index) {
	case ORTS_API_TABLE_INDEX_COMMON:
		// Check value index.
		switch (value_index) {
		case ORTS_API_SAMPLE_INDEX_DRIVE_PERCENT:
			// Set default error value.
			orts_ctx.data[ORTS_DATA_INDEX_DRIVE_PERCENT] = -1;
			// Parse value.
			status = _ORTS_parse_value_unit(json_data, ORTS_API_UNIT_PERCENT, &value);
			if (status != ORTS_SUCCESS) goto errors;
			// Update local data.
			orts_ctx.data[ORTS_DATA_INDEX_DRIVE_PERCENT] = value;
			break;
		case ORTS_API_SAMPLE_INDEX_DYNAMIC_BRAKE_PERCENT:
			// Set default error value.
			orts_ctx.data[ORTS_DATA_INDEX_DYNAMIC_BRAKE_PERCENT] = -1;
			// Parse value.
			status = _ORTS_parse_value_unit(json_data, ORTS_API_UNIT_PERCENT, &value);
			if (status != ORTS_SUCCESS) goto errors;
			// Update local data.
			orts_ctx.data[ORTS_DATA_INDEX_DYNAMIC_BRAKE_PERCENT] = value;
			break;
		default:
			// Unused field.
			break;
		}
		break;
	case ORTS_API_TABLE_INDEX_EXTRA:
		// Check value index.
		switch (value_index) {
		case ORTS_API_SAMPLE_INDEX_SPEED_KMH:
			// Parse value.
			status = _ORTS_parse_value_unit(json_data, ORTS_API_UNIT_KMH, &value);
			if (status != ORTS_SUCCESS) goto errors;
			// Update local data.
			orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH] = value;
			break;
		case ORTS_API_SAMPLE_INDEX_SPEED_LIMIT_KMH:
			// Parse value.
			status = _ORTS_parse_value_unit(json_data, ORTS_API_UNIT_KMH, &value);
			if (status != ORTS_SUCCESS) goto errors;
			// Update local data.
			orts_ctx.data[ORTS_DATA_INDEX_SPEED_LIMIT_KMH] = value;
			break;
		default:
			// Unused field.
			break;
		}
		break;
	default:
		// Unused table.
		break;
	}
errors:
	LOG_ERROR(status, ORTS_SUCCESS);
	return status;
}

/*******************************************************************/
ORTS_status_t _ORTS_parse_api_data(void) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	cJSON* json = NULL;
	cJSON* n_tables = NULL;
	cJSON* table = NULL;
	cJSON* n_rows = NULL;
	cJSON* n_cols = NULL;
	cJSON* values = NULL;
	cJSON* value = NULL;
	uint32_t number_of_tables = 0;
	uint32_t number_of_rows = 0;
	uint32_t number_of_columns = 0;
	uint32_t table_idx = 0;
	uint32_t value_idx = 0;
	// Parse JSON structure.
	json = cJSON_Parse((char*) orts_ctx.curl_data);
	if (json == NULL) goto errors;
	// nTables field.
	n_tables = (json -> child);
	if (n_tables == NULL) goto errors;
	number_of_tables = (n_tables -> valueint);
	// First table.
	table = (n_tables -> next);
	if (table == NULL) goto errors;
	// Tables loop.
	for (table_idx=0 ; table_idx<number_of_tables ; table_idx++) {
		// nRows field.
		n_rows = (table -> child);
		if (n_rows == NULL) goto errors;
		number_of_rows = (n_rows -> valueint);
		// nCols field.
		n_cols = (n_rows -> next);
		if (n_cols == NULL) goto errors;
		number_of_columns = (n_cols -> valueint);
		// values field.
		values = (n_cols -> next);
		if (values == NULL) goto errors;
		// First value.
		value = (values -> child);
		if (value == NULL) goto errors;
		// Values loop.
		for (value_idx=0 ; value_idx<(number_of_rows * number_of_columns) ; value_idx++) {
			// Parse data.
			_ORTS_parse_api_sample(table_idx, value_idx, (value -> valuestring));
			// Go to next value.
			value = (value -> next);
			if (value == NULL) break;
		}
		// Go to next table.
		table = (table -> next);
		if (table == NULL) goto errors;
	}
errors:
	cJSON_Delete(json);
	LOG_ERROR(status, ORTS_SUCCESS);
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
	orts_ctx.tx_data_index = 0;
	// Init CURL library.
	orts_ctx.curl = curl_easy_init();
	// Check returned object.
	if (orts_ctx.curl == NULL) {
		status = ORTS_ERROR_CURL_INIT;
		goto errors;
	}
errors:
	LOG_ERROR(status, ORTS_SUCCESS);
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
		orts_ctx.request_next_time = TIME_get_milliseconds() + ORTS_API_REQUEST_PERIOD_MS;
		// Reset data.
		for (idx=0 ; idx<ORTS_CURL_BUFFER_SIZE ; idx++) {
			orts_ctx.curl_data[idx] = '\0';
		}
		orts_ctx.curl_data_index = 0;
		// Check CURL object.
		if ((orts_ctx.curl) != NULL) {
			// Configure request.
			curl_easy_setopt(orts_ctx.curl, CURLOPT_URL, ORTS_SERVER_ADDRESS);
			curl_easy_setopt(orts_ctx.curl, CURLOPT_TIMEOUT, ORTS_API_REQUEST_TIMEOUT_S);
			curl_easy_setopt(orts_ctx.curl, CURLOPT_WRITEFUNCTION, _ORTS_write_api_data);
			// Perform request.
			curl_status = curl_easy_perform(orts_ctx.curl);
			CURL_stack_exit_error(ORTS_ERROR_DRIVER_CURL);
			// Update data.
			status = _ORTS_parse_api_data();
			if (status != ORTS_SUCCESS) goto errors;
			// Send data to LSMCU.
			switch (orts_ctx.tx_data_index) {
			case ORTS_TX_DATA_INDEX_SPEED_KMH:
				lsmcu_status = LSMCU_send(LSMCU_TCH_SPEED_OFFSET + orts_ctx.data[ORTS_TX_DATA_INDEX_SPEED_KMH]);
				LSMCU_stack_exit_error(ORTS_ERROR_DRIVER_LSMCU);
				break;
			case ORTS_TX_DATA_INDEX_SPEED_LIMIT_KMH:
				lsmcu_status = LSMCU_send(LSMCU_SPEED_LIMIT_OFFSET + (orts_ctx.data[ORTS_TX_DATA_INDEX_SPEED_LIMIT_KMH] / LSAGIU_SPEED_LIMIT_FACTOR));
				LSMCU_stack_exit_error(ORTS_ERROR_DRIVER_LSMCU);
				break;
			default:
				status = ORTS_ERROR_API_SAMPLE_INDEX;
				goto errors;
			}
			orts_ctx.tx_data_index = (orts_ctx.tx_data_index + 1) % ORTS_TX_DATA_INDEX_LAST;
			// Send data to other modules.
			track_status = TRACK_set_speed(orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH]);
			TRACK_stack_exit_error(ORTS_ERROR_DRIVER_TRACK);
			fpb_status = FPB_set_speed(orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH]);
			FPB_stack_exit_error(ORTS_ERROR_DRIVER_TRACK);
			MP_set_current_position(orts_ctx.data[ORTS_DATA_INDEX_DRIVE_PERCENT], orts_ctx.data[ORTS_DATA_INDEX_DYNAMIC_BRAKE_PERCENT]);
#ifdef LOG_ORTS
			LOG("speed=%dkm/h speed_limit=%dkm/h drive=%d brake=%d", orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH], orts_ctx.data[ORTS_DATA_INDEX_SPEED_LIMIT_KMH], orts_ctx.data[ORTS_DATA_INDEX_DRIVE_PERCENT], orts_ctx.data[ORTS_DATA_INDEX_DYNAMIC_BRAKE_PERCENT]);
#endif
		}
	}
errors:
	LOG_ERROR(status, ORTS_SUCCESS);
	return status;
}
