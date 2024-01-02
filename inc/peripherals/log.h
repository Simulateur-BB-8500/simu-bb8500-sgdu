/*
 * log.h
 *
 *  Created on: 20 sep. 2023
 *      Author: Ludo
 */

#ifndef __LOG_H__
#define __LOG_H__

/*** LOG macros ***/

//#define LOG_ENABLE

#ifdef LOG_ENABLE
// Peripherals.
#define LOG_KEYBOARD
#define LOG_SERIAL
#define LOG_SOUND
// Simulator.
#define LOG_BPGD
#define LOG_BPSA
#define LOG_COMPRESSOR
#define LOG_EMERGENCY
#define LOG_FD
#define LOG_FPB
#define LOG_KVB
#define LOG_LIGHT
#define LOG_LSMCU
#define LOG_MP
#define LOG_MPINV
#define LOG_PBL2
#define LOG_TRACK
#define LOG_WHISTLE
#define LOG_ZDJ
#define LOG_ZPT
#define LOG_ZVM
// Game.
#define LOG_ORTS
// Applicative
#define LOG_ERROR_STACK
#endif

/*** LOG functions ***/

/*******************************************************************/
#ifdef LOG_ENABLE
#define LOG(...)					{ TIME_print(); printf("%s() *** ", __FUNCTION__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); }
#endif

/*******************************************************************/
#ifdef LOG_ENABLE
#define LOG_ERROR(status, success) 	{ if (status != success) { LOG("ERROR 0x%04X", status); } }
#else
#define LOG_ERROR(status, success)
#endif

#endif /* __LOG_H__ */

