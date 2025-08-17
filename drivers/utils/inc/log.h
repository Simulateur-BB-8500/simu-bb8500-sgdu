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
#define LOG_SCU
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

/*** LOG structures ***/

/*!******************************************************************
 * \enum LOG_color_t
 * \brief Log message print colors list.
 *******************************************************************/
typedef enum {
    LOG_COLOR_RED = 0,
    LOG_COLOR_GREEN,
    LOG_COLOR_BLUE,
    LOG_COLOR_YELLOW,
    LOG_COLOR_WHITE,
    LOG_COLOR_LAST
} LOG_color_t;

/*** LOG functions ***/

/*******************************************************************/
#define LOG_trace(color, ...) { \
    LOG_print_system_time(); \
    LOG_print(LOG_COLOR_BLUE, "%s() ", __FUNCTION__); \
    LOG_print(color, __VA_ARGS__); \
    LOG_print(color, "\n"); \
}

/*******************************************************************/
#ifdef LOG_ENABLE
#define LOG_ERROR(status, success)  { if (status != success) { LOG_trace(LOG_COLOR_RED, "ERROR 0x%04X", status); } }
#else
#define LOG_ERROR(status, success)
#endif

/*!******************************************************************
 * \fn void LOG_print(LOG_color_t color, const char*, format, ...)
 * \brief Print a message on console.
 * \param[in]   color: Message color.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void LOG_print(LOG_color_t color, const char* format, ...);

/*!******************************************************************
 * \fn void LOG_print_system_time(void)
 * \brief Print system time.
 * \param[in]   none
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void LOG_print_system_time(void);

#endif /* __LOG_H__ */

