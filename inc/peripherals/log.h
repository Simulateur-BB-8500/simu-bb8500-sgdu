/*
 * log.h
 *
 *  Created on: 20 sep. 2023
 *      Author: Ludo
 */

#ifndef __LOG_H__
#define __LOG_H__

/*** LOG macros ***/

// Peripherals.
#define LOG_KEYBOARD
#define LOG_SERIAL
#define LOG_SOUND
// Simulator.
#define LOG_COMPRESSOR
#define LOG_FD
#define LOG_FPB
#define LOG_KVB
#define LOG_LIGHT
#define LOG_LSMCU
#define LOG_MP
#define LOG_MPINV
#define LOG_WHISTLE
#define LOG_ZDJ
#define LOG_ZPT
#define LOG_ZVM
// Game.
#define LOG_ORTS

/*** LOG functions ***/

/*******************************************************************/
#define LOG(...)	{ TIME_print(); printf("%s() *** ", __FUNCTION__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); }

/*******************************************************************/
#define LOG_STATUS(status, success, ...) 	{ if (status == success) { LOG(__VA_ARGS__); } else { LOG("ERROR 0x%04X", status); } }

#endif /* __LOG_H__ */

