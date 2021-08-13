#include <stdio.h>
#include <stdlib.h>
#include "FileHdr.h"
#define MAXSYMBOLSIZE 50
#define MAXTABLESIZE 20
#define MAXS1SIZE 15 //max s1 size is based on a static length of 5
//#define DEBUG

//this struct contains the symbols of L1 records
struct pubs
{
	char pub_symbol[MAXSYMBOLSIZE];
	int pub_symbol_addr;

};

//this struct contains the symbols of L0 records. these are subsequently stored in a table.
struct externs {
	char extern_symbol[MAXSYMBOLSIZE];
	int extern_index;
};

//this struct contains symbols of L3 records
struct lthrees {
	int lt_index;
	int lt_address;
	char lt_brbl;
};

//this struct contains the symbols of L1 records
struct pubsexterns
{
	char symbol[MAXSYMBOLSIZE];
	int address;
	int index;
};

//the L1 record symbols and addresses are collected in this array
struct pubsexterns pubsexterns_table[MAXTABLESIZE];

//pubs externs table count
int ps_count = 0;

//combine L0 and L1 records
void combine_lzlo(struct pubs pubs_table[], int total_pubs, struct externs externs_table[], int total_externs) {
	int p_count;
	printf("Total Pubs: %d\t Total Externs: %d\n", total_pubs, total_externs);

	for (p_count = 0; p_count <= total_pubs; p_count++) {
		int e_count=0;
		do {
			int not_found;
			not_found = strcmp(pubs_table[p_count].pub_symbol, externs_table[e_count].extern_symbol);
			//printf("Not found value: %d\n", not_found);
			printf("%d\n", e_count);
			if (not_found == 0 && strcmp(pubs_table[p_count].pub_symbol,"")!=0) {
				printf("This is the pubs symbol: %s \t externs symbol: %s \t  and the table count:%d\n", pubs_table[p_count].pub_symbol,  externs_table[e_count].extern_symbol, ps_count);
				sscanf(pubs_table[p_count].pub_symbol, "%s", pubsexterns_table[ps_count].symbol); //store the symbol name in the table
				pubsexterns_table[ps_count].address = pubs_table[p_count].pub_symbol_addr; //store address
				pubsexterns_table[ps_count].index = externs_table[e_count].extern_index; //store the index
				ps_count++;//increment the table counter
			}

			e_count++;
		} while (e_count <= total_externs);

		e_count = 0;//reset extern count for the next public
	}

	printf("Pubs externs count: %d \n", ps_count);
	printf("Extern and Pubs Table \n");
	printf("Symbol \t Address \t Index \n");
	printf("------ \t ------- \n");
	for (int count_table = 0; count_table < ps_count; count_table++) {
		printf("%s \t %04x \t %04x \n", pubsexterns_table[count_table].symbol, pubsexterns_table[count_table].address, pubsexterns_table[count_table].index);
	}
}

//make the s1 from l3
void make_s1(int reloc_addr, int l3_addr, char blbra) {
	//reloc_addr = 276;
	//l3_addr = 256;
	printf("RA: %04x\t L3: %04x\n", reloc_addr,l3_addr);

	unsigned int offset;
	unsigned int encoded;
	unsigned int length = 5;
	unsigned int le_encoded; //little endian
	unsigned int le_low;
	unsigned int le_high;
	//unsigned int zle_encoded; //little endian
	//unsigned int tle_encoded; //little endian

	unsigned int low_addr;
	unsigned int high_addr;

	unsigned int checksum;
	char s1[MAXS1SIZE];

	low_addr = (l3_addr & 0xFF);
	high_addr = ((l3_addr >> 8) & 0xFF);

	printf("Low byte: %02x \t High byte: %02x \n", low_addr, high_addr);
	//calculate encoded offset
	offset = (unsigned int)reloc_addr - ((unsigned int)l3_addr + 2);
	printf("Offset: %04x\n", offset);
	encoded = offset >> 1; //right shift by one to get encoded offset
	printf("Encoded offset: %03x \n", encoded);

	if (blbra == '0') {
		le_encoded = encoded << 8; //left shift 4 bits to flip endianness of 3 byte hex value
		printf("With 0: %04x\n", le_encoded);
	}
	else if (blbra == '2') {
		le_encoded = encoded << 8;
		le_encoded = le_encoded + 2;
		printf("With 2: %04x\n", le_encoded);
	}
	else {
		printf("Incorrect bl/bra value.\n");
	}
	le_low= (le_encoded & 0xFF);
	le_high= ((le_encoded >> 8) & 0xFF);
	//calculate checksum
	checksum = ~(length + high_addr + low_addr + le_low + le_high)& 0xff;

	printf("Checksum: %02x \n", checksum);


	//create s1 record
	sprintf(s1, "S1%02X%04X%04x%02x", length, l3_addr, le_encoded, chUseecksum);

	printf("S1 Record: %s\n", s1);


	//append to xme file
	append_file(s1);
}
//parse the L3 records to create S1
void parse_l3(struct lthrees table[], int max_lthrees){

#ifdef DEBUG
	printf("Index \t Address \t BR/BL \n");
	printf("------\t ------- \t ----- \n");
	for (int count_table = 0; count_table < max_lthrees; count_table++) {
		printf("%04x \t %04x \t %c \n", table[count_table].lt_index, table[count_table].lt_address, table[count_table].lt_brbl);
	}
#endif // Ensure the array is passed through properly

	int lt_count;
	for (lt_count = 0; lt_count < max_lthrees; lt_count++) {
		int lzlo_count = 0;
		do {
			int not_found;
			int r_address;
			int l3_address;
			char blbra;

			//use index to map L3 with L0 and L1 records
			if (pubsexterns_table[lzlo_count].index==table[lt_count].lt_index) {
				
				//store relocation address, blbra character, and l3 address
				r_address = pubsexterns_table[lzlo_count].address;
				l3_address = table[lt_count].lt_address;
				blbra = table[lt_count].lt_brbl;

				//make the s1 record from l3
				make_s1(r_address, l3_address, blbra);
			}

			lzlo_count++;
		} while (lzlo_count < ps_count);

		lzlo_count = 0;//reset the counter for the next l3 record
	}


}

