/*
 * mp.c
 *
 *  Created on: 9 may 2020
 *      Author: Ludo
 */

#include "mp.h"

#include "keyboard.h"
#include "mixer.h"
#include "openrails.h"
#include "sound.h"
#include "stdint.h"
#include "stdio.h"

/*** MP local macros ***/

#define MP_LOG

/*** MP functions ***/

/* INIT THROTTLE MODULE.
 * @param:	None.
 * @return:	None.
 */
void MP_init(void) {
	// TBD.
}

/* SET THROTTLE TO 0.
 * @param:	None.
 * @return:	None.
 */
void MP_0(void) {
	// Send OpenRails shortcut.
	KEYBOARD_send(&OPENRAILS_MP_0, OPENRAILS_PRESS_DURATION_MS_DEFAULT);
#ifdef MP_LOG
	printf("MP *** 0.\n");
	fflush(stdout);
#endif
}

/* ADD ONE GEAR.
 * @param:	None.
 * @return:	None.
 */
void MP_t_more(void) {
	// Send OpenRails shortcut.
	KEYBOARD_send(&OPENRAILS_MP_T_MORE, OPENRAILS_PRESS_DURATION_MS_MP);
#ifdef MP_LOG
	printf("MP *** T More.\n");
	fflush(stdout);
#endif
}

/* REMOVE ONE GEAR.
 * @param:	None.
 * @return:	None.
 */
void MP_t_less(void) {
	// Send OpenRails shortcut.
	KEYBOARD_send(&OPENRAILS_MP_T_LESS, OPENRAILS_PRESS_DURATION_MS_MP);
#ifdef MP_LOG
	printf("MP *** T Less.\n");
	fflush(stdout);
#endif
}
