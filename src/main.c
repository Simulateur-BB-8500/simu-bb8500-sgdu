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
#include "openrails.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"
#include "whistle.h"
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
	ZBA_init();
	ZDJ_init();
	ZPT_init();
	ZVM_init();
	// Init serial link.
	LSMCU_init("COM6");
	// Main loop.
	while (1) {
		COMPRESSOR_task();
		FPB_task();
		FD_task();
		LIGHTS_task();
		LSMCU_task();
		KEYBOARD_task();
		ZVM_task();
		fflush(stdout);
	}
	return 0;
}
