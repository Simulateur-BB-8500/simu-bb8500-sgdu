/*
 * comp.h
 *
 *  Created on: 8 may 2020
 *      Author: Ludo
 */

#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

/*** COMP functions ***/

void COMPRESSOR_init(void);
void COMPRESSOR_play_auto_regulation_min(void);
void COMPRESSOR_play_auto_regulation_max(void);
void COMPRESSOR_play_direct(void);
void COMPRESSOR_turn_off(void);
void COMPRESSOR_task(void);

#endif /* __COMPRESSOR_H__ */
