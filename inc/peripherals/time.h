/*
 * time.h
 *
 *  Created on: 30 jul. 2017
 *      Author: Ludo
 */

#ifndef __TIME_H__
#define __TIME_H__

#include "stdint.h"

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
 * \fn void TIME_print(void)
 * \brief Print current time.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void TIME_print(void);

/*!******************************************************************
 * \fn uint32_t TIME_get_milliseconds(void)
 * \brief Get program uptime.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Uptime in milliseconds.
 *******************************************************************/
uint32_t TIME_get_milliseconds(void);

#endif /* __TIME_H__ */
