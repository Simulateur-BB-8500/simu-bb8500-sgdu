/*
 * main.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "bpgd.h"
#include "compressor.h"
#include "emergency.h"
#include "error.h"
#include "fd.h"
#include "fpb.h"
#include "keyboard.h"
#include "kvb.h"
#include "light.h"
#include "log.h"
#include "lsmcu.h"
#include "mp.h"
#include "mpinv.h"
#include "orts.h"
#include "pbl2.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"
#include "track.h"
#include "whistle.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** MAIN macros ***/

#define LSAGIU_LSMCU_COM_PORT					"COM6"
#define LSAGIU_INTERFACES_POLLING_PERIOD_MS		1000
#define LSAGIU_ERROR_STACK_CHECK_PERIOD_MS		10000

/*** MAIN structures ***/

/*******************************************************************/
typedef enum {
	LSAGIU_STATE_INIT = 0,
	LSAGIU_STATE_WAIT_INTERFACES,
	LSAGIU_STATE_RUNNING,
	LSAGIU_STATE_LAST
} LSAGIU_state_t;

/*******************************************************************/
typedef struct {
	LSAGIU_state_t state;
	uint8_t lsmcu_connected;
	uint8_t orts_server_connected;
	uint64_t interfaces_polling_next_time;
#ifdef LOG_ERROR_STACK
	uint64_t error_stack_check_next_time;
#endif
} LSAGIU_context_t;

/*** MAIN global variables ***/

static LSAGIU_context_t lsagiu_ctx;

/*** MAIN local functions ***/

#ifdef LOG_ERROR_STACK
/*******************************************************************/
static void _LSAGIU_print_error_stack(void) {
	// Local variables.
	ERROR_code_t error_code = ERROR_BASE_NONE;
	uint32_t count = 0;
	// Print stack.
	TIME_print();
	printf("LSAGIU_print_error_stack() *** [ ");
	// Check if empty.
	if (ERROR_stack_is_empty() == 0) {
		do {
			// Unstack error.
			error_code = ERROR_stack_read();
			printf("0x%08X ", error_code);
			// Manage screen width.
			count++;
			if (count >= 10) {
				// Change line.
				printf("]\n");
				TIME_print();
				printf("                               [ ");
				count = 0;
			}
		} while (error_code != ERROR_BASE_NONE);
	}
	else {
		printf("empty ");
	}
	printf("]\n");
	fflush(stdout);
}
#endif

/*** MAIN function ***/

/*******************************************************************/
int main (void) {
	// Start print.
	printf("********************************************************************\n");
	printf("*** Locomotive Simulator Audio and Game Interface Unit (LS-AGIU) ***\n");
	printf("********************************************************************\n\n");
	fflush(stdout);
	// Local variables.
	BPGD_status_t bpgd_status = BPGD_SUCCESS;
	COMPRESSOR_status_t compressor_status = COMPRESSOR_SUCCESS;
	EMERGENCY_status_t emergency_status = EMERGENCY_SUCCESS;
	FD_status_t fd_status = FD_SUCCESS;
	FPB_status_t fpb_status = FPB_SUCCESS;
	KEYBOARD_status_t keyboard_status = KEYBOARD_SUCCESS;
	KVB_status_t kvb_status = KVB_SUCCESS;
	LIGHT_status_t light_status = LIGHT_SUCCESS;
	LSMCU_status_t lsmcu_status = LSMCU_SUCCESS;
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
	// Init context.
	lsagiu_ctx.state = LSAGIU_STATE_INIT;
	lsagiu_ctx.lsmcu_connected = 0;
	lsagiu_ctx.orts_server_connected = 0;
	lsagiu_ctx.interfaces_polling_next_time = 0;
#ifdef LOG_ERROR_STACK
	lsagiu_ctx.error_stack_check_next_time = 0;
#endif
	// Main loop.
	while (1) {
		// Perform state machine.
		switch (lsagiu_ctx.state) {
		case LSAGIU_STATE_INIT:
			// Init peripherals.
			keyboard_status = KEYBOARD_init();
			KEYBOARD_stack_error();
			sound_status = SOUND_init_fmod_system();
			SOUND_stack_error();
			TIME_init();
			// Init modules.
			bpgd_status = BPGD_init();
			BPGD_stack_error();
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
			_LSAGIU_print_error_stack();
#endif
			// Compute next state.
			lsagiu_ctx.state = LSAGIU_STATE_WAIT_INTERFACES;
			printf("*******************************************************************\n");
			fflush(stdout);
			break;
		case LSAGIU_STATE_WAIT_INTERFACES:
			// Check period.
			if (TIME_get_milliseconds() >= lsagiu_ctx.interfaces_polling_next_time) {
				// Update next time.
				lsagiu_ctx.interfaces_polling_next_time = TIME_get_milliseconds() + LSAGIU_INTERFACES_POLLING_PERIOD_MS;
				// Open LSMCU interface.
				if (lsagiu_ctx.lsmcu_connected == 0) {
					// Open serial port.
					lsmcu_status = LSMCU_init(LSAGIU_LSMCU_COM_PORT);
					LSMCU_stack_error();
					// Update flag.
					lsagiu_ctx.lsmcu_connected = (lsmcu_status == LSMCU_SUCCESS) ? 1 : 0;
				}
				// Open ORTS server.
				if (lsagiu_ctx.orts_server_connected == 0) {
					// Open server.
					orts_status = ORTS_init();
					ORTS_stack_error();
					// Update flag.
					lsagiu_ctx.orts_server_connected = (orts_status == ORTS_SUCCESS) ? 1 : 0;
				}
#ifdef LOG_ERROR_STACK
				_LSAGIU_print_error_stack();
#endif
			}
			// Start program if all interface have been properly initialized.
			if ((lsagiu_ctx.lsmcu_connected != 0) && (lsagiu_ctx.orts_server_connected != 0)) {
				// Update state.
				lsagiu_ctx.state = LSAGIU_STATE_RUNNING;
				printf("*******************************************************************\n");
			}
			fflush(stdout);
			break;
		case LSAGIU_STATE_RUNNING:
			// Process interfaces.
			orts_status = ORTS_process();
			ORTS_stack_error();
			lsmcu_status = LSMCU_process();
			LSMCU_stack_error();
			// Process modules.
			bpgd_status = BPGD_process();
			BPGD_stack_error();
			compressor_status = COMPRESSOR_process();
			COMPRESSOR_stack_error();
			emergency_status = EMERGENCY_process();
			EMERGENCY_stack_error();
			fpb_status = FPB_process();
			FPB_stack_error();
			fd_status = FD_process();
			FD_stack_error();
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
			// Check error stack period.
			if (TIME_get_milliseconds() >= lsagiu_ctx.error_stack_check_next_time) {
				// Update next time.
				lsagiu_ctx.error_stack_check_next_time = TIME_get_milliseconds() + LSAGIU_ERROR_STACK_CHECK_PERIOD_MS;
				_LSAGIU_print_error_stack();
			}
#endif
			fflush(stdout);
			break;
		default:
			LOG("state=UNKNOWN");
			goto errors;
		}
	}
errors:
	printf("*******************************************************************\n");
	fflush(stdout);
	return 0;
}
