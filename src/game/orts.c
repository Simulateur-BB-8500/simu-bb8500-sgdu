/*
 * orts.c
 *
 *  Created on: 4 apr. 2023
 *      Author: Ludo
 */

#include "orts.h"

#include "curl/curl.h"
#include "lsagiu.h"
#include "lsmcu.h"
#include "stdint.h"
#include "string.h"
#include "time.h"

/*** ORTS macros ***/

#define ORTS_SERVER_ADDRESS				"http://localhost:2150/API/HUD/7"
#define ORTS_CURL_BUFFER_SIZE			16384
#define ORTS_STRING_BUFFER_SIZE			32

#define ORTS_HUD7_SPEED_DATA_KEYWORD	"Chemin"
#define ORTS_HUD7_SPEED_UNIT			"km/h"

#define ORTS_CURL_REQUEST_PERIOD_MS		1000
#define ORTS_REQUEST_TIMEOUT_S			1

#define ORTS_LOG

/*** ORTS local structures ***/

typedef enum {
	ORTS_DATA_INDEX_SPEED_KMH = 0,
	ORTS_DATA_INDEX_SPEED_LIMIT_KMH,
	ORTS_DATA_INDEX_LAST
} ORTS_api_sample_t;

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
 * @param:			None.
 * @return status:	Function execution status.
 */
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
		printf("ORTS *** API server request: ");
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
			// Update data.
			status = _ORTS_update_data();
			if (status != ORTS_SUCCESS) goto errors;
#ifdef ORTS_LOG
			printf("ORTS *** API data: speed = %dkm/h, speed limit = %dkm/h\n", orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH], orts_ctx.data[ORTS_DATA_INDEX_SPEED_LIMIT_KMH]);
#endif
			// Send data to LSMCU.
			LSMCU_send(LSMCU_TCH_SPEED_OFFSET + orts_ctx.data[ORTS_DATA_INDEX_SPEED_KMH]);
			LSMCU_send(LSMCU_SPEED_LIMIT_OFFSET + (orts_ctx.data[ORTS_DATA_INDEX_SPEED_LIMIT_KMH] / LSAGIU_SPEED_LIMIT_FACTOR));
		}
	}
errors:
	return status;
}
