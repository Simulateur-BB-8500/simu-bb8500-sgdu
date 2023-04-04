/*
 * orts.c
 *
 *  Created on: 4 apr. 2023
 *      Author: Ludo
 */

#include "orts.h"

#include "curl/curl.h"
#include "lsmcu.h"
#include "stdint.h"
#include "string.h"
#include "time.h"

/*** ORTS macros ***/

#define ORTS_SERVER_ADDRESS				"http://localhost:2150/API/CABCONTROLS"
#define ORTS_CURL_BUFFER_SIZE			16384
#define ORTS_STRING_BUFFER_SIZE			32

#define ORTS_CURL_REQUEST_PERIOD_MS		1000
#define ORTS_REQUEST_TIMEOUT_S			1

//#define ORTS_LOG

/*** ORTS local structures ***/

typedef enum {
	ORTS_API_SAMPLE_SPEED = 0,
	ORTS_API_SAMPLE_LAST
} ORTS_api_sample_t;

typedef struct {
	CURL* curl;
	char curl_data[ORTS_CURL_BUFFER_SIZE];
	uint32_t curl_data_index;
	uint64_t request_next_time;
} ORTS_context_t;

/*** ORTS local global variables ***/

static const char* ORTS_JSON_FIELD_API_SAMPLE[ORTS_API_SAMPLE_LAST] = {"SPEEDOMETER"};
static ORTS_context_t orts_ctx;

/*** ORTS local functions ***/

/* CURL WRITE CALLBACK FUNCTION.
 * @param ptr:			Pointer to the data to store.
 * @param size:			Set to 1.
 * @param nmemb:		Number of elements to write.
 * @param user_data:	Pointer to the user data (not used here).
 * @return nmemb:		Number of elements that has been written.
 */
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

/* GET ORTS API SAMPLE.
 * @param api_sample:	API sample index.
 * @param value:		Pointer that will contain the value.
 * @return status:		Function execution status.
 */
ORTS_status_t _ORTS_get_api_sample(ORTS_api_sample_t api_sample, float* value) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	char* api_sample_ptr = NULL;
	char name_str[ORTS_STRING_BUFFER_SIZE];
	char min_value_str[ORTS_STRING_BUFFER_SIZE];
	char max_value_str[ORTS_STRING_BUFFER_SIZE];
	char range_fraction_str[ORTS_STRING_BUFFER_SIZE];
	float min_value = 0.0;
	float max_value = 0.0;
	float range_fraction = 0.0;
	int32_t scan_success_count = 0;
	// Check parameters.
	if (api_sample >= ORTS_API_SAMPLE_LAST) {
		status = ORTS_ERROR_API_SAMPLE;
		goto errors;
	}
	if (value == NULL) {
		status = ORTS_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Search JSON field in CURL data.
	api_sample_ptr = strstr(orts_ctx.curl_data, ORTS_JSON_FIELD_API_SAMPLE[api_sample]);
	if (api_sample_ptr == NULL) {
		status = ORTS_ERROR_API_SAMPLE_NOT_FOUND;
		goto errors;
	}
	// Parse JSON structure.
	scan_success_count = sscanf(api_sample_ptr, "%s %s %f, %s %f, %s %f", name_str, min_value_str, &min_value, max_value_str, &max_value, range_fraction_str, &range_fraction);
	if (scan_success_count != 7) {
		status = ORTS_ERROR_API_SAMPLE_PARSING;
		goto errors;
	}
	// Compute value.
	(*value) = ((max_value - min_value) * range_fraction);
errors:
	return status;
}

/*** ORTS functions ***/

/* INIT ORTS SERVER INTERFACE.
 * @param:			None.
 * @return status:	Function execution status.
 */
ORTS_status_t ORTS_init_server(void) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	// Init context.
	orts_ctx.curl_data_index = 0;
	orts_ctx.request_next_time = 0;
	// Init CURL library.
#ifdef ORTS_LOG
	printf("ORTS *** Opening server: ");
#endif
	orts_ctx.curl = curl_easy_init();
	// Check returned object.
	if (orts_ctx.curl == NULL) {
		status = ORTS_ERROR_CURL_INIT;
		goto errors;
	}
errors:
#ifdef ORTS_LOG
	printf("%s\n", ((status == ORTS_SUCCESS) ? "OK" : "Error"));
#endif
	return status;
}

/* MAIN TASK OF ORTS SERVER INTERFACE
 * @param:			None.
 * @return status:	Function execution status.
 */
ORTS_status_t ORTS_task(void) {
	// Local variables.
	ORTS_status_t status = ORTS_SUCCESS;
	CURLcode curl_status;
	uint32_t idx = 0;
	float value = 0.0;
	// Check period.
	if (TIME_get_milliseconds() >= orts_ctx.request_next_time) {
		// Update next time.
		orts_ctx.request_next_time = TIME_get_milliseconds() + ORTS_CURL_REQUEST_PERIOD_MS;
		// Reset data.
		for (idx=0 ; idx<ORTS_CURL_BUFFER_SIZE ; idx++) {
			orts_ctx.curl_data[idx] = '\0';
		}
		orts_ctx.curl_data_index = 0;
#ifdef ORTS_LOG
		printf("ORTS *** API Server request: ");
#endif
		// Check CURL object.
		if ((orts_ctx.curl) != NULL) {
			// Configure request.
			curl_easy_setopt(orts_ctx.curl, CURLOPT_URL, ORTS_SERVER_ADDRESS);
			curl_easy_setopt(orts_ctx.curl, CURLOPT_TIMEOUT, ORTS_REQUEST_TIMEOUT_S);
			curl_easy_setopt(orts_ctx.curl, CURLOPT_WRITEFUNCTION, _ORTS_write_api_data);
			// Perform request.
			curl_status = curl_easy_perform(orts_ctx.curl);
			// Check status.
			if (curl_status != CURLE_OK) {
#ifdef ORTS_LOG
				printf("Error\n");
#endif
				status = ORTS_ERROR_CURL_REQUEST;
				goto errors;
			}
#ifdef ORTS_LOG
			printf("OK (%d bytes received)\n", (orts_ctx.curl_data_index + 1));
#endif
			// Get speed.
			status = _ORTS_get_api_sample(ORTS_API_SAMPLE_SPEED, &value);
			if (status != ORTS_SUCCESS) goto errors;
			// Send speed to LSMCU.
			LSMCU_send((uint8_t) value);
		}
	}
errors:
	return status;
}
