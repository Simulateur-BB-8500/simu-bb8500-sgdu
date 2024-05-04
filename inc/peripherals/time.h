/*
 * time.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __TIME_H__
#define __TIME_H__

#include "stdint.h"

/*** TIME structures ***/

/*!******************************************************************
 * \enum TIME_system_t
 * \brief System time structure.
 *******************************************************************/
typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint32_t milliseconds;
} TIME_system_t;

/*** TIME functions ***/

/*!******************************************************************
 * \fn void TIME_init(void)
 * \brief Init time base.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void TIME_init(void);

/*!******************************************************************
 * \fn void TIME_get_system_time(TIME_system_t* system_time)
 * \brief Print current time.
 * \param[in]  	none
 * \param[out] 	system_time: Pointer to the system time.
 * \retval		none
 *******************************************************************/
void TIME_get_system_time(TIME_system_t* system_time);

/*!******************************************************************
 * \fn uint32_t TIME_get_milliseconds(void)
 * \brief Get program uptime.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Uptime in milliseconds.
 *******************************************************************/
uint32_t TIME_get_milliseconds(void);

#endif /* __TIME_H__ */
