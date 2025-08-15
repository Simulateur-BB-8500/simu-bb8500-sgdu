/*
 * scu.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "scu.h"

#include "bpgd.h"
#include "bpsa.h"
#include "compressor.h"
#include "emergency.h"
#include "error.h"
#include "fd.h"
#include "fpb.h"
#include "kvb.h"
#include "light.h"
#include "log.h"
#include "mp.h"
#include "mpinv.h"
#include "pbl2.h"
#include "serial.h"
#include "sgdu.h"
#include "stdint.h"
#include "tch.h"
#include "time.h"
#include "whistle.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** SCU local global variables ***/

static SERIAL_port_t scu_serial_port;

/*** SCU functions ***/

/*******************************************************************/
SCU_status_t SCU_init(char* port) {
	// Local variables.
	SCU_status_t status = SCU_SUCCESS;
	SERIAL_status_t serial_status = SERIAL_SUCCESS;
	// Check parameter.
	if (port == NULL) {
		status = SCU_ERROR_NULL_PARAMETER;
		goto errors;
	}
	// Open serial port.
	serial_status = SERIAL_open(&scu_serial_port, port);
	SERIAL_stack_exit_error(SCU_ERROR_DRIVER_SERIAL);
#ifdef LOG_SCU
	LOG_trace(LOG_COLOR_WHITE, "Port %s opened", port);
#endif
errors:
	LOG_ERROR(status, SCU_SUCCESS);
	return status;
}

/*******************************************************************/
SCU_status_t SCU_send(uint8_t tx_command) {
	// Local variables.
	SCU_status_t status = SCU_SUCCESS;
	SERIAL_status_t serial_status = SERIAL_SUCCESS;
#ifdef LOG_SCU
	LOG_trace(LOG_COLOR_WHITE, "tx_command=%d", tx_command);
#endif
	// Write on serial port.
	serial_status = SERIAL_write(&scu_serial_port, tx_command);
	SERIAL_stack_exit_error(SCU_ERROR_DRIVER_SERIAL);
errors:
	LOG_ERROR(status, SCU_SUCCESS);
	return status;
}

