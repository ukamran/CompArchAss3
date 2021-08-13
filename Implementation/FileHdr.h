/*
FileHdr.h: Declares an external for the read_file function.
 ECED 3403
 10 August 21
*/
#pragma once

extern void read_file(char[], int, int);
extern void combine_lzlo(struct pubs pubs_table[], int total_pubs, struct externs externs_table[], int total_extern);
extern void parse_l3(struct lthrees table[], int);
extern void create_xme(char filename[]);
void append_file(char output[]);