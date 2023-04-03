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

void TIME_init(void);
uint64_t TIME_get_ms(void);

#endif /* __TIME_H__ */
