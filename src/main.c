/*
 * main.c
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#include "comp.h"
#include "fd.h"
#include "fpb.h"
#include "keyboard.h"
#include "kvb.h"
#include "lights.h"
#include "log.h"
#include "lsmcu.h"
#include "mp.h"
#include "mpinv.h"
#include "s.h"
#include "sound.h"
#include "stdio.h"
#include "time.h"
#include "zba.h"
#include "zdj.h"
#include "zpt.h"
#include "zvm.h"

/* MAIN FUNCTION.
 * @param:	None.
 * @return:	None.
 */
int main (void) {
	// Start print.
	printf("*******************************************************************\n");
	printf("*** Locomotive Simulator Sound and Game Interface Unit (LSSGIU) ***\n");
	printf("*******************************************************************\n\n");
	fflush(stdout);
	// Init time.
	TIME_Init();
	// Init log.
	LOG_Init();
	// Init modules.
	SOUND_FmodSystemInit();
	COMP_Init();
	FD_Init();
	FPB_Init();
	KEYBOARD_Init();
	KVB_Init();
	LIGHTS_Init();
	MP_Init();
	MPINV_Init();
	S_Init();
	ZBA_Init();
	ZDJ_Init();
	ZPT_Init();
	ZVM_Init();
	// Init serial link.
#ifdef WINDOWS
	LSMCU_Init("COM5");
#endif
#ifdef LINUX
	LSMCU_Init("USB1");
#endif
	// Main loop.
	while (1) {
		COMP_Task();
		FPB_Task();
		FD_Task();
		LIGHTS_Task();
		LOG_Task();
		LSMCU_Task();
		KEYBOARD_Task();
		KVB_Task();
		ZVM_Task();
	}
	return 0;
}