/*******************************************************************/
SCU_status_t SCU_process(void) {
	// Local variables.
	SCU_status_t status = SCU_SUCCESS;
	BPGD_status_t bpgd_status = BPGD_SUCCESS;
	BPSA_status_t bpsa_status = BPSA_SUCCESS;
	COMPRESSOR_status_t compressor_status = COMPRESSOR_SUCCESS;
	EMERGENCY_status_t emergency_status = EMERGENCY_SUCCESS;
	FD_status_t fd_status = FD_SUCCESS;
	FPB_status_t fpb_status = FPB_SUCCESS;
	KVB_status_t kvb_status = KVB_SUCCESS;
	LIGHT_status_t light_status = LIGHT_SUCCESS;
	MP_status_t mp_status = MP_SUCCESS;
	MPINV_status_t mpinv_status = MPINV_SUCCESS;
	PBL2_status_t pbl2_status = PBL2_SUCCESS;
	SERIAL_status_t serial_status = SERIAL_SUCCESS;
	WHISTLE_status_t whistle_status = WHISTLE_SUCCESS;
	ZDJ_status_t zdj_status = ZDJ_SUCCESS;
	ZPT_status_t zpt_status = ZPT_SUCCESS;
	ZVM_status_t zvm_status = ZVM_SUCCESS;
	uint8_t rx_command = SCU_OUT_NOP;
	// Read serial port.
	serial_status = SERIAL_read(&scu_serial_port, &rx_command);
	SERIAL_stack_exit_error(SCU_ERROR_DRIVER_SERIAL);
#ifdef LOG_SCU
	if (rx_command != SCU_OUT_NOP) {
		LOG_trace(LOG_COLOR_WHITE, "rx_command=0x%02X", rx_command);
	}
#endif
	// Decode incoming command.
	switch (rx_command) {
	case SCU_OUT_ZBA_ON:
		// Nothing to do.
		break;
	case SCU_OUT_ZBA_OFF:
		// Nothing to do.
		break;
	case SCU_OUT_RSEC_ON:
		// Nothing to do.
		break;
	case SCU_OUT_RSEC_OFF:
		// Nothing to do.
		break;
	case SCU_OUT_ZDV_ON:
		kvb_status = KVB_set_state(KVB_STATE_ON);
		KVB_stack_exit_error(SCU_ERROR_DRIVER_KVB);
		break;
	case SCU_OUT_ZDV_OFF:
		kvb_status = KVB_set_state(KVB_STATE_OFF);
		KVB_stack_exit_error(SCU_ERROR_DRIVER_KVB);
		break;
	case SCU_OUT_ZPT_REAR_UP:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_REAR, ZPT_STATE_UP);
		ZPT_stack_exit_error(SCU_ERROR_DRIVER_ZPT);
		break;
	case SCU_OUT_ZPT_REAR_DOWN:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_REAR, ZPT_STATE_DOWN);
		ZPT_stack_exit_error(SCU_ERROR_DRIVER_ZPT);
		break;
	case SCU_OUT_ZPT_FRONT_UP:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_FRONT, ZPT_STATE_UP);
		ZPT_stack_exit_error(SCU_ERROR_DRIVER_ZPT);
		break;
	case SCU_OUT_ZPT_FRONT_DOWN:
		zpt_status = ZPT_set_position(ZPT_PANTOGRAPH_FRONT, ZPT_STATE_DOWN);
		ZPT_stack_exit_error(SCU_ERROR_DRIVER_ZPT);
		break;
	case SCU_OUT_ZDJ_OFF:
		zdj_status = ZDJ_set_state(ZDJ_STATE_OPEN);
		ZDJ_stack_exit_error(SCU_ERROR_DRIVER_ZDJ);
		break;
	case SCU_OUT_ZEN_ON:
		zdj_status = ZDJ_set_state(ZDJ_STATE_LOCK);
		ZDJ_stack_exit_error(SCU_ERROR_DRIVER_ZDJ);
		break;
	case SCU_OUT_ZCA_REGULATION_MIN:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_REQUEST_ZCA_REGULATION_MIN);
		COMPRESSOR_stack_exit_error(SCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case SCU_OUT_ZCA_REGULATION_MAX:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_REQUEST_ZCA_REGULATION_MAX);
		COMPRESSOR_stack_exit_error(SCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case SCU_OUT_ZCD_ON:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_REQUEST_ZCD_ON);
		COMPRESSOR_stack_exit_error(SCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case SCU_OUT_ZCX_OFF:
		compressor_status = COMPRESSOR_set_request(COMPRESSOR_REQUEST_ZCX_OFF);
		COMPRESSOR_stack_exit_error(SCU_ERROR_DRIVER_COMPRESSOR);
		break;
	case SCU_OUT_PBL2_ON:
		pbl2_status = PBL2_set_state(PBL2_STATE_ON);
		PBL2_stack_exit_error(SCU_ERROR_DRIVER_PBL2);
		break;
	case SCU_OUT_PBL2_OFF:
		pbl2_status = PBL2_set_state(PBL2_STATE_OFF);
		PBL2_stack_exit_error(SCU_ERROR_DRIVER_PBL2);
		break;
	case SCU_OUT_BPGD:
		bpgd_status = BPGD_set_state(BPGD_STATE_ON);
		BPGD_stack_exit_error(SCU_ERROR_DRIVER_BPGD);
		break;
	case SCU_OUT_FPB_APPLY:
		fpb_status = FPB_set_state(FPB_STATE_APPLY);
		FPB_stack_exit_error(SCU_ERROR_DRIVER_FPB);
		break;
	case SCU_OUT_FPB_NEUTRAL:
		fpb_status = FPB_set_state(FPB_STATE_NEUTRAL);
		FPB_stack_exit_error(SCU_ERROR_DRIVER_FPB);
		break;
	case SCU_OUT_FPB_RELEASE:
		fpb_status = FPB_set_state(FPB_STATE_RELEASE);
		FPB_stack_exit_error(SCU_ERROR_DRIVER_FPB);
		break;
	case SCU_OUT_ZVM_ON:
		zvm_status = ZVM_set_state(ZVM_STATE_ON);
		ZVM_stack_exit_error(SCU_ERROR_DRIVER_ZVM);
		break;
	case SCU_OUT_ZVM_OFF:
		zvm_status = ZVM_set_state(ZVM_STATE_OFF);
		ZVM_stack_exit_error(SCU_ERROR_DRIVER_ZVM);
		break;
	case SCU_OUT_MPINV_FORWARD:
		mpinv_status = MPINV_set_position(MPINV_POSITION_FORWARD);
		MPINV_stack_exit_error(SCU_ERROR_DRIVER_MPINV);
		break;
	case SCU_OUT_MPINV_NEUTRAL:
		mpinv_status = MPINV_set_position(MPINV_POSITION_NEUTRAL);
		MPINV_stack_exit_error(SCU_ERROR_DRIVER_MPINV);
		break;
	case SCU_OUT_MPINV_BACKWARD:
		mpinv_status = MPINV_set_position(MPINV_POSITION_BACKWARD);
		MPINV_stack_exit_error(SCU_ERROR_DRIVER_MPINV);
		break;
	case SCU_OUT_MP_0:
		mp_status = MP_set_event(MP_EVENT_0);
		MP_stack_exit_error(SCU_ERROR_DRIVER_MP);
		break;
	case SCU_OUT_MP_T_MORE:
		mp_status = MP_set_event(MP_EVENT_T_MORE);
		MP_stack_exit_error(SCU_ERROR_DRIVER_MP);
		break;
	case SCU_OUT_MP_T_LESS:
		mp_status = MP_set_event(MP_EVENT_T_LESS);
		MP_stack_exit_error(SCU_ERROR_DRIVER_MP);
		break;
	case SCU_OUT_MP_P:
		mp_status = MP_set_event(MP_EVENT_P);
		MP_stack_exit_error(SCU_ERROR_DRIVER_MP);
		break;
	case SCU_OUT_MP_F_MORE:
		mp_status = MP_set_event(MP_EVENT_F_MORE);
		MP_stack_exit_error(SCU_ERROR_DRIVER_MP);
		break;
	case SCU_OUT_MP_F_LESS:
		mp_status = MP_set_event(MP_EVENT_F_LESS);
		MP_stack_exit_error(SCU_ERROR_DRIVER_MP);
		break;
	case SCU_OUT_FD_APPLY:
		fd_status = FD_set_state(FD_STATE_APPLY);
		FD_stack_exit_error(SCU_ERROR_DRIVER_FD);
		break;
	case SCU_OUT_FD_NEUTRAL:
		fd_status = FD_set_state(FD_STATE_NEUTRAL);
		FD_stack_exit_error(SCU_ERROR_DRIVER_FD);
		break;
	case SCU_OUT_FD_RELEASE:
		fd_status = FD_set_state(FD_STATE_RELEASE);
		FD_stack_exit_error(SCU_ERROR_DRIVER_FD);
		break;
	case SCU_OUT_WHISTLE_HIGH_TONE:
		whistle_status = WHISTLE_set_state(WHISTLE_STATE_HIGH_TONE);
		WHISTLE_stack_exit_error(SCU_ERROR_DRIVER_WHISTLE);
		break;
	case SCU_OUT_WHISTLE_NEUTRAL:
		whistle_status = WHISTLE_set_state(WHISTLE_STATE_NEUTRAL);
		WHISTLE_stack_exit_error(SCU_ERROR_DRIVER_WHISTLE);
		break;
	case SCU_OUT_WHISTLE_LOW_TONE:
		whistle_status = WHISTLE_set_state(WHISTLE_STATE_LOW_TONE);
		WHISTLE_stack_exit_error(SCU_ERROR_DRIVER_WHISTLE);
		break;
	case SCU_OUT_BPEV_ON:
		// TODO
		break;
	case SCU_OUT_BPEV_OFF:
		// TODO
		break;
	case SCU_OUT_BPSA_ON:
		bpsa_status = BPSA_set_state(BPSA_STATE_ON);
		BPSA_stack_exit_error(SCU_ERROR_DRIVER_BPSA);
		break;
	case SCU_OUT_BPSA_OFF:
		bpsa_status = BPSA_set_state(BPSA_STATE_OFF);
		BPSA_stack_exit_error(SCU_ERROR_DRIVER_BPSA);
		break;
	case SCU_OUT_ZFG_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFG, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZFG_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFG, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZFD_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFD, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZFD_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZFD, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZPR_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZPR, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZPR_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZPR, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZLFRG_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRG, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZLFRG_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRG, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZLFRD_ON:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRD, LIGHT_STATE_ON);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_ZLFRD_OFF:
		light_status = LIGHT_set_state(LIGHT_TYPE_ZLFRD, LIGHT_STATE_OFF);
		LIGHT_stack_exit_error(SCU_ERROR_DRIVER_LIGHT);
		break;
	case SCU_OUT_EMERGENCY_ON:
		EMERGENCY_set_state(EMERGENCY_STATE_ON);
		EMERGENCY_stack_exit_error(SCU_ERROR_DRIVER_EMERGENCY);
		break;
	case SCU_OUT_EMERGENCY_OFF:
		EMERGENCY_set_state(EMERGENCY_STATE_OFF);
		EMERGENCY_stack_exit_error(SCU_ERROR_DRIVER_EMERGENCY);
		break;
	case SCU_OUT_NOP:
		// Nothing to do.
		break;
	default:
		// Unknown command.
		status = SCU_ERROR_UNKNOWN_COMMAND;
		goto errors;
	}
errors:
	LOG_ERROR(status, SCU_SUCCESS);
	return status;
}
