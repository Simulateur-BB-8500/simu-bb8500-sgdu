/*
 * file.h
 *
 *  Created on: 29 mar. 2020
 *      Author: Ludo
 */

#ifndef __FILE_H__
#define __FILE_H__

#include "stdio.h"

/*** FILE macros ***/

#define FILE_LINE_MAX_LENGTH	200

/*** FILE functions ***/

void FILE_open(FILE** file, const char* file_path);
void FILE_get_last_line(FILE* file, char* last_line);
void FILE_close(FILE* file);

#endif /* __FILE_H__ */
