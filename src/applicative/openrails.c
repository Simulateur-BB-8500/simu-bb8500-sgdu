/*
 * openrails.c
 *
 *  Created on: 4 apr. 2023
 *      Author: Ludo
 */

#include "openrails.h"

#include "curl/curl.h"
#include "stdint.h"
#include "string.h"

/*** OPENRAILS macros ***/

#define OPENRAILS_SERVER_ADDRESS	"http://localhost:2150/API/CABCONTROLS"
#define OPENRAILS_CURL_BUFFER_SIZE	16384
#define OPENRAILS_LOG

/*** OPENRAILS local structures ***/

typedef struct {
	CURL* curl;
	char curl_data[OPENRAILS_CURL_BUFFER_SIZE];
	uint32_t curl_data_index;
} OPENRAILS_context_t;

/*** OPENRAILS local global variables ***/

static OPENRAILS_context_t openrails_ctx;

/*** OPENRAILS local functions ***/

/* CURL WRITE CALLBACK FUNCTION.
 * @param ptr:			Pointer to the data to store.
 * @param size:			Set to 1.
 * @param nmemb:		Number of elements to write.
 * @param user_data:	Pointer to the user data (not used here).
 * @return nmemb:		Number of elements that has been written.
 */
size_t _OPENRAILS_write_api_data(char* ptr, size_t size, size_t nmemb, void* user_data) {
	// Local variables.
	uint32_t idx = 0;
	// Print data.
#ifdef OPENRAILS_LOG
	printf("OPENRAILS *** Writing API data (%d)\n", nmemb);
	fflush(stdout);
#endif
	// Copy data into local buffer.
	for (idx=0 ; idx<nmemb ; idx++) {
		openrails_ctx.curl_data[openrails_ctx.curl_data_index] = ptr[idx];
		openrails_ctx.curl_data_index = (openrails_ctx.curl_data_index + 1) % OPENRAILS_CURL_BUFFER_SIZE;
	}
	return nmemb;
}

/*** OPENRAILS functions ***/

/* INIT OPENRAILS SERVER INTERFACE.
 * @param:			None.
 * @return status:	Function execution status.
 */
OPENRAILS_status_t OPENRAILS_init_server(void) {
	// Local variables.
	OPENRAILS_status_t status = OPENRAILS_SUCCESS;
	// Init CURL library.
#ifdef OPENRAILS_LOG
	printf("OPENRAILS *** Opening server: ");
#endif
	openrails_ctx.curl = curl_easy_init();
	// Check returned object.
	if (openrails_ctx.curl == NULL) {
#ifdef OPENRAILS_LOG
		printf("Error\n");
#endif
		status = OPENRAILS_ERROR_CURL_INIT;
		goto errors;
	}
#ifdef OPENRAILS_LOG
	printf("OK\n");
#endif
errors:
	return status;
}

/* GET OPENRAILS API SAMPLE.
 * @param api_sample:	API sample index.
 * @param value:		Pointer that will contain the value;
 * @return:				None.
 */
OPENRAILS_status_t OPENRAILS_get_api_sample(OPENRAILS_api_sample_t api_sample, uint32_t* value) {
	// Local variables.
	OPENRAILS_status_t status = OPENRAILS_SUCCESS;
	CURLcode curl_status;
	uint32_t idx = 0;
	// Reset data.
	for (idx=0 ; idx<OPENRAILS_CURL_BUFFER_SIZE ; idx++) {
		openrails_ctx.curl_data[idx] = '\0';
	}
	openrails_ctx.curl_data_index = 0;
	// Check CURL object.
	if ((openrails_ctx.curl) != NULL) {
		// Configure request.
		curl_easy_setopt(openrails_ctx.curl, CURLOPT_URL, OPENRAILS_SERVER_ADDRESS);
		curl_easy_setopt(openrails_ctx.curl, CURLOPT_WRITEFUNCTION, _OPENRAILS_write_api_data);
		// Perform request.
		curl_status = curl_easy_perform(openrails_ctx.curl);
		curl_easy_cleanup(openrails_ctx.curl);
		// Check status.
		if (curl_status != CURLE_OK) {
			status = OPENRAILS_ERROR_CURL_REQUEST;
			goto errors;
		}
	}
errors:
	return status;
}
