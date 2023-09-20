/*
 * lsmcu.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "lsmcu.h"

#include "compressor.h"
#include "error.h"
#include "fd.h"
#include "fpb.h"
#include "kvb.h"
#include "light.h"
#include "log.h"
#include "lsagiu.h"
#include "mp.h"
#include "mpinv.h"
#include "serial.h"
#include "stdint.h"
#include "stdio.h"
#include "tch.h"
#include "time.h"
#include "whistle.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** LSMCU local global variables ***/

static SERIAL_port_t lsmcu_serial_port;

/*** LSMCU functions ***/

/*******************************************************************/
LSMCU_status_t LSMCU_init(char* port) {
	// Local variables.
	LSMCU_status_t status = LSMCU_SUCCESS;
	SERIAL_status_t serial_status = SERIAL_SUCCESS;
	// Check parameter.
	if (port == NULL) {
		status = LSMCU_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Open serial port.
	serial_status = SERIAL_open(&lsmcu_serial_port, port);
	SERIAL_stack_exit_error(LSMCU_ERROR_DRIVER_SERIAL);
errors:
#ifdef LOG_LSMCU
	LOG_STATUS(status, LSMCU_SUCCESS, "Port %s opened", port);
#endif
	return status;
}

/*******************************************************************/
LSMCU_status_t LSMCU_send(uint8_t tx_command) {
	// Local variables.
	LSMCU_status_t status = LSMCU_SUCCESS;
	SERIAL_status_t serial_status = SERIAL_SUCCESS;
	// Write on serial port.
	serial_status = SERIAL_write(&lsmcu_serial_port, tx_command);
	SERIAL_stack_exit_error(LSMCU_ERROR_DRIVER_SERIAL);
errors:
#ifdef LOG_LSMCU
	LOG_STATUS(status, LSMCU_SUCCESS, "tx_command=0x%02X", tx_command);
#endif
	return status;
}

/*******************************************************************/
LSMCU_status_t LSMCU_process(void) {
	// Local variables.
	LSMCU_status_t status = LSMCU_SUCCESS;
	SERIAL_status_t serial_status = SERIAL_SUCCESS;
	KVB_status_t kvb_status = KVB_SUCCESS;
	ZPT_status_t zpt_status = ZPT_SUCCESS;
	ZDJ_status_t zdj_status = ZDJ_SUCCESS;
	COMPRESSOR_status_t compressor_status = COMPRESSOR_SUCCESS;
	FPB_status_t fpb_status = FPB_SUCCESS;
	ZVM_status_t zvm_status = ZVM_SUCCESS;
	MPINV_status_t mpinv_status = MPINV_SUCCESS;
	MP_status_t mp_status = MP_SUCCESS;
	FD_status_t fd_status = FD_SUCCESS;
	WHISTLE_status_t whistle_status = WHISTLE_SUCCESS;
	LIGHT_status_t light_status = LIGHT_SUCCESS;
	uint8_t rx_command = LSMCU_OUT_NOP;
	// Read serial port.
	serial_status = SERIAL_read(&lsmcu_serial_port, &rx_command);
	SERIAL_stack_exit_error(LSMCU_ERROR_DRIVER_SERIAL);
	// Decode incoming command.
	switch (rx_command) {
	case LSMCU_OUT_ZBA_ON:
		// TODO
		break;
	case LSMCU_OUT_ZBA_OFF:
		// TODO
		break;
	case LSMCU_OUT_RSEC_ON:
		// TODO
		break;
	case LSMCU_OUT_RSEC_OFF:
		// TODO
		break;
	case LSMCU_OUT_ZDV_ON:
		kvb_status = KVB_set_state(KVB_STATE_ON);
		KVB_stack_exit_error(LSMCU_ERROR_DRIVER_KVB);
		break;
	case LSMCU_OUT_ZDV_OFF:
		kvb_status = KVB_set_state(KVB_STATE_OFF);
		KVB_stack_exit_error(LSMCU_ERROR_DRIVER_KVB);
		break;
	case LSMCU_OUT_ZPT_REAR_UP:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_REAR, ZPT_STATE_UP);
		ZPT_stack_exit_error(LSMCU_ERROR_DRIVER_ZPT);
		break;
	case LSMCU_OUT_ZPT_REAR_DOWN:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_REAR, ZPT_STATE_DOWN);
		ZPT_stack_exit_error(LSMCU_ERROR_DRIVER_ZPT);
		break;
	case LSMCU_OUT_ZPT_FRONT_UP:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_FRONT, ZPT_STATE_UP);
		ZPT_stack_exit_error(LSMCU_ERROR_DRIVER_ZPT);
		break;
	case LSMCU_OUT_ZPT_FRONT_DOWN:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_FRONT, ZPT_STATE_DOWN);
		ZPT_stack_exit_error(LSMCU_ERROR_DRIVER_ZPT);
		break;
	case LSMCU_OUT_ZDJ_OFF:
		zdj_status = ZDJ_set_state(ZDJ_STATE_OPEN);
		ZDJ_stack_exit_error(LSMCU_ERROR_DRIVER_ZDJ);
		break;
	case LSMCU_OUT_ZEN_ON:
		zdj_status = ZDJ_set_state(ZDJ_STATE_LOCK);
		ZDJ_stack_exit_error(LSMCU_ERROR_DRIVER_ZDJ);
		break;
	case LSMCU_OUT_COMPRESSOR_AUTO_REG_MIN_ON:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_SOUND_REQUEST_ZCA_MIN);
		COMPRESSOR_stack_exit_error(LSMCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case LSMCU_OUT_COMPRESSOR_AUTO_REG_MAX_ON:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_SOUND_REQUEST_ZCA_MAX);
		COMPRESSOR_stack_exit_error(LSMCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case LSMCU_OUT_COMPRESSOR_DIRECT_ON:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_SOUND_REQUEST_ZCD);
		COMPRESSOR_stack_exit_error(LSMCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case LSMCU_OUT_COMPRESSOR_OFF:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_SOUND_REQUEST_OFF);
		COMPRESSOR_stack_exit_error(LSMCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case LSMCU_OUT_FPB_ON:
		// TODO
		break;
	case LSMCU_OUT_FPB_OFF:
		// TODO
		break;
	case LSMCU_OUT_FPB_APPLY:
		fpb_status = FPB_set_state(FPB_STATE_APPLY);
		FPB_stack_exit_error(LSMCU_ERROR_DRIVER_FPB);
		break;
	case LSMCU_OUT_FPB_NEUTRAL:
		fpb_status = FPB_set_state(FPB_STATE_NEUTRAL);
		FPB_stack_exit_error(LSMCU_ERROR_DRIVER_FPB);
		break;
	case LSMCU_OUT_FPB_RELEASE:
		fpb_status = FPB_set_state(FPB_STATE_RELEASE);
		FPB_stack_exit_error(LSMCU_ERROR_DRIVER_FPB);
		break;
	case LSMCU_OUT_BPGD:
		// TODO
		break;
	case LSMCU_OUT_ZVM_ON:
		zvm_status = ZVM_set_state(ZVM_STATE_ON);
		ZVM_stack_exit_error(LSMCU_ERROR_DRIVER_ZVM);
		break;
	case LSMCU_OUT_ZVM_OFF:
		zvm_status = ZVM_set_state(ZVM_STATE_OFF);
		ZVM_stack_exit_error(LSMCU_ERROR_DRIVER_ZVM);
		break;
	case LSMCU_OUT_MPINV_FORWARD:
		mpinv_status = MPINV_set_position(MPINV_POSITION_FORWARD);
		MPINV_stack_exit_error(LSMCU_ERROR_DRIVER_MPINV);
		break;
	case LSMCU_OUT_MPINV_NEUTRAL:
		mpinv_status = MPINV_set_position(MPINV_POSITION_NEUTRAL);
		MPINV_stack_exit_error(LSMCU_ERROR_DRIVER_MPINV);
		break;
	case LSMCU_OUT_MPINV_BACKWARD:
		mpinv_status = MPINV_set_position(MPINV_POSITION_BACKWARD);
		MPINV_stack_exit_error(LSMCU_ERROR_DRIVER_MPINV);
		break;
	case LSMCU_OUT_MP_0:
		mp_status = MP_set_event(MP_EVENT_0);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_MP_T_MORE:
		mp_status = MP_set_event(MP_EVENT_T_MORE);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_MP_T_LESS:
		mp_status = MP_set_event(MP_EVENT_T_LESS);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_MP_P:
		mp_status = MP_set_event(MP_EVENT_P);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_MP_F_MORE:
		mp_status = MP_set_event(MP_EVENT_F_MORE);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_MP_F_LESS:
		mp_status = MP_set_event(MP_EVENT_F_LESS);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_MP_FR:
		mp_status = MP_set_event(MP_EVENT_FR);
		MP_stack_exit_error(LSMCU_ERROR_DRIVER_MP);
		break;
	case LSMCU_OUT_FD_APPLY:
		fd_status = FD_set_state(FD_STATE_APPLY);
		FD_stack_exit_error(LSMCU_ERROR_DRIVER_FD);
		break;
	case LSMCU_OUT_FD_NEUTRAL:
		fd_status = FD_set_state(FD_STATE_NEUTRAL);
		FD_stack_exit_error(LSMCU_ERROR_DRIVER_FD);
		break;
	case LSMCU_OUT_FD_RELEASE:
		fd_status = FD_set_state(FD_STATE_RELEASE);
		FD_stack_exit_error(LSMCU_ERROR_DRIVER_FD);
		break;
	case LSMCU_OUT_WHISTLE_HIGH_TONE:
		whistle_status = WHISTLE_set_state(WHISTLE_STATE_HIGH_TONE);
		WHISTLE_stack_exit_error(LSMCU_ERROR_DRIVER_WHISTLE);
		break;
	case LSMCU_OUT_WHISTLE_NEUTRAL:
		whistle_status = WHISTLE_set_state(WHISTLE_STATE_NEUTRAL);
		WHISTLE_stack_exit_error(LSMCU_ERROR_DRIVER_WHISTLE);
		break;
	case LSMCU_OUT_WHISTLE_LOW_TONE:
		whistle_status = WHISTLE_set_state(WHISTLE_STATE_LOW_TONE);
		WHISTLE_stack_exit_error(LSMCU_ERROR_DRIVER_WHISTLE);
		break;
	case LSMCU_OUT_BPEV_ON:
		// TODO
		break;
	case LSMCU_OUT_BPEV_OFF:
		// TODO
		break;
	case LSMCU_OUT_BPSA_ON:
		// TODO
		break;
	case LSMCU_OUT_BPSA_OFF:
		// TODO
		break;
	case LSMCU_OUT_ZFG_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFG, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZFG_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFG, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZFD_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFD, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZFD_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFD, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZPR_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZPR, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZPR_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZPR, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZLFRG_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRG, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZLFRG_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRG, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZLFRD_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRD, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_ZLFRD_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRD, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(LSMCU_ERROR_DRIVER_LIGHT);
		break;
	case LSMCU_OUT_URGENCY:
		// TODO
		break;
	case LSMCU_OUT_NOP:
		// Nothing to do.
		break;
	default:
		// Unknown command.
		status = LSMCU_ERROR_UNKNOWN_COMMAND;
		goto errors;
	}
errors:
#ifdef LOG_LSMCU
	LOG_STATUS(status, LSMCU_SUCCESS, "rx_command=0x%02X", rx_command);
#endif
	return status;
}
