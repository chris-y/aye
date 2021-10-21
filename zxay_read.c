#include <stdint.h>
#include <stdio.h>
#include <malloc.h>

#include "zxay_read.h"

static int16_t readoffset(char *rel, int16_t addoffset)
{
	int16_t offset= 0;
	offset = (rel[0] << 8) | rel[1];
	if(offset == 0) return 0;
	
	offset += addoffset;
	return offset;
}

static int aye_read_string(FILE *fp, char *buffer)
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

static char *aye_read_generic(FILE *fp, int16_t offset)
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

static char *aye_read_author(FILE *fp, struct zxay_header *header)
{
	int16_t offset = readoffset(header->PAuthor, PAUTHOR_OFFSET);
	return aye_read_generic(fp, offset);
}

static char *aye_read_misc(FILE *fp, struct zxay_header *header)
{
	int16_t offset = readoffset(header->PMisc, PMISC_OFFSET);
	return aye_read_generic(fp, offset);
}

static char *aye_read_songname(FILE *fp, struct zxay_header *header, struct zxay_song *songstructure, int16_t song)
{
	songstructure += (sizeof(struct zxay_song) * song);
	int16_t songstructureoffset = readoffset(header->PSongsStructure, PSONGSSTRUCTURE_OFFSET + PSONGNAME_OFFSET);
	int16_t offset = readoffset(songstructure->PSongName, songstructureoffset + (sizeof(struct zxay_song) * song));

	return aye_read_generic(fp, offset);
}

static struct zxay_songdata *aye_read_songdata(FILE *fp, struct zxay_header *header, struct zxay_song *songstructure, int16_t song)
{
	struct zxay_songdata *songdata = NULL;
	size_t size = 0;
	
	songstructure += (sizeof(struct zxay_song) * song);
	int16_t songstructureoffset = readoffset(header->PSongsStructure, PSONGSSTRUCTURE_OFFSET + PSONGDATA_OFFSET);
	int16_t offset = readoffset(songstructure->PSongData, songstructureoffset + (sizeof(struct zxay_song) * song));

	songdata = malloc(sizeof(struct zxay_songdata));
	if(songdata == NULL) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		size = fread(songdata, sizeof(struct zxay_song), (header->NumOfSongs + 1), fp);
	
		if(size != (header->NumOfSongs + 1)) {
			printf("Error reading song data\n");
			free(songdata);
			return NULL;
		}
	}
	
	return songdata;
}

static struct zxay_song *aye_read_songstructure(FILE *fp, struct zxay_header *header)
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

static struct zxay_header *aye_read_header(FILE *fp)
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

static FILE *aye_open(char *filename)
{
	FILE *fp = 0;
	
	fp = fopen(filename, "r");
	if(fp == 0) return NULL;

	return fp;
}

static void aye_close(FILE *fp)
{
	fclose(fp);
}

void zxay_free(struct zxay_file *zxay)
{
	/* free song names */
	for(int i = 0; i <= zxay->header->NumOfSongs; i++) {
		if(zxay->songname[i]) free(zxay->songname[i]);
		if(zxay->songdata[i]) free(zxay->songdata[i]);
	}
	free(zxay->songname);
	free(zxay->songdata);
	free(zxay->songsstruct);
	free(zxay->songptrs);
	free(zxay->songdatablocks);
	free(zxay->misc);
	free(zxay->author);
	free(zxay->header);

	free(zxay);
}

struct zxay_file *zxay_load(char *filename)
{
	FILE *fp;
	struct zxay_file *zxay = NULL;

	struct zxay_header *header = NULL;
	struct zxay_song *songsstruct = NULL;
	char *author = NULL;
	char *misc = NULL;
	char **songname = NULL;
	struct zxay_songdata **songdata;
	struct zxay_songpointers **songptrs;
	struct zxay_songdatablock **songdatablocks;

	zxay = calloc(1, sizeof(struct zxay_file));

	if(zxay == NULL) return NULL;

	if(fp = aye_open(filename)) {
		if(zxay->header = aye_read_header(fp)) {
			zxay->author = aye_read_author(fp, zxay->header);
			printf("Author: %s\n", zxay->author);
			zxay->misc = aye_read_misc(fp, zxay->header);
			printf("Misc: %s\n", zxay->misc);
			
			printf("Tracks: %d\n", zxay->header->NumOfSongs + 1);
			
			zxay->songsstruct = aye_read_songstructure(fp, zxay->header);
			zxay->songname = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->songdata = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->songptrs = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->songdatablocks = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			
			if(zxay->songname && zxay->songdata && zxay->songptrs && zxay->songdatablocks) {
				for(int i = 0; i <= zxay->header->NumOfSongs; i++) {
					zxay->songname[i] = aye_read_songname(fp, zxay->header, zxay->songsstruct, i);
					printf(" %d: %s\n", i + 1, zxay->songname[i]);
					zxay->songdata[i] = aye_read_songdata(fp, zxay->header, zxay->songsstruct, i);
				}
			} else {
				printf("Unable to allocate memory\n");
			}
		} else {
			printf("Error reading header\n");
		}
		aye_close(fp);
	} else {
		printf("Error opening file\n");
	}

	return zxay;
	
}

