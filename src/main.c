/*
 * main.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "bpgd.h"
#include "bpsa.h"
#include "compressor.h"
#include "emergency.h"
#include "error.h"
#include "fd.h"
#include "fpb.h"
#include "keyboard.h"
#include "kvb.h"
#include "light.h"
#include "log.h"
#include "mp.h"
#include "mpinv.h"
#include "orts.h"
#include "pbl2.h"
#include "scu.h"
#include "sound.h"
#include "stdint.h"
#include "time.h"
#include "track.h"
#include "version.h"
#include "whistle.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** MAIN macros ***/

#define SGDU_SCU_COM_PORT						"COM6"
#define SGDU_INTERFACES_POLLING_PERIOD_MS		1000
#define SGDU_ERROR_STACK_CHECK_PERIOD_MS		10000

/*** MAIN structures ***/

/*******************************************************************/
typedef enum {
	SGDU_STATE_INIT = 0,
	SGDU_STATE_WAIT_INTERFACES,
	SGDU_STATE_RUNNING,
	SGDU_STATE_LAST
} SGDU_state_t;

/*******************************************************************/
typedef struct {
	SGDU_state_t state;
	uint8_t scu_connected;
	uint8_t orts_server_connected;
	uint32_t interfaces_polling_next_time;
#ifdef LOG_ERROR_STACK
	uint32_t error_stack_check_next_time;
#endif
} SGDU_context_t;

/*** MAIN global variables ***/

static SGDU_context_t sgdu_ctx;

/*** MAIN local functions ***/

/*******************************************************************/
static void _SGDU_print_versions(void) {
	// Local variables.
	uint16_t product = 0;
	uint8_t major = 0;
	uint8_t minor = 0;
	uint8_t patch = 0;
	LOG_color_t log_color = LOG_COLOR_WHITE;
	// Print program version.
	log_color = (GIT_DIRTY_FLAG != 0) ? LOG_COLOR_YELLOW : LOG_COLOR_GREEN;
	LOG_print(LOG_COLOR_WHITE, "Program version:\n");
	LOG_print(LOG_COLOR_WHITE, "sgdu: ");
	LOG_print(log_color, "sw%d.%d.%d", GIT_MAJOR_VERSION, GIT_MINOR_VERSION, GIT_COMMIT_INDEX);
	if (GIT_DIRTY_FLAG != 0) {
		LOG_print(log_color, "-d ");
		LOG_print(LOG_COLOR_RED, "(DIRTY)");
	}
	LOG_print(LOG_COLOR_WHITE, "\n\n");
	// Print libraries version.
	LOG_print(LOG_COLOR_WHITE, "Libraries version:\n");
	LOG_print(LOG_COLOR_WHITE, "fmod: ");
	SOUND_get_fmod_version(&product, &major, &minor);
	LOG_print(LOG_COLOR_GREEN, "v%d.%d.%d\n", product, major, minor);
	LOG_print(LOG_COLOR_WHITE, "curl: ");
	ORTS_get_curl_version(&major, &minor, &patch);
	LOG_print(LOG_COLOR_GREEN, "v%d.%d.%d\n", major, minor, patch);
	LOG_print(LOG_COLOR_WHITE, "cjson: ");
	ORTS_get_cjson_version(&major, &minor, &patch);
	LOG_print(LOG_COLOR_GREEN, "v%d.%d.%d\n\n", major, minor, patch);
}

#ifdef LOG_ERROR_STACK
/*******************************************************************/
static void _SGDU_print_error_stack(void) {
	// Local variables.
	ERROR_code_t error_code = ERROR_BASE_NONE;
	uint32_t count = 0;
	// Check period.
	if (TIME_get_milliseconds() >= sgdu_ctx.error_stack_check_next_time) {
		// Update next time.
		sgdu_ctx.error_stack_check_next_time = TIME_get_milliseconds() + SGDU_ERROR_STACK_CHECK_PERIOD_MS;
		// Check if empty.
		if (ERROR_stack_is_empty() == 0) {
			// Print stack.
			LOG_trace(LOG_COLOR_WHITE, "");
			LOG_print(LOG_COLOR_WHITE, "[ ");
			do {
				// Unstack error.
				error_code = ERROR_stack_read();
				LOG_print(LOG_COLOR_YELLOW, "0x%08X ", error_code);
				// Manage screen width.
				count++;
				if (count >= 10) {
					// Change line.
					LOG_print(LOG_COLOR_WHITE, "]\n[ ");
					count = 0;
				}
			} while (error_code != ERROR_BASE_NONE);
			LOG_print(LOG_COLOR_WHITE, "]\n");
		}
	}
}
#endif

/*** MAIN function ***/

