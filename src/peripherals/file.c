/*
 * file.c
 *
 *  Created on: 29 mar. 2020
 *      Author: Ludo
 */

#include "file.h"

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"

/*** FILE functions ***/

/* OPEN A FILE FOR READING ONLY.
 * @param file:			Pointer to file.
 * @param file_path:	Abolute or relative path of the file.
 * @return:				None.
 */
void FILE_open(FILE** file, const char* file_path) {
	(*file) = fopen(file_path, "r");
	if ((*file) != NULL) {
		printf("FILE : Open %s: OK.\n", file_path);
	}
	else {
		printf("FILE : Open %s: Error.\n", file_path);
	}
	fflush(stdout);
}

/* GET THE LAST LINE OF A FILE.
 * @param file:			File to analyse.
 * @param last_line:	Pointer to the string that will contain the last line.
 * @return:				None.
 */
void FILE_get_last_line(FILE* file, char* last_line) {
	if (file != NULL) {
		unsigned int char_idx = 0;
		// Move the cursor before the last character of the file.
		fseek(file, -1, SEEK_END);
		char current_char = fgetc(file);
		char_idx++;
		// Skip all ending carriage returns of the last line.
		while ((current_char == '\n') || (current_char == '\r')) {
			fseek(file, -3, SEEK_CUR); // -3='\n'+'\r'+1 because fgetc() made the cursor go forward.
			current_char = fgetc(file);
			char_idx++;
		}
		unsigned int line_length = 0;
		while (current_char != '\n') {
			// Go back character per character.
			fseek(file, -2, SEEK_CUR); // -2 because fgetc() made the cursor go forward.
			current_char = fgetc(file);
			char_idx++;
			line_length++;
		}
		// Create the line.
		if (line_length > 0) {
			fgets(last_line, FILE_LINE_MAX_LENGTH, file);
		}
	}
}

/* CLOSE A FILE.
 * @param file:	Pointer to the file to close.
 * @return:		None.
 */
void FILE_close(FILE* file) {
	if (file != NULL) {
		fclose(file);
	}
}
