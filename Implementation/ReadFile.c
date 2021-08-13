/*
ReadFile.c: Reads each file and generates a symbol table at the very end.
 ECED 3403
 21 August 09
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "FileHdr.h"

#define LSIZ 256 
#define RSIZ 256 
#define HEXARRSIZE 5
#define LENARRSIZE 3
#define MAXSYMBOLSIZE 50
#define MAXOFFSETSIZE 5
#define MAXTABLESIZE 20
#define MAXS1SIZE 100
//#define DEBUG
//int reloc_address=0;
int last_address;
int table_count = 0; //count for pubs
int extern_tcount = 0; //count for externs table
int lthree_tcount = 0; //count for L3 table

//this struct contains the symbols of L1 records
struct pubs
{
	char pub_symbol[MAXSYMBOLSIZE]; 
	int pub_symbol_addr;

};

//the L1 record symbols and addresses are collected in this array
struct pubs table[MAXTABLESIZE];

//this struct contains the symbols of L0 records. these are subsequently stored in a table.
struct externs {
	char extern_symbol[MAXSYMBOLSIZE];
	int extern_index;
};

struct externs e_table[MAXTABLESIZE];

//this struct contains the contents stored in L3 records. these are subsequently stored in a table.
struct lthrees {
	int lt_index;
	int lt_address;
	char lt_brbl;
};

struct lthrees lt_table[MAXTABLESIZE];


/*this function reads the input filename and parses the file searching for L1 records and S1 records.
* the symbol's addresses are also stored by adding the relocation address to the offset seen in the L1 record
*/
void read_file(char filename[], int filenum, int maxfiles) {
	FILE* file = fopen(filename, "r"); 

	char line[RSIZ][LSIZ];
	
	int i = 0;
	int tot = 0;
	
	//read and store the entire file
	while (fgets(line[i], sizeof(line[i]), file)) {
		/* insert a null terminator */
		// printf("%s", line);
		line[i][strlen(line[i])] = '\0';
		i++;
	}

	//set the last address to 0 if this is the first file
	if (filenum == 0) {
		last_address = 0;
	}

	tot = i;
	int publics = 0;
	int mod_st_address_found = 0;
	char mod_st_address_arr[HEXARRSIZE];
	char s1_address_arr[HEXARRSIZE];
	char reloc_addr_hex[HEXARRSIZE];
	char length_arr[LENARRSIZE];
	unsigned int s1_starting_address;
	int mod_starting_address;
	int num_bytes;
	int addr_cksum_bytes = 3;
	int interim_ra; //relocation address within a module relative to its starting address
	printf("\nFilename: %s  \n", filename);

	//read the entire file from the stored array
	for (i = 0; i < tot; i++)
	{
		//check for L1 records
		if (line[i][0] == 'L' && line[i][1] == '1') {
			char * token;
			token = strtok(line[i], " ");//extract l1 record and split it up into tokens
			// loop through the string to extract all other tokens
			char symbol[MAXSYMBOLSIZE];
			char offset_arr[MAXOFFSETSIZE];
			int offset;
			int offsetted_addr;
			//we need to extract the 2nd and 3rd tokens (name and offset)
			int token_count = 0;
			while (token != NULL) {
				
				//extract the symbol
				if (token_count == 1) {
					sscanf(token, "%s", symbol);
				}

				//extract the offset
				else if (token_count == 2) {
					sscanf(token, "%s", offset_arr);
				}
				token = strtok(NULL, " ");
				token_count++;
			}
			//null terminate offset array
			offset_arr[4] = 0;

			//convert the offset array into an int value
			offset = (int)strtol(offset_arr, NULL, 16);
#ifdef DEBUG
			printf("Offset array: %s\n", offset_arr);
			printf("Converted offset: %04x\n", offset);
			printf("Interim RA: %04x\n", interim_ra);
#endif // DEBUG

			offsetted_addr = last_address + interim_ra + offset;// this is the relocation address + the offset

			//store the symbol and the address in the table
			sscanf(symbol, "%s", table[table_count].pub_symbol);
			table[table_count].pub_symbol_addr = offsetted_addr;

			printf("Name: %s \t Offset: %04x \t Address with offset: %04x\n", symbol, offset, offsetted_addr);
			//printf("%s", line[i]);
			publics++;
			table_count++; //increment the table counter to point to the next row
		}

		//L0 record
		else if ((line[i][0] == 'L' && line[i][1] == '0')) {
			printf("L0 record: %s \n", line[i]);

			char* lztoken;
			lztoken = strtok(line[i], " ");//extract l1 record and split it up into tokens
			// loop through the string to extract all other tokens
			char lzsymbol[MAXSYMBOLSIZE];
			char lzindex_arr[HEXARRSIZE];
			//we need to extract the 2nd and 3rd tokens (name and offset)
			int lztoken_count = 0;
			int lzindex;
			while (lztoken != NULL) {

				//extract the symbol
				if (lztoken_count == 1) {
					sscanf(lztoken, "%s", lzsymbol);
				}

				//extract the index
				else if (lztoken_count == 2) {
					sscanf(lztoken, "%s", lzindex_arr);
				}
				lztoken = strtok(NULL, " ");
				lztoken_count++;
			}

			//null terminate the L0 array
			lzindex_arr[4] = 0;

			//convert the L0 array into an int value so that we can access hex equivalent
			lzindex = (int)strtol(lzindex_arr, NULL, 16);

			//store the symbol and the address in the table
			sscanf(lzsymbol, "%s", e_table[extern_tcount].extern_symbol);
			e_table[extern_tcount].extern_index = lzindex;
			
			//increment the table count to point to the next instance
			extern_tcount++;
		}

		//L3 record
		else if ((line[i][0] == 'L' && line[i][1] == '3')) {
			printf("L3 record: %s \n", line[i]);

			int lt_address;
			int lt_index;
			int lt_numbytes = 2;//always two bytes
			char lt_address_arr[] = { line[i][2], line[i][3], line[i][4], line[i][5],0 };
			char lt_brbl = line[i][6];
			char lt_index_arr[] = { line[i][7],line[i][8],line[i][9],0 };

			//convert the L3 address array into an int value so that we can access hex equivalent
			lt_address = (int)strtol(lt_address_arr, NULL, 16);
			lt_index = (int)strtol(lt_index_arr, NULL, 16);

			//store the symbol and the address in the table
			lt_table[lthree_tcount].lt_index = lt_index;
			lt_table[lthree_tcount].lt_address = lt_address;
			lt_table[lthree_tcount].lt_brbl = lt_brbl;

			//set the interim relocation address
			
			interim_ra = lt_address+lt_numbytes;
			//increment the table count to point to the next instance
			lthree_tcount++;
		}

		

		//extract the S1 record
		else if (line[i][0] == 'S' && line[i][1] == '1') {
			unsigned int new_s1_address;
			unsigned int new_checksum;
			unsigned int original_checksum;
			unsigned int lo_s1_sa;
			unsigned int hi_s1_sa;

			char new_s1_record[MAXS1SIZE];
			char new_s1_address_arr[HEXARRSIZE];
			char new_chksum_arr[LENARRSIZE]; //length and checksum bytes same size
			new_s1_address_arr[4] = 0;
			//extract the starting address of the module
			if (mod_st_address_found == 0) {
				for (int addr_count = 0; addr_count < HEXARRSIZE; addr_count++) {
					mod_st_address_arr[addr_count] = line[i][4 + addr_count]; //4 is the offset value.
				}
				//null terminate
				mod_st_address_arr[4] = 0;
				//convert the starting address from a string array to int val. add to the last address. print the resulting starting address of the module.
				mod_starting_address = (int)strtol(mod_st_address_arr, NULL, 16)+last_address;
				printf("Module starting address: %04x\n", mod_starting_address);
				mod_st_address_found = 1; //set the module's starting address to 1 now that it has been found

			}

			//extract the starting address of the S1 record
			for (int addr_count = 0; addr_count < HEXARRSIZE; addr_count++) {
				s1_address_arr[addr_count] = line[i][4 + addr_count]; //4 is the offset value on the s1 record to get to the address.
			}
			//null terminate
			s1_address_arr[4] = 0;
			
			//convert the starting address from a string array to int val
			s1_starting_address= (int)strtol(s1_address_arr, NULL, 16);

			//acquire length array to get the number of bytes
			for (int len_count = 0; len_count < LENARRSIZE; len_count++) {
				length_arr[len_count] = line[i][2 + len_count]; //need to change magic number. need to extract characters 2 & 3.
			}
			//null terminate
			length_arr[2] = 0;

			//covert to an int value
			num_bytes = (int)strtol(length_arr, NULL, 16) - addr_cksum_bytes;
#ifdef DEBUG
			printf("%s \n", length_arr);
			printf("Number of bytes: %02x\n", num_bytes);

#endif // DEBUG

			//get the relocation address relative to the record's starting address
			interim_ra = s1_starting_address + num_bytes;
			//reloc_address = reloc_address + last_address;
			new_s1_address = s1_starting_address + last_address; // this is the new relocation address
			
			//get the original checksum byte and convert it to an int value to be used as hex
			char orig_cksum_arr[] = { line[i][8 + (num_bytes * 2)],line[i][9 + (num_bytes * 2)],0 };
			original_checksum = ((int)strtol(orig_cksum_arr, NULL, 16)) & 0xff;

			//calculate the new checksum. checksum = ~(~(S1 chksum)+lo relocation address + hi relocation address)
			lo_s1_sa = (new_s1_address & 0xFF);
			hi_s1_sa= ((new_s1_address >> 8) & 0xFF);

			new_checksum = ((~(~(original_checksum)+lo_s1_sa + hi_s1_sa))&0xFF);
			
			//create the new s1 record
			strcpy(new_s1_record, line[i]);

			//replace the address
			sprintf(new_s1_address_arr, "%04x", new_s1_address);
			
			//too tired for better implementation
			new_s1_record[4] = new_s1_address_arr[0];
			new_s1_record[5] = new_s1_address_arr[1];
			new_s1_record[6] = new_s1_address_arr[2];
			new_s1_record[7] = new_s1_address_arr[3];

			//replace the checksum
			sprintf(new_chksum_arr, "%02x", new_checksum);

			new_s1_record[8 + (num_bytes * 2)] = new_chksum_arr[0];
			new_s1_record[9 + (num_bytes * 2)] = new_chksum_arr[1];

			//s1 record completed

			printf(" New S1: %s \n", new_s1_record);
			append_file(new_s1_record);
#ifdef DEBUG
			printf("Interim relocation address: %04x\n", interim_ra);

#endif // DEBUG

		}
		
		//printf("%s", line[i]);
	}

	//check to ensure the starting address is being appropriately printed from the array
#ifdef DEBUG
	printf("Starting address %s\n", mod_st_address_arr);
#endif

	//if there are no publics return the following diagnostic
	if (publics == 0) {
		printf("No publics. \n");
	}
	
	//at the end of the file, update the last address to the new last address
	last_address = interim_ra+last_address;
	interim_ra = 0; //need to reset the interim address for use by the next module
	//getchar();

	//normalize to count from 1 rather than 0
	int actualfilenum = filenum + 1;

	//create the symbol table once we have reached the last file
	if (actualfilenum == maxfiles) {
		printf("Symbol Table \n");
		printf("Symbol \t Address \n");
		printf("------ \t ------- \n");
		for (int numsymbols = 0; numsymbols < table_count; numsymbols++) {
			printf("%s \t %04x \n", table[numsymbols].pub_symbol, table[numsymbols].pub_symbol_addr);
		}

		combine_lzlo(table, table_count,e_table, extern_tcount); // create combo of L0 and L1
		parse_l3(lt_table, lthree_tcount); //create the S1 records from L3s
	}
	

	
	fclose(file);
}

