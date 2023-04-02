/*
 * lsmcu.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "lsmcu.h"

#include "compressor.h"
#include "fd.h"
#include "fpb.h"
#include "kvb.h"
#include "lights.h"
#include "lssgiu.h"
#include "log.h"
#include "mp.h"
#include "mpinv.h"
#include "serial.h"
#include "stdio.h"
#include "whistle.h"
#include "zba.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** LSMCU local macros ***/

#define LSMCU_LOG

/*** LSMCU local global variables ***/

static SERIAL_port_t lsmcu_serial_port;

/*** LSMCU functions ***/

/* INIT LSMCU MANAGER.
 * @param port:	LSMCU port number ("COMxx").
 * @return:		None.
 */
void LSMCU_init(char* port) {
	// Local variables.
	SERIAL_status_t status = SERIAL_ERROR_OPEN;
#ifdef LSMCU_LOG
	printf("LSMCU *** Opening port %s: ", port);
#endif
	// Open serial port.
	status = SERIAL_open(&lsmcu_serial_port, port);
#ifdef LSMCU_LOG
	printf("%s\n", ((status == SERIAL_SUCCESS) ? "OK" : "Error"));
#endif
}

/* SEND A COMMAND TO LSMCU.
 * @param tx_command:	Command to send (see enumeration in lsmcu.h).
 * @return:				None.
 */
void LSMCU_send(unsigned tx_command) {
#ifdef LSMCU_LOG
	printf("LSMCU *** TX command = 0x%02X.\n", tx_command);
#endif
	SERIAL_write(&lsmcu_serial_port, tx_command);
}

/* MAIN TASK OF LSMCU MANAGER.
 * @param:	None.
 * @return:	None.
 */
void LSMCU_task(void) {
	// Read serial port.
	unsigned char rx_command = LSMCU_OUT_NOP;
	SERIAL_status_t rx_success = SERIAL_read(&lsmcu_serial_port, &rx_command);
	if ((rx_success == SERIAL_SUCCESS) && (rx_command != LSMCU_OUT_NOP)) {
#ifdef LSMCU_LOG
		printf("LSMCU *** RX command = 0x%02X.\n", rx_command);
#endif
		// Decode incoming command.
		switch (rx_command) {
		case LSMCU_OUT_ZBA_ON:
			ZBA_turn_on();
			break;
		case LSMCU_OUT_ZBA_OFF:
			ZBA_turn_off();
			break;
		case LSMCU_OUT_ZDV_ON:
			KVB_turn_on();
			LOG_enable();
			break;
		case LSMCU_OUT_ZDV_OFF:
			KVB_turn_off();
			LOG_disable();
			break;
		case LSMCU_OUT_ZPT_BACK_UP:
			ZPT_back_up();
			break;
		case LSMCU_OUT_ZPT_BACK_DOWN:
			ZPT_back_down();
			break;
		case LSMCU_OUT_ZPT_FRONT_UP:
			ZPT_front_up();
			break;
		case LSMCU_OUT_ZPT_FRONT_DOWN:
			ZPT_front_down();
			break;
		case LSMCU_OUT_ZDJ_OFF:
			ZDJ_open();
			break;
		case LSMCU_OUT_ZEN_ON:
			ZDJ_lock();
			break;
		case LSMCU_OUT_COMPRESSOR_AUTO_REG_MIN_ON:
			COMPRESSOR_play_zca_regulation_min();
			break;
		case LSMCU_OUT_COMPRESSOR_AUTO_REG_MAX_ON:
			COMPRESSOR_play_zca_regulation_max();
			break;
		case LSMCU_OUT_COMPRESSOR_DIRECT_ON:
			COMPRESSOR_play_zcd();
			break;
		case LSMCU_OUT_COMPRESSOR_OFF:
			COMPRESSOR_off();
			break;
		case LSMCU_OUT_FPB_ON:
			FPB_on();
			break;
		case LSMCU_OUT_FPB_OFF:
			FPB_off();
			break;
		case LSMCU_OUT_FPB_APPLY:
			FPB_apply();
			break;
		case LSMCU_OUT_FPB_NEUTRAL:
			FPB_neutral();
			break;
		case LSMCU_OUT_FPB_RELEASE:
			FPB_release();
			break;
		case LSMCU_OUT_ZVM_ON:
			ZVM_turn_on();
			break;
		case LSMCU_OUT_ZVM_OFF:
			ZVM_turn_off();
			break;
		case LSMCU_OUT_MPINV_FORWARD:
			MPINV_forward();
			break;
		case LSMCU_OUT_MPINV_NEUTRAL:
			MPINV_neutral();
			break;
		case LSMCU_OUT_MPINV_BACKWARD:
			MPINV_backward();
			break;
		case LSMCU_OUT_MP_0:
			MP_0();
			break;
		case LSMCU_OUT_MP_T_MORE:
			MP_t_more();
			break;
		case LSMCU_OUT_MP_T_LESS:
			MP_t_less();
			break;
		case LSMCU_OUT_FD_APPLY:
			FD_apply();
			break;
		case LSMCU_OUT_FD_NEUTRAL:
			FD_neutral();
			break;
		case LSMCU_OUT_FD_RELEASE:
			FD_release();
			break;
		case LSMCU_OUT_WHISTLE_HIGH_TONE:
			WHISTLE_high_tone();
			break;
		case LSMCU_OUT_WHISTLE_NEUTRAL:
			WHISTLE_neutral();
			break;
		case LSMCU_OUT_WHISTLE_LOW_TONE:
			WHISTLE_low_tone();
			break;
		case LSMCU_OUT_ZFG_ON:
			LIGHTS_zfg_on();
			break;
		case LSMCU_OUT_ZFG_OFF:
			LIGHTS_zfg_off();
			break;
		case LSMCU_OUT_ZFD_ON:
			LIGHTS_zfd_on();
			break;
		case LSMCU_OUT_ZFD_OFF:
			LIGHTS_zfd_off();
			break;
		case LSMCU_OUT_ZPR_ON:
			LIGHTS_zpr_on();
			break;
		case LSMCU_OUT_ZPR_OFF:
			LIGHTS_zpr_off();
			break;
		default:
			// Unknwon command.
			break;
		}
	}
}


