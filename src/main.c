/*
 * main.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "compressor.h"
#include "fd.h"
#include "fpb.h"
#include "keyboard.h"
#include "kvb.h"
#include "lights.h"
#include "lsmcu.h"
#include "mp.h"
#include "mpinv.h"
#include "orts.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"
#include "whistle.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** MAIN macros ***/

#define LSAGIU_LSMCU_COM_PORT					"COM6"
#define LSAGIU_INTERFACES_POLLING_PERIOD_MS		1000

/*** MAIN structures ***/

typedef enum {
	LSAGIU_STATE_INIT = 0,
	LSAGIU_STATE_WAIT_INTERFACES,
	LSAGIU_STATE_RUNNING,
	LSAGIU_STATE_LAST
} LSAGIU_state_t;

typedef struct {
	LSAGIU_state_t state;
	uint8_t lsmcu_connected;
	uint8_t orts_server_connected;
	uint64_t interfaces_polling_next_time;
} LSAGIU_context_t;

/*** MAIN global variables ***/

static LSAGIU_context_t lsagiu_ctx;

/* MAIN FUNCTION.
 * @param:	None.
 * @return:	None.
 */
int main (void) {
	// Start print.
	printf("********************************************************************\n");
	printf("*** Locomotive Simulator Audio and Game Interface Unit (LS-AGIU) ***\n");
	printf("********************************************************************\n\n");
	fflush(stdout);
	// Local variables.
	LSMCU_status_t lsmcu_status = LSMCU_SUCCESS;
	ORTS_status_t orts_status = ORTS_SUCCESS;
	// Init context.
	lsagiu_ctx.state = LSAGIU_STATE_INIT;
	lsagiu_ctx.lsmcu_connected = 0;
	lsagiu_ctx.orts_server_connected = 0;
	lsagiu_ctx.interfaces_polling_next_time = 0;
	// Main loop.
	while (1) {
		// Perform state machine.
		switch (lsagiu_ctx.state) {
		case LSAGIU_STATE_INIT:
			// Init time.
			TIME_init();
			// Init modules.
			SOUND_fmod_system_init();
			COMPRESSOR_init();
			FD_init();
			FPB_init();
			KEYBOARD_init();
			KVB_init();
			LIGHTS_init();
			MP_init();
			MPINV_init();
			WHISTLE_init();
			ZDJ_init();
			ZPT_init();
			ZVM_init();
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
					lsmcu_status = LSMCU_init(LSAGIU_LSMCU_COM_PORT);
					// Update flag.
					lsagiu_ctx.lsmcu_connected = (lsmcu_status == LSMCU_SUCCESS) ? 1 : 0;
				}
				// Open ORTS server.
				if (lsagiu_ctx.orts_server_connected == 0) {
					orts_status = ORTS_init_server();
					// Update flag.
					lsagiu_ctx.orts_server_connected = (orts_status == ORTS_SUCCESS) ? 1 : 0;
				}
			}
			// Compute next state.
			if ((lsagiu_ctx.lsmcu_connected != 0) && (lsagiu_ctx.orts_server_connected != 0)) {
				lsagiu_ctx.state = LSAGIU_STATE_RUNNING;
				printf("*******************************************************************\n");
			}
			fflush(stdout);
			break;
		case LSAGIU_STATE_RUNNING:
			ORTS_task();
			COMPRESSOR_task();
			FPB_task();
			FD_task();
			LIGHTS_task();
			LSMCU_task();
			KEYBOARD_task();
			ZVM_task();
			fflush(stdout);
			break;
		default:
			goto errors;
			break;
		}
	}
errors:
	printf("*******************************************************************\n");
	fflush(stdout);
	return 0;
}
