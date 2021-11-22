/*
 * lsmcu.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "lsmcu.h"

#include "comp.h"
#include "fd.h"
#include "fpb.h"
#include "kvb.h"
#include "lights.h"
#include "lssgkcu.h"
#include "log.h"
#include "mp.h"
#include "mpinv.h"
#include "s.h"
#include "serial.h"
#include "stdio.h"
#include "zba.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/*** LSMCU local macros ***/

#define LSMCU_SERIAL_BAUDRATE	9600
#define LSMCU_LOG

/*** LSMCU local global variables ***/

static SERIAL_Port_t lsmcu_serial_port;

/*** LSMCU functions ***/

/* INIT LSMCU MANAGER.
 * @param port:	LSMCU port number ("COMxx").
 * @return:		None.
 */
void LSMCU_Init(char* port) {
	// Open serial port.
	SERIAL_Open(&lsmcu_serial_port, port, LSMCU_SERIAL_BAUDRATE);
}

/* SEND A COMMAND TO LSMCU.
 * @param tx_command:	Command to send (see enumeration in lsmcu.h).
 * @return:				None.
 */
void LSMCU_Send(unsigned tx_command) {
	SERIAL_Write(&lsmcu_serial_port, tx_command);
#ifdef LSMCU_LOG
	printf("LSMCU *** TX command = 0x%x.\n", tx_command);
	fflush(stdout);
#endif
}

/* MAIN TASK OF LSMCU MANAGER.
 * @param:	None.
 * @return:	None.
 */
void LSMCU_Task(void) {
	// Read serial port.
	unsigned char rx_command = LSMCU_OUT_NOP;
	unsigned char rx_success = SERIAL_Read(&lsmcu_serial_port, &rx_command);
	if ((rx_success != 0) && (rx_command != LSMCU_OUT_NOP)) {
#ifdef LSMCU_LOG
		printf("LSMCU *** RX command = 0x%x.\n", rx_command);
		fflush(stdout);
#endif
		// Decode incoming command.
		switch (rx_command) {
		case LSMCU_OUT_ZBA_ON:
			ZBA_TurnOn();
			break;
		case LSMCU_OUT_ZBA_OFF:
			ZBA_TurnOff();
			break;
		case LSMCU_OUT_ZDV_ON:
			KVB_TurnOn();
			LOG_Enable();
			break;
		case LSMCU_OUT_ZDV_OFF:
			KVB_TurnOff();
			LOG_Disable();
			break;
		case LSMCU_OUT_ZPT_BACK_UP:
			ZPT_BackUp();
			break;
		case LSMCU_OUT_ZPT_BACK_DOWN:
			ZPT_BackDown();
			break;
		case LSMCU_OUT_ZPT_FRONT_UP:
			ZPT_FrontUp();
			break;
		case LSMCU_OUT_ZPT_FRONT_DOWN:
			ZPT_FrontDown();
			break;
		case LSMCU_OUT_ZDJ_OFF:
			ZDJ_Open();
			break;
		case LSMCU_OUT_ZEN_ON:
			ZDJ_Lock();
			break;
		case LSMCU_OUT_COMP_AUTO_REG_MIN_ON:
			COMP_PlayAutoRegulationMin();
			break;
		case LSMCU_OUT_COMP_AUTO_REG_MAX_ON:
			COMP_PlayAutoRegulationMax();
			break;
		case LSMCU_OUT_COMP_DIRECT_ON:
			COMP_PlayDirect();
			break;
		case LSMCU_OUT_COMP_OFF:
			COMP_TurnOff();
			break;
		case LSMCU_OUT_FPB_ON:
			FPB_On();
			break;
		case LSMCU_OUT_FPB_OFF:
			FPB_Off();
			break;
		case LSMCU_OUT_FPB_APPLY:
			FPB_Apply();
			break;
		case LSMCU_OUT_FPB_NEUTRAL:
			FPB_Neutral();
			break;
		case LSMCU_OUT_FPB_RELEASE:
			FPB_Release();
			break;
		case LSMCU_OUT_ZVM_ON:
			ZVM_TurnOn();
			break;
		case LSMCU_OUT_ZVM_OFF:
			ZVM_TurnOff();
			break;
		case LSMCU_OUT_MPINV_FORWARD:
			MPINV_Forward();
			break;
		case LSMCU_OUT_MPINV_NEUTRAL:
			MPINV_Neutral();
			break;
		case LSMCU_OUT_MPINV_BACKWARD:
			MPINV_Backward();
			break;
		case LSMCU_OUT_MP_0:
			MP_0();
			break;
		case LSMCU_OUT_MP_T_MORE:
			MP_T_More();
			break;
		case LSMCU_OUT_MP_T_LESS:
			MP_T_Less();
			break;
		case LSMCU_OUT_FD_APPLY:
			FD_Apply();
			break;
		case LSMCU_OUT_FD_NEUTRAL:
			FD_Neutral();
			break;
		case LSMCU_OUT_FD_RELEASE:
			FD_Release();
			break;
		case LSMCU_OUT_S_HIGH_TONE:
			S_HighTone();
			break;
		case LSMCU_OUT_S_NEUTRAL:
			S_Neutral();
			break;
		case LSMCU_OUT_S_LOW_TONE:
			S_LowTone();
			break;
		case LSMCU_OUT_ZFG_ON:
			LIGHTS_ZfgOn();
			break;
		case LSMCU_OUT_ZFG_OFF:
			LIGHTS_ZfgOff();
			break;
		case LSMCU_OUT_ZFD_ON:
			LIGHTS_ZfdOn();
			break;
		case LSMCU_OUT_ZFD_OFF:
			LIGHTS_ZfdOff();
			break;
		case LSMCU_OUT_ZPR_ON:
			LIGHTS_ZprOn();
			break;
		case LSMCU_OUT_ZPR_OFF:
			LIGHTS_ZprOff();
			break;
		default:
			// Unknwon command.
			break;
		}
	}
}


