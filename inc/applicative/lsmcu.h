/*
 * lsmcu.h
 *
 *  Created on: 25 mar. 2020
 *      Author: Ludo
 */

#ifndef LSMCU_H
#define LSMCU_H

/*** LSMCU functions ***/

void LSMCU_Init(char* port);
void LSMCU_Send(unsigned tx_command);
void LSMCU_Task(void);

#endif /* LSMCU_H */
