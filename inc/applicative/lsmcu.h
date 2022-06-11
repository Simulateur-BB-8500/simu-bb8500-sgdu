/*
 * lsmcu.h
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#ifndef __LSMCU_H__
#define __LSMCU_H__

/*** LSMCU functions ***/

void LSMCU_init(char* port);
void LSMCU_send(unsigned tx_command);
void LSMCU_task(void);

#endif /* __LSMCU_H__ */
