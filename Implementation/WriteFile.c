/*
 OutputFile.c: Creates a file output or appends to an existing file.
 ECED 3403
12 August 2021

*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "FileHdr.h"


char name_of_file[100];
unsigned int length_of_name;

//this function creates a file with an input filename and a prescreibed length of the filename
void create_xme(char filename[]) {
	// set the filename locally here
	int i=0;
	length_of_name = strlen(filename);
	//store the name of the file in the global variable for use by the append_file function
	do {
		name_of_file[i] = filename[i];
		i++;
	} while (filename[i] != '.');
	name_of_file[i] = '.';
	name_of_file[i+1] = 'x';
	name_of_file[i + 2] = 'm';
	name_of_file[i + 3] = 'e';


	//pointer to the file
	FILE* fp;
	//create the file with the set filename
	fp = fopen(filename, "w");
	//close the file
	fclose(fp);
}

void append_file(char output[]) {
	FILE* fp;
	//open the filename set in the global variable
	fp = fopen(name_of_file, "a");
	//append the char array output[] to the file  
	fprintf(fp, "%s", output);
	//close the file 
	fclose(fp);
}