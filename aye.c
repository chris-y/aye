#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "zxay.h"

int16_t readoffset(char *rel, int16_t addoffset)
{
	int16_t offset= 0;
	offset = (rel[0] << 8) | rel[1];
	if(offset == 0) return 0;
	
	offset += addoffset;
	return offset;
}

int aye_read_string(FILE *fp, char *buffer)
{
	int chars = 0;
	char c;
	
	do {
		c = fgetc(fp);
		if(buffer != NULL) {
			*buffer = c;
			buffer++;
		}
		chars++;
	} while(c);
	
	return chars;
}

char *aye_read_generic(FILE *fp, int16_t offset)
{
	int chars = 0;
	char *string = NULL;
	
	if(offset == 0) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		chars = aye_read_string(fp, NULL);
	}
	
	if(chars != 0) {
		if(string = malloc(chars)) {
			if(fseek(fp, offset, SEEK_SET) == 0) {
				aye_read_string(fp, string);
			}
		}
	}
	
	return string;
}

char *aye_read_author(FILE *fp, struct zxay_header *header)
{
	int16_t offset = readoffset(header->PAuthor, PAUTHOR_OFFSET);
	return aye_read_generic(fp, offset);
}

char *aye_read_misc(FILE *fp, struct zxay_header *header)
{
	int16_t offset = readoffset(header->PMisc, PMISC_OFFSET);
	return aye_read_generic(fp, offset);
}

char *aye_read_songname(FILE *fp, struct zxay_header *header, struct zxay_song *songstructure, int16_t song)
{
	songstructure += (sizeof(struct zxay_song) * song);
	int16_t songstructureoffset = readoffset(header->PSongsStructure, PSONGSSTRUCTURE_OFFSET);
	int16_t offset = readoffset(songstructure->PSongName, songstructureoffset + (sizeof(struct zxay_song) * song));

	return aye_read_generic(fp, offset);
}

struct zxay_song *aye_read_songstructure(FILE *fp, struct zxay_header *header)
{
	size_t size = 0;
	struct zxay_song *songsstruct = NULL;
	int32_t bufsize = sizeof(struct zxay_song) * (header->NumOfSongs + 1);
	int16_t offset = readoffset(header->PSongsStructure, PSONGSSTRUCTURE_OFFSET);
	
	songsstruct = malloc(bufsize);
	if(songsstruct == NULL) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		size = fread(songsstruct, sizeof(struct zxay_song), (header->NumOfSongs + 1), fp);
	
		if(size != (header->NumOfSongs + 1)) {
			printf("Error reading songs\n");
			free(songsstruct);
			return NULL;
		}
	}

	return songsstruct;
}

struct zxay_header *aye_read_header(FILE *fp)
{
	size_t size = 0;
	struct zxay_header *header;
	
	header = malloc(sizeof(struct zxay_header));
	if(header == NULL) {
		printf("Error allocating memory\n");
		return NULL;
	}
	
	size = fread(header, sizeof(struct zxay_header), 1, fp);
	
	if(size != 1) {
		free(header);
		return NULL;
	}
	
	if((header->FileID[0] == 'Z') &&
		(header->FileID[1] == 'X') &&
		(header->FileID[2] == 'A') &&
		(header->FileID[3] == 'Y')) {
		/* ZXAY file */
		if((header->TypeID[0] == 'E') &&
			(header->TypeID[1] == 'M') &&
			(header->TypeID[2] == 'U') &&
			(header->TypeID[3] == 'L')) {
			/* ZXAY EMUL */
			return header;
		} else {
			printf("This is an unsupported ZXAY type %c%c%c%c\n",
				header->TypeID[0], header->TypeID[1], header->TypeID[2], header->TypeID[3]);
		}
	} else {
		printf("Not a ZXAY file!\n");
	}
	
	return NULL;
}

FILE *aye_open(char *filename)
{
	FILE *fp = 0;
	
	fp = fopen(filename, "r");
	if(fp == 0) return NULL;

	return fp;
}

void aye_close(FILE *fp)
{
	fclose(fp);
}

int main(int argc, char **argv)
{
	int c;
	FILE *fp;
	struct zxay_header *header = NULL;
	struct zxay_song *songsstruct = NULL;
	char *author = NULL;
	char *misc = NULL;
	char **songname = NULL;
	char *outfile = NULL;
	char *infile = NULL;

	printf("AYE 1.0\nZXAYEMUL metadata editor by Chris Young 2021\n\n");
		
	while ((c = getopt(argc, argv, "ho:")) != -1) {
		switch(c) {
			case 'h':
				printf("aye [OPTIONS] [INPUT]\n\n");
				printf("Options:\n  -h This help\n  -o Output file\n\n");
				return 0;
			break;
			case 'o':
				outfile = optarg;
			break;
		}	
	}
	
	for(c = optind; c < argc; c++) {
		infile = argv[c];
	}
	
	if(infile == NULL) {
		printf("No file specified\n\n");
		return 0;
	}
	
	if(fp = aye_open(infile)) {
		if(header = aye_read_header(fp)) {
			author = aye_read_author(fp, header);
			printf("Author: %s\n", author);
			misc = aye_read_misc(fp, header);
			printf("Misc: %s\n", misc);
			
			printf("Tracks: %d\n", header->NumOfSongs + 1);
			
			songsstruct = aye_read_songstructure(fp, header);
			songname = malloc((header->NumOfSongs + 1) * sizeof(char *));
			
			for(int i = 0; i <= header->NumOfSongs; i++) {
				songname[i] = aye_read_songname(fp, header, songsstruct, i);
				printf(" %d: %s\n", i + 1, songname[i]);
			}

			// TODO: free song names
			free(misc);
			free(author);
			free(header);
		} else {
			printf("Error reading header\n");
		}
		aye_close(fp);
	} else {
		printf("Error opening file\n");
	}
	
	//if(outfile) printf("%s\n", outfile);
	
	return 0;
}
