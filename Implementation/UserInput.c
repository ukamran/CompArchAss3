/*
 UserInput.c: Prompts user input in order to acquire the xmo files to be parsed
 
 ECED 3403
 10 Aug 2021
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "FileHdr.h"


#define MAXROWS 100
#define MAXSTRLEN 100
#define MAXINPUT 4


void main()
{
	
	int num_modules;
	int count_modules;
	int input_length;
	char input[MAXINPUT] = "";
	int valid = 1;

	//prompt the user to select the number of xmo modules. type check for valid input.
	do {
		printf("Please select the number of xmo modules to input. This must be an integer value greater than 1: \n");
		scanf("%s", &input);
		getchar();

		input_length = strlen(input);
		for (int i = 0; i < input_length; i++) {
			//check each character to ensure that there isn't an erroneous non-numeric character. break if so.
			if (!isdigit(input[i]))
			{
				printf("Entered input is not a valid number. Try again.\n");
				valid = 0;
				break;
			}
			//ensure that only 1 file is selected
			else if(atoi(input)<=1) {
				printf("Must be more than 1 file. Try again.\n");
				valid = 0;
				break;
			}
			else {
				valid = 1;
			}
			
		}
	} while (valid != 1);
	num_modules = atoi(input);
	char filename_arr[MAXROWS][MAXSTRLEN];
	char single_filename[MAXSTRLEN];
	printf("Great! You have selected %d modules. The first file you input will be the main file. All files must be in the current directory. \n", num_modules);

	count_modules = 0;
	//requests the user to input valid xmo files. stores these in an array to be used next.
	do {
		printf("Please input the name of xmo file %d \n", count_modules+1);
		scanf("%s", &filename_arr[count_modules]);
		printf("This is the resulting value %s and count number %d \n", filename_arr[count_modules], count_modules);
		getchar();
		count_modules++;
	} while (count_modules < num_modules);
	
	//create an xme file using the first filename 
	create_xme(filename_arr[0]);

	//read each file. at the end, output the symbol table.
	int i;
	for (i = 0; i < count_modules; i++) {
		//printf("%s \n", filename_arr[i]);
		read_file(filename_arr[i],i, count_modules);
	}

	
	getchar();

} 