/*******************************************************************/
int main(void) {
	// Start print.
	LOG_print(LOG_COLOR_WHITE, "**************************************************************\n");
	LOG_print(LOG_COLOR_WHITE, "*** Simulateur BB 8500 - Sound and Game Driver Unit (SGDU) ***\n");
	LOG_print(LOG_COLOR_WHITE, "**************************************************************\n\n");
	// Local variables.
	BPGD_status_t bpgd_status = BPGD_SUCCESS;
	BPSA_status_t bpsa_status = BPSA_SUCCESS;
	COMPRESSOR_status_t compressor_status = COMPRESSOR_SUCCESS;
	EMERGENCY_status_t emergency_status = EMERGENCY_SUCCESS;
	FD_status_t fd_status = FD_SUCCESS;
	FPB_status_t fpb_status = FPB_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	KVB_status_t kvb_status = KVB_SUCCESS;
	LIGHT_status_t light_status = LIGHT_SUCCESS;
	SCU_status_t scu_status = SCU_SUCCESS;
	MP_status_t mp_status = MP_SUCCESS;
	MPINV_status_t mpinv_status = MPINV_SUCCESS;
	ORTS_status_t orts_status = ORTS_SUCCESS;
	PBL2_status_t pbl2_status = PBL2_SUCCESS;
	SOUND_status_t sound_status = SOUND_SUCCESS;
	TRACK_status_t track_status = TRACK_SUCCESS;
	WHISTLE_status_t whistle_status = WHISTLE_SUCCESS;
	ZDJ_status_t zdj_status = ZDJ_SUCCESS;
	ZPT_status_t zpt_status = ZPT_SUCCESS;
	ZVM_status_t zvm_status = ZVM_SUCCESS;
	// Print versions.
	_SGDU_print_versions();
	// Init context.
	sgdu_ctx.state = SGDU_STATE_INIT;
	sgdu_ctx.scu_connected = 0;
	sgdu_ctx.orts_server_connected = 0;
	sgdu_ctx.interfaces_polling_next_time = 0;
#ifdef LOG_ERROR_STACK
	sgdu_ctx.error_stack_check_next_time = 0;
#endif
	LOG_trace(LOG_COLOR_GREEN, "state = SGDU_STATE_INIT");
	// Main loop.
	while (1) {
		// Perform state machine.
		switch (sgdu_ctx.state) {
		case SGDU_STATE_INIT:
			// Init peripherals.
			keyboard_status = KEYBOARD_init();
			KEYBOARD_stack_error();
			sound_status = SOUND_init_fmod_system();
			SOUND_stack_error();
			TIME_init();
			// Init modules.
			bpgd_status = BPGD_init();
			BPGD_stack_error();
			bpsa_status = BPSA_init();
			BPSA_stack_error();
			compressor_status = COMPRESSOR_init();
			COMPRESSOR_stack_error();
			emergency_status = EMERGENCY_init();
			EMERGENCY_stack_error();
			fd_status = FD_init();
			FD_stack_error();
			fpb_status = FPB_init();
			FPB_stack_error();
			kvb_status = KVB_init();
			KVB_stack_error();
			light_status = LIGHT_init();
			LIGHT_stack_error();
			mp_status = MP_init();
			MP_stack_error();
			mpinv_status = MPINV_init();
			MPINV_stack_error();
			pbl2_status = PBL2_init();
			PBL2_stack_error();
			track_status = TRACK_init();
			TRACK_stack_error();
			whistle_status = WHISTLE_init();
			WHISTLE_stack_error();
			zdj_status = ZDJ_init();
			ZDJ_stack_error();
			zpt_status = ZPT_init();
			ZPT_stack_error();
			zvm_status = ZVM_init();
			ZVM_stack_error();
#ifdef LOG_ERROR_STACK
			_SGDU_print_error_stack();
#endif
			// Compute next state.
			sgdu_ctx.state = SGDU_STATE_WAIT_INTERFACES;
			LOG_trace(LOG_COLOR_YELLOW, "state = SGDU_STATE_WAIT_INTERFACES");
			break;
		case SGDU_STATE_WAIT_INTERFACES:
			// Check period.
			if (TIME_get_milliseconds() >= sgdu_ctx.interfaces_polling_next_time) {
				// Update next time.
				sgdu_ctx.interfaces_polling_next_time = TIME_get_milliseconds() + SGDU_INTERFACES_POLLING_PERIOD_MS;
				// Open SCU interface.
				if (sgdu_ctx.scu_connected == 0) {
					// Open serial port.
					scu_status = SCU_init(SGDU_SCU_COM_PORT);
					SCU_stack_error();
					// Update flag.
					sgdu_ctx.scu_connected = (scu_status == SCU_SUCCESS) ? 1 : 0;
				}
				// Open ORTS server.
				if (sgdu_ctx.orts_server_connected == 0) {
					// Open server.
					orts_status = ORTS_init();
					ORTS_stack_error();
					// Update flag.
					sgdu_ctx.orts_server_connected = (orts_status == ORTS_SUCCESS) ? 1 : 0;
				}
#ifdef LOG_ERROR_STACK
				_SGDU_print_error_stack();
#endif
			}
			// Start program if all interface have been properly initialized.
			if ((sgdu_ctx.scu_connected != 0) && (sgdu_ctx.orts_server_connected != 0)) {
				// Update state.
				sgdu_ctx.state = SGDU_STATE_RUNNING;
				LOG_trace(LOG_COLOR_GREEN, "state = SGDU_STATE_RUNNING");
			}
			break;
		case SGDU_STATE_RUNNING:
			// Process interfaces.
			orts_status = ORTS_process();
			ORTS_stack_error();
			scu_status = SCU_process();
			SCU_stack_error();
			// Process modules.
			bpgd_status = BPGD_process();
			BPGD_stack_error();
			compressor_status = COMPRESSOR_process();
			COMPRESSOR_stack_error();
			fpb_status = FPB_process();
			FPB_stack_error();
			fd_status = FD_process();
			FD_stack_error();
			mp_status = MP_process();
			MP_stack_error();
			pbl2_status = PBL2_process();
			PBL2_stack_error();
			track_status = TRACK_process();
			TRACK_stack_error();
			whistle_status = WHISTLE_process();
			WHISTLE_stack_error();
			zvm_status = ZVM_process();
			ZVM_stack_error();
			// Process peripherals.
			keyboard_status = KEYBOARD_process();
			KEYBOARD_stack_error();
#ifdef LOG_ERROR_STACK
			_SGDU_print_error_stack();
#endif
			break;
		default:
			goto errors;
		}
	}
errors:
	LOG_print(LOG_COLOR_WHITE, "*******************************************************************\n");
	return 0;
}